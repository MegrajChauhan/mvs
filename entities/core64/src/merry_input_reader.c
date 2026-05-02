#include <merry_input_reader.h>

_MVS_ATTR_INTERNAL_ mbool_t merry_input_parse_header(MerryInput *inp,
                                                    msize_t flen) {
  // inp will be valid
  if (flen < 32)  {
  	// err
  	return mfalse;
  }
  mbyte_t chunk[8] = {0};
  MVSHostMemLayout dlen, ilen, dbg_len;
  dlen.whole_word = 0;
  ilen.whole_word = 0;
  dbg_len.whole_word = 0;

  memcpy(chunk, inp->file_ptr, 8);

  if (chunk[0] != 'M' || chunk[1] != 'I' || chunk[2] != 'F') {
    mvs_log_err("Failed to Identify Input File: IDENTIFICATION bytes"
         "=%c%c%c",
         chunk[0], chunk[1], chunk[2]);
    return mfalse;
  }

  if (chunk[3] != _MERRY_BYTE_ORDER_) {
    // MERR("Mismatched ENDIANNESS. The host and the input file must have the "
    //      "same endianness.",
    //      NULL);
    return mfalse;
  }
  
  // Now time for the lengths
  memcpy(chunk, inp->file_ptr + 8, 8);
  
  ilen.bytes.b0 = chunk[0];
  ilen.bytes.b1 = chunk[1];
  ilen.bytes.b2 = chunk[2];
  ilen.bytes.b3 = chunk[3];
  ilen.bytes.b4 = chunk[4];
  ilen.bytes.b5 = chunk[5];
  ilen.bytes.b6 = chunk[6];
  ilen.bytes.b7 = chunk[7];

  if (ilen.whole_word == 0) {
    // MERR("No instructions provided: Instruction section length is 0", NULL);
    return mfalse;  
  }
  // Must be divisible by 8
  if (ilen.whole_word % 8 != 0) {
    // MERR("Mis-aligned instruction section length: %zu is not divisible by 8",
         // ilen.whole_word);
    return mfalse;;
  }
  // MDBG("Intruction section: Length=%zu BYTES", ilen.whole_word);
  memcpy(chunk, inp->file_ptr + 16, 8);

  dlen.bytes.b0 = chunk[0];
  dlen.bytes.b1 = chunk[1];
  dlen.bytes.b2 = chunk[2];
  dlen.bytes.b3 = chunk[3];
  dlen.bytes.b4 = chunk[4];
  dlen.bytes.b5 = chunk[5];
  dlen.bytes.b6 = chunk[6];
  dlen.bytes.b7 = chunk[7];

  // MDBG("Data section: Length=%zu BYTES", dlen.whole_word);

  memcpy(chunk, inp->file_ptr + 24, 8);

  dbg_len.bytes.b0 = chunk[0];
  dbg_len.bytes.b1 = chunk[1];
  dbg_len.bytes.b2 = chunk[2];
  dbg_len.bytes.b3 = chunk[3];
  dbg_len.bytes.b4 = chunk[4];
  dbg_len.bytes.b5 = chunk[5];
  dbg_len.bytes.b6 = chunk[6];
  dbg_len.bytes.b7 = chunk[7];

  // MDBG("Debug section: Length=%zu BYTES", dbg_len.whole_word);

  if ((ilen.whole_word + dlen.whole_word + dbg_len.whole_word) > flen) {
    // MERR("Input file header's information doesn't match with what was read",
    //      NULL);
    return mfalse;
  }

  inp->data_len = dlen.whole_word;
  inp->instruction_len = ilen.whole_word;
  // MDBG("Header Parsing Succeded", NULL);
  return mtrue;
}

MerryInput *merry_input_init() {
  // MDBG("Initializing Input Reader", NULL);
  MerryInput *inp = (MerryInput *)malloc(sizeof(MerryInput));
  if (!inp) {
    // MERR("Failed to allocate memory for input reader", NULL);
    return NULL;
  }
  if (mvs_mapped_memory_create(&inp->mapped, 0) != MRES_SUCCESS) {
  	// err
  	free(inp);
  	return NULL;
  }
  mvs_mapped_memory_add_align_param(inp->mapped, _MERRY_CORE_PAGE_LEN_IN_BYTES_);
  inp->data = NULL;
  inp->data_len = 0;
  inp->input_file = NULL;
  inp->instruction_len = 0;
  inp->instructions = NULL;
  inp->file_ptr = NULL;
  // MDBG("Successfully initialized input reader", NULL);
  return inp;
}

mbool_t merry_input_read(MerryInput *inp, mstr_t path) {
  // MDBG("Reading Input File %s", path);
  
  if (mvs_mapped_memory_mapf(inp->mapped, path, 0, MVS_MMAPPED_FLAG_READ | MVS_MMAPPED_FLAG_WRITE | MVS_MMAPPED_FLAG_PRIVATE | MVS_MMAPPED_FLAG_ALIGN) != MRES_SUCCESS) {
    // MDBG("Failed to initialize input reader", NULL);
    return mfalse;
  }
  if (mvs_mapped_memory_obtain_ptr(inp->mapped, &inp->file_ptr, 0) != MRES_SUCCESS) {
    // debugging!!!
  	return mfalse;
  }
  if (! merry_input_parse_header(inp, mvs_mapped_memory_obtain_map_size(inp->mapped))) {
    // MERR("While parsing input file: PATH=%s", path);
    return mfalse;
  }

  inp->data_len += _MERRY_CORE_PAGE_LEN_IN_BYTES_ - (inp->data_len % _MERRY_CORE_PAGE_LEN_IN_BYTES_);
  inp->instructions = inp->file_ptr + 32;
  inp->data = inp->instructions + inp->instruction_len;  

  // MDBG("Read Input file '%s' successfully", path);
  return mtrue;
}

void merry_input_destroy(MerryInput *inp) {
  if (!inp)
    return;

  mvs_mapped_memory_unmap(inp->mapped);
  mvs_mapped_memory_destroy(inp->mapped);
  free(inp);
  // MDBG("Input Reader Destroyed", NULL);
}
