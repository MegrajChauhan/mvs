#include <merry_input_reader.h>

_MVS_ATTR_INTERNAL_ mbool_t merry_input_parse_header(MerryInput *inp) {
  if (inp->flen < 48) {
    MERRY_ERR("Invalid File Structure. File Len is just %zu", inp->flen);
    return mfalse;
  }

  MVSHostMemLayout dlen, ilen, dbg_len, entry_point;
  dlen.whole_word = 0;
  ilen.whole_word = 0;
  dbg_len.whole_word = 0;
  // If the IO operations performed here ever fail, the only reason would be
  // INTERFACE_HOST_FAILURE since the interface should be perfectly configured.
  mbptr_t tmp = inp->input_file;

  if (tmp[0] != 'M' || tmp[1] != 'I' || tmp[2] != 'F') {
    MERRY_ERR(
        "Unknown Input File Type received: The IDENTIFICATION bytes 'MIF' "
        "expected but got %c%c%c",
        tmp[0], tmp[1], tmp[2]);
    return mfalse;
  }

  if (tmp[3] != _MVS_HOST_ARCH_BYTE_ORDER_) {
    MERRY_ERR(
        "Mismatched ENDIANNESS. The host and the input file must have the "
        "same endianness.");
    return mfalse;
  }
  tmp += 4;

  ilen.bytes.b0 = tmp[0];
  ilen.bytes.b1 = tmp[1];
  ilen.bytes.b2 = tmp[2];
  ilen.bytes.b3 = tmp[3];
  ilen.bytes.b4 = tmp[4];
  ilen.bytes.b5 = tmp[5];
  ilen.bytes.b6 = tmp[6];
  ilen.bytes.b7 = tmp[7];

  if (ilen.whole_word == 0) {
    MERRY_ERR("No instructions provided: Instruction section length is 0");
    return mfalse;
  }
  // Must be divisible by 8
  if (ilen.whole_word % 8 != 0) {
    MERRY_ERR(
        "Mis-aligned instruction section length: %zu is not divisible by 8",
        ilen.whole_word);
    return mfalse;
  }

  tmp += 8;

  dlen.bytes.b0 = tmp[0];
  dlen.bytes.b1 = tmp[1];
  dlen.bytes.b2 = tmp[2];
  dlen.bytes.b3 = tmp[3];
  dlen.bytes.b4 = tmp[4];
  dlen.bytes.b5 = tmp[5];
  dlen.bytes.b6 = tmp[6];
  dlen.bytes.b7 = tmp[7];

  tmp += 8;

  dbg_len.bytes.b0 = tmp[0];
  dbg_len.bytes.b1 = tmp[1];
  dbg_len.bytes.b2 = tmp[2];
  dbg_len.bytes.b3 = tmp[3];
  dbg_len.bytes.b4 = tmp[4];
  dbg_len.bytes.b5 = tmp[5];
  dbg_len.bytes.b6 = tmp[6];
  dbg_len.bytes.b7 = tmp[7];

  tmp += 8;

  entry_point.bytes.b0 = tmp[0];
  entry_point.bytes.b1 = tmp[1];
  entry_point.bytes.b2 = tmp[2];
  entry_point.bytes.b3 = tmp[3];
  entry_point.bytes.b4 = tmp[4];
  entry_point.bytes.b5 = tmp[5];
  entry_point.bytes.b6 = tmp[6];
  entry_point.bytes.b7 = tmp[7];

  if ((ilen.whole_word + dlen.whole_word + dbg_len.whole_word) > inp->flen) {
    MERRY_ERR(
        "Input file header's information doesn't match with what was read");
    return mfalse;
  }
  if (entry_point.whole_word >= ilen.whole_word) {
    MERRY_ERR("Invalid entry point provided %X", entry_point.whole_word);
    return mfalse;
  }

  inp->data_len = dlen.whole_word;
  inp->instruction_len = ilen.whole_word;
  inp->entry = entry_point.whole_word;
  return mtrue;
}

MerryInput *merry_input_init() {
  MerryInput *inp = (MerryInput *)malloc(sizeof(MerryInput));
  if (!inp) {
    MERRY_ERR("Failed to allocate memory for input reader");
    return NULL;
  }
  if (mvs_mapped_memory_create(&inp->mapped) !=
      MRES_SUCCESS) {
    MERRY_ERR("Failed to initialize memory for input reader");
    free(inp);
    return NULL;
  }
  inp->data = NULL;
  inp->data_len = 0;
  inp->input_file = NULL;
  inp->instruction_len = 0;
  inp->instructions = NULL;
  return inp;
}

mbool_t merry_input_read(MerryInput *inp, mstr_t path) {
  if (mvs_mapped_memory_mapf(inp->mapped, path, 0,
                             MVS_MMAPPED_FLAG_READ | MVS_MMAPPED_FLAG_WRITE |
                                 MVS_MMAPPED_FLAG_PRIVATE) != MRES_SUCCESS) {
    MERRY_ERR("Failed to allocate memory for input file %s", path);
    return mfalse;
  }

  mvs_mapped_memory_obtain_ptr(inp->mapped, (mbptr_t *)(&inp->input_file), 0);
  mvs_mapped_memory_obtain_map_size(inp->mapped, &inp->flen);

  if (!merry_input_parse_header(inp)) {
    MERRY_ERR("While parsing input file %s", path);
	return mfalse;
  }

  inp->data_len =
      mvs_align_value(inp->data_len, _MERRY_CORE_PAGE_LEN_IN_BYTES_);

  // Now finally allocate the memory
  msize_t total_len = inp->data_len + inp->instruction_len + 32;

  mvs_mapped_memory_obtain_ptr(inp->mapped, &inp->instructions, 40);

  mvs_mapped_memory_obtain_ptr(inp->mapped, &inp->data,
                               40 + inp->instruction_len);

  return mtrue;
}

void merry_input_destroy(MerryInput *inp) {
  if (!inp)
    return;

  mvs_mapped_memory_destroy(inp->mapped);
  free(inp);
}
