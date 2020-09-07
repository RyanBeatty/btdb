#ifndef STORAGE_H
#define STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "types.h"
#include "utils.h"

typedef struct ColDesc {
  const char* column_name;
  BType type;
} ColDesc;

typedef struct TableDef {
  const char* name;
  ColDesc* tuple_desc;  // sbarr
  size_t index;
} TableDef;

TableDef* MakeTableDef(const char*, ColDesc*, size_t);

extern TableDef* TableDefs;  // sbarr

TableDef* FindTableDef(const char*);
BType GetColType(TableDef*, const char*);

typedef struct DataLoc {
  size_t offset;
  size_t length;
} DataLoc;

typedef struct TupleId {
  uint16_t page_num;
  uint16_t loc_num;
} TupleId;

typedef struct Tuple {
  size_t length;
  TupleId self_tid;  // Points to either item loc for this tuple or (in the case of updates)
                     // item loc for 'next' tuple.
  size_t num_cols;
  byte null_bitmap[];
} Tuple;

#define GetDataLocs(tuple) \
  (DataLoc*)((byte*)tuple + sizeof(Tuple) + tuple->num_cols * sizeof(byte))

#define GetDataPtr(tuple)                                         \
  (byte*)tuple + sizeof(Tuple) + tuple->num_cols * sizeof(byte) + \
      tuple->num_cols * sizeof(DataLoc)

#define TupleGetSize(tuple) tuple->length

Tuple* MakeTuple(TableDef*);
size_t GetColIdx(Tuple*, const char*, TableDef*, bool*);
Datum GetCol(Tuple*, const char*, TableDef*);
Tuple* SetCol(Tuple*, const char*, Datum, TableDef*);
Tuple* CopyTuple(Tuple*);

typedef Tuple** Table;

extern Table* Tables;  // sbarr

void CreateTable(TableDef*);

void InitSystemTables();

#define PAGE_SIZE 8192

typedef unsigned char* Page;

typedef struct ItemLoc {
  uint16_t offset;
  uint16_t length;
  // TODO: Make bitmap
  bool dead;
} ItemLoc;

typedef struct PageHeader {
  uint16_t free_lower_offset;
  uint16_t free_upper_offset;
  uint16_t num_locs;
  ItemLoc item_locs[];
} PageHeader;

#define GetPageHeader(page) ((PageHeader*)page)
#define GetPageNextLocNum(ptr) GetPageHeader(ptr)->num_locs
#define PageGetNumLocs(ptr) GetPageHeader(ptr)->num_locs
#define PageGetItemLoc(ptr, i) GetPageHeader(ptr)->item_locs[i]

void PageInit(Page);
uint16_t PageGetFreeStart(Page);
bool PageAddItem(Page, unsigned char*, size_t);
unsigned char* PageGetItem(Page, size_t);
uint16_t PageGetItemSize(Page, size_t);
void PageDeleteItem(Page, size_t);

typedef struct Cursor {
  size_t table_index;
  size_t page_index;
  size_t tuple_index;
} Cursor;

void CursorInit(Cursor*, TableDef*);
Tuple* CursorSeekNext(Cursor*);
// Tuple* CursorPeek(Cursor*);
void CursorInsertTuple(Cursor*, Tuple*);
void CursorDeleteTupleById(Cursor*, TupleId);
void CursorUpdateTupleById(Cursor*, Tuple*, TupleId);

#ifdef __cplusplus
}
#endif

#endif  // STORAGE_H
