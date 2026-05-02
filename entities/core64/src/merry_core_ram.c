#include <merry_core_ram.h>

MerryCoreRAM *merry_core_memory_init() {
  MerryCoreRAM *mem = (MerryCoreRAM *)malloc(sizeof(MerryCoreRAM));
  if (!mem) {
    mvs_log_err("Core64: Failed to allocate memory for Core RAM");
    return RET_NULL;
  }
  mem->pg_list = NULL;
  mem->max_address = 0;
  return mem;
}

mbool_t merry_core_memory_populate(MerryCoreRAM *mem, msize_t space_len,
                                     mbptr_t addr_space) {
  msize_t pg_count =
      space_len /
      _MERRY_CORE_PAGE_LEN_IN_BYTES_; // space_len should be aligned most of the
                                      // time until instruction len is extremely
                                      // small
  if (space_len && (space_len < _MERRY_CORE_PAGE_LEN_IN_BYTES_))
    pg_count = 1;
  mResult_t res = mvs_dynamic_listl_create(&mem->pg_list, pg_count, sizeof(MerryCoreRAMPage));
  if (res != mtrue) {
    mvs_log_err("Core64: RAM Page Buffer allocation failed[PGC=%zu]", pg_count);
    return mfalse;
  }

  mem->max_address = space_len;

  for (msize_t i = 0; i < pg_count; i++) {
    MerryCoreRAMPage *pg;
    mvs_dynamic_listl_ref_of(mem->pg_list, &pg, i);
    pg->repr.bytes = addr_space + i * _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  }
  return mtrue;
}

mbool_t merry_core_memory_read_byte(MerryCoreRAM *mem, maddress_t addr,
                                      mbptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
  	mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  *store_in = ((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_];
  return mtrue;
}

mbool_t merry_core_memory_read_word(MerryCoreRAM *mem, maddress_t addr,
                                      mwptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 2) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *store_in =
     *(mwptr_t)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_));
  return mtrue;
}

mbool_t merry_core_memory_read_dword(MerryCoreRAM *mem, maddress_t addr,
                                       mdptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 4) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *store_in =
     *(mdptr_t)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_));
  return mtrue;
}

mbool_t merry_core_memory_read_qword(MerryCoreRAM *mem, maddress_t addr,
                                       mqptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 8) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *store_in =
     *(mqptr_t)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_));
  return mtrue;
}

mbool_t merry_core_memory_read_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mbptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  *store_in = atomic_load_explicit(
      (_Atomic mbyte_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      memory_order_acquire);
  return mtrue;
}

mbool_t merry_core_memory_read_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mwptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 2) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *store_in = atomic_load_explicit(
      (_Atomic mword_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      memory_order_acquire);
  return mtrue;
}

mbool_t merry_core_memory_read_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mdptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 4) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *store_in = atomic_load_explicit(
      (_Atomic mdword_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      memory_order_acquire);
  return mtrue;
}

mbool_t merry_core_memory_read_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mqptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 8) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *store_in = atomic_load_explicit(
      (_Atomic mqword_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      memory_order_acquire);
  return mtrue;
}

mbool_t merry_core_memory_read_bulk(MerryCoreRAM *mem, maddress_t addr,
                                      msize_t len, mbptr_t store_in) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF(!len))
    return mtrue;
  register maddress_t end = addr + len;
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  register msize_t pg = addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t diff = _MERRY_CORE_PAGE_LEN_IN_BYTES_ - off;
  mbptr_t iter = store_in;

  //////  FIX THIS SHIT
  if (diff > len) {
    memcpy(store_in, ((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,pg))->repr.bytes + off, len);
  } else {
    memcpy(store_in, ((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,pg))->repr.bytes + off, diff);
    iter += diff;
    len -= diff;
    pg++;
    off = 0;

    while (len > 0) {
      diff = (len < _MERRY_CORE_PAGE_LEN_IN_BYTES_)
                 ? len
                 : _MERRY_CORE_PAGE_LEN_IN_BYTES_;
      memcpy(iter, ((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,pg))->repr.bytes, diff);
      iter += diff;
      len -= diff;
      pg++;
    }
  }

  return mtrue;
}

mbool_t merry_core_memory_write_byte(MerryCoreRAM *mem, maddress_t addr,
                                       mbyte_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  ((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_] = store;
  return mtrue;
}

mbool_t merry_core_memory_write_word(MerryCoreRAM *mem, maddress_t addr,
                                       mword_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 2) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *(mwptr_t)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)) = store;
  return mtrue;
}

mbool_t merry_core_memory_write_dword(MerryCoreRAM *mem, maddress_t addr,
                                        mdword_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 4) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *(mdptr_t)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)) = store;
  return mtrue;
}

mbool_t merry_core_memory_write_qword(MerryCoreRAM *mem, maddress_t addr,
                                        mqword_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 4) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  *(mqptr_t)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)) = store;
  return mtrue;
}

mbool_t merry_core_memory_write_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mbyte_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  atomic_store_explicit(
      (_Atomic mbyte_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                           mword_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 2) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  atomic_store_explicit(
      (_Atomic mword_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                            mdword_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 4) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  atomic_store_explicit(
      (_Atomic mdword_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                            mqword_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF((addr % 8) != 0)) {
    mvs_log_err("Core64: Misaligned memory access[addr=%zu]", addr);
    return mfalse; 
  }
  atomic_store_explicit(
      (_Atomic mqword_t
           *)(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_)),
      store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_bulk(MerryCoreRAM *mem, maddress_t addr,
                                       msize_t len, mbptr_t store) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  if (surelyF(!len))
    return mtrue; // this shouldn't really happen though
  register maddress_t end = addr + len;
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  register msize_t pg = addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t diff = _MERRY_CORE_PAGE_LEN_IN_BYTES_ - off;
  mbptr_t iter = store;
  if (diff > len) {
    memcpy(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,pg))->repr.bytes + off, store, len);
  } else {
    memcpy(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,pg))->repr.bytes + off, store, diff);
    iter += diff;
    len -= diff;
    pg++;
    off = 0;

    while (len > 0) {
      diff = (len < _MERRY_CORE_PAGE_LEN_IN_BYTES_)
                 ? len
                 : _MERRY_CORE_PAGE_LEN_IN_BYTES_;
      memcpy(((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,pg))->repr.bytes, iter, diff);
      iter += diff;
      len -= diff;
      pg++;
    }
  }

  return mtrue;
}

mbool_t merry_core_memory_cmpxchg(MerryCoreRAM *mem, maddress_t addr,
                                    mbyte_t exp, mbyte_t des) {
  if (surelyF(addr >= mem->max_address)){
    mvs_log_err("Core64: Out of Bounds Access[addr=%zu]", addr);
    return mfalse;
  }
  atomic_compare_exchange_strong_explicit(
      (_Atomic mbyte_t *)((MerryCoreRAMPage*)mvs_dynamic_listl_ref_of_unsafe(mem,addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_))->repr.bytes + (addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_),
      &exp, des, memory_order_relaxed,memory_order_relaxed);
  return mtrue;
}

void merry_core_memory_destroy(MerryCoreRAM *mem) {
  // The pages and the memory buffer is none of RAM's concern
  mvs_dynamic_listl_destroy(mem->pg_list);
  free(mem);
}
