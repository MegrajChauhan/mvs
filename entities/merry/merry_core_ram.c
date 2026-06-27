#include <merry_core_ram.h>

#define _MERRY_MEM_PAGE_ACCESS_(mem, addr)                                     \
  (MerryCoreRAMPage *)mvs_dynamic_listl_ref_of_unsafe(                         \
      (mem)->pg_list, (addr) / _MERRY_CORE_PAGE_LEN_IN_BYTES_)

MerryCoreRAM *merry_core_memory_init() {
  MerryCoreRAM *mem = (MerryCoreRAM *)malloc(sizeof(MerryCoreRAM));
  if (!mem) {
    MERRY_ERR("Failed to allocate memory for Core RAM");
    return NULL;
  }
  mem->pg_list = NULL;
  mem->max_address = 0;
  mem->err = MEM_ERR_NONE;
  return mem;
}

mbool_t merry_core_memory_populate(MerryCoreRAM *mem, msize_t space_len,
                                   mbptr_t addr_space) {
  msize_t pg_count =
      space_len /
      _MERRY_CORE_PAGE_LEN_IN_BYTES_; // space_len should be aligned most of the
                                      // time unless instruction len is
                                      // extremely small
  if (space_len && (space_len < _MERRY_CORE_PAGE_LEN_IN_BYTES_))
    pg_count = 1;
  mResult_t res = mvs_dynamic_listl_create(&mem->pg_list, pg_count,
                                           sizeof(MerryCoreRAMPage));
  if (res != MRES_SUCCESS) {
    MERRY_ERR("Failed to allocate memory for RAM");
    return mfalse;
  }
  if (space_len >= _MERRY_CORE_PAGE_LEN_IN_BYTES_) {
    for (msize_t i = 0; i < pg_count; i++) {
      MerryCoreRAMPage pg;
      pg.repr.bytes = addr_space + i * _MERRY_CORE_PAGE_LEN_IN_BYTES_;
      pg.independent_page = mfalse;
      if ((res = mvs_dynamic_listl_push(mem->pg_list, &pg)) != MRES_SUCCESS) {
        MERRY_ERR("Failed to populate RAM");
        mvs_dynamic_listl_destroy(mem->pg_list);
        mem->pg_list = NULL;
        return mfalse;
      }
    }
  }
  if (space_len % _MERRY_CORE_PAGE_LEN_IN_BYTES_) {
    msize_t remaining = space_len % _MERRY_CORE_PAGE_LEN_IN_BYTES_;
    MVSMappedMemory *pg;
    if (mvs_mapped_memory_create(&pg, MVS_INTERFACE_CONF_SHAREABLE) !=
        MRES_SUCCESS) {
      MERRY_ERR("Failed to populate RAM");
      mvs_dynamic_listl_destroy(mem->pg_list);
      mem->pg_list = NULL;
      return mfalse;
    }
    if (mvs_mapped_memory_map(pg, _MERRY_CORE_PAGE_LEN_IN_BYTES_) !=
        MRES_SUCCESS) {
      MERRY_ERR("Failed to populate RAM");
      mvs_dynamic_listl_destroy(mem->pg_list);
      mvs_mapped_memory_destroy(pg);
      mem->pg_list = NULL;
      return mfalse;
    }
    // now copy the contents
    MerryCoreRAMPage page;
    mvs_mapped_memory_obtain_ptr(pg, &page.repr.bytes, 0);
    memcpy(page.repr.bytes,
           addr_space + (pg_count)*_MERRY_CORE_PAGE_LEN_IN_BYTES_,
           (space_len % _MERRY_CORE_PAGE_LEN_IN_BYTES_));
    page.independent_page = mtrue;
    page.mapped_page = pg;
    if ((res = mvs_dynamic_listl_push(mem->pg_list, &page)) != MRES_SUCCESS) {
      MERRY_ERR("Failed to populate RAM");
      mvs_dynamic_listl_destroy(mem->pg_list);
      mvs_mapped_memory_destroy(pg);
      mem->pg_list = NULL;
      return mfalse;
    }
  }
  mem->max_address = space_len;
  return mtrue;
}

mbool_t merry_core_memory_read_byte(MerryCoreRAM *mem, maddress_t addr,
                                    mbptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in = (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                  ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_];
  return mtrue;
}

mbool_t merry_core_memory_read_word(MerryCoreRAM *mem, maddress_t addr,
                                    mwptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in =
      *(mwptr_t) & (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                       ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_];
  return mtrue;
}

mbool_t merry_core_memory_read_dword(MerryCoreRAM *mem, maddress_t addr,
                                     mdptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in =
      *(mdptr_t) & (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                       ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_];
  return mtrue;
}

mbool_t merry_core_memory_read_qword(MerryCoreRAM *mem, maddress_t addr,
                                     mqptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in =
      *(mqptr_t) & (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                       ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_];
  return mtrue;
}

mbool_t merry_core_memory_read_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                        mbptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in = atomic_load_explicit(
      (_Atomic mbyte_t *)(&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                               ->repr
                               .bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_]),
      memory_order_relaxed);
  return mtrue;
}

mbool_t merry_core_memory_read_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                        mwptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in = atomic_load_explicit(
      (_Atomic mword_t *)(&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                               ->repr
                               .bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_]),
      memory_order_relaxed);
  return mtrue;
}

mbool_t merry_core_memory_read_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mdptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in = atomic_load_explicit(
      (_Atomic mdword_t *)(&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                                ->repr
                                .bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_]),
      memory_order_relaxed);
  return mtrue;
}

mbool_t merry_core_memory_read_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mqptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *store_in = atomic_load_explicit(
      (_Atomic mqword_t *)(&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                                ->repr
                                .bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_]),
      memory_order_relaxed);
  return mtrue;
}

mbool_t merry_core_memory_read_bulk(MerryCoreRAM *mem, maddress_t addr,
                                    msize_t len, mbptr_t store_in) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  if (surelyF(!len))
    return mtrue;
  register maddress_t end = addr + len;
  if (surelyF(end >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  register msize_t pg = addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t to_read_this_page = _MERRY_CORE_PAGE_LEN_IN_BYTES_ - off;
  register msize_t end_page = (end / _MERRY_CORE_PAGE_LEN_IN_BYTES_);
  mbptr_t iter = store_in;

  for (msize_t i = pg; i <= end_page; i++) {
    memcpy(iter, (_MERRY_MEM_PAGE_ACCESS_(mem, addr))->repr.bytes + off,
           to_read_this_page);
    iter += to_read_this_page;
    off = 0;
    addr += to_read_this_page;
    len -= to_read_this_page;
    if (len < _MERRY_CORE_PAGE_LEN_IN_BYTES_)
      to_read_this_page = len;
    else
      to_read_this_page = _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  }
  return mtrue;
}

mbool_t merry_core_memory_write_byte(MerryCoreRAM *mem, maddress_t addr,
                                     mbyte_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *(mbptr_t) & (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                   ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_] = store;
  return mtrue;
}

mbool_t merry_core_memory_write_word(MerryCoreRAM *mem, maddress_t addr,
                                     mword_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *(mqptr_t) & (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                   ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_] = store;
  return mtrue;
}

mbool_t merry_core_memory_write_dword(MerryCoreRAM *mem, maddress_t addr,
                                      mdword_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *(mdptr_t) & (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                   ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_] = store;
  return mtrue;
}

mbool_t merry_core_memory_write_qword(MerryCoreRAM *mem, maddress_t addr,
                                      mqword_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  *(mqptr_t) & (_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                   ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_] = store;
  return mtrue;
}

mbool_t merry_core_memory_write_byte_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mbyte_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  atomic_store_explicit((_Atomic mbyte_t *)&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                            ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_],
                        store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_word_atm(MerryCoreRAM *mem, maddress_t addr,
                                         mword_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  atomic_store_explicit((_Atomic mword_t *)&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
                            ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_],
                        store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_dword_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mdword_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  atomic_store_explicit(
      (_Atomic mdword_t *)&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
          ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_qword_atm(MerryCoreRAM *mem, maddress_t addr,
                                          mqword_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  atomic_store_explicit(
      (_Atomic mqword_t *)&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
          ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_],
      store, memory_order_release);
  return mtrue;
}

mbool_t merry_core_memory_write_bulk(MerryCoreRAM *mem, maddress_t addr,
                                     msize_t len, mbptr_t store) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  if (surelyF(!len))
    return mtrue;
  register maddress_t end = addr + len;
  if (surelyF(end >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  register msize_t pg = addr / _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t off = addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  register msize_t to_write_this_page = _MERRY_CORE_PAGE_LEN_IN_BYTES_ - off;
  register msize_t end_page = (end / _MERRY_CORE_PAGE_LEN_IN_BYTES_);
  mbptr_t iter = store;

  for (msize_t i = pg; i <= end_page; i++) {
    memcpy((_MERRY_MEM_PAGE_ACCESS_(mem, addr))->repr.bytes + off, iter,
           to_write_this_page);
    iter += to_write_this_page;
    off = 0;
    addr += to_write_this_page;
    len -= to_write_this_page;
    if (len < _MERRY_CORE_PAGE_LEN_IN_BYTES_)
      to_write_this_page = len;
    else
      to_write_this_page = _MERRY_CORE_PAGE_LEN_IN_BYTES_;
  }
  return mtrue;
}

mbool_t merry_core_memory_cmpxchg(MerryCoreRAM *mem, maddress_t addr,
                                  mbyte_t exp, mbyte_t des) {
  if (surelyF(addr >= mem->max_address)) {
    mem->err = MEM_ERR_OUT_OF_BOUNDS_ACCESS;
    return mfalse;
  }
  atomic_compare_exchange_strong(
      (_Atomic mbyte_t *)&(_MERRY_MEM_PAGE_ACCESS_(mem, addr))
          ->repr.bytes[addr % _MERRY_CORE_PAGE_LEN_IN_BYTES_],
      &exp, des);
  return mtrue;
}

void merry_core_memory_destroy(MerryCoreRAM *mem) {
  if (!mem)
    return;
  if (mem->pg_list) {
    for (msize_t i = 0; i < mvs_dynamic_listl_size_unsafe(mem->pg_list); i++) {
      MerryCoreRAMPage *pg =
          (MerryCoreRAMPage *)mvs_dynamic_listl_ref_of_unsafe(mem->pg_list, i);
      if (pg->independent_page)
        mvs_mapped_memory_destroy(pg->mapped_page);
    }
    mvs_dynamic_listl_destroy(mem->pg_list);
  }
  free(mem);
}
