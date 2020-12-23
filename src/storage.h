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

Tuple* MakeTuple(const TableDef*);
size_t GetColIdx(const TableDef*, const char*, bool*);
Datum GetCol(Tuple*, const char*, TableDef*);
Tuple* SetCol(Tuple*, const char*, Datum, TableDef*);
Tuple* CopyTuple(Tuple*);

typedef Tuple** Table;

Tuple* SerializeTableDef(const TableDef*);
TableDef* DeserializeTableDef(Tuple*);
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
  uint16_t special_pointer_offset;
  uint16_t num_locs;
  ItemLoc item_locs[];
} PageHeader;

#define GetPageHeader(page) ((PageHeader*)page)
#define GetPageNextLocNum(ptr) GetPageHeader(ptr)->num_locs
#define PageGetNumLocs(ptr) GetPageHeader(ptr)->num_locs
#define PageGetItemLoc(ptr, i) GetPageHeader(ptr)->item_locs[i]
#define PageGetSpecial(ptr) (ptr + GetPageheader(ptr)->special_pointer_offset)

void PageInit(Page, uint16_t);
uint16_t PageGetFreeStart(Page);
bool PageAddItem(Page, unsigned char*, size_t);
unsigned char* PageGetItem(Page, size_t);
uint16_t PageGetItemSize(Page, size_t);
void PageDeleteItem(Page, size_t);

typedef struct Cursor {
  size_t table_index;
  size_t page_index;
  size_t tuple_index;
  const char* rel_name;
} Cursor;

void CursorInit(Cursor*, TableDef*);
Tuple* CursorSeekNext(Cursor*);
// Tuple* CursorPeek(Cursor*);
void CursorInsertTuple(Cursor*, Tuple*);
void CursorDeleteTupleById(Cursor*, TupleId);
void CursorUpdateTupleById(Cursor*, Tuple*, TupleId);

////////////////////////////////////////////////////
// API for Storage Manager
////////////////////////////////////////////////////
Page ReadPage(uint64_t, const char*, uint64_t);
void WritePage(uint64_t, const char*, uint64_t, Page);

typedef struct RelStorageManager {
  int fd;
  uint64_t rel_id;
  const char* rel_name;  // Only really used for file naming.
} RelStorageManager;

RelStorageManager* SMS;  // stb-array.

RelStorageManager* SMOpen(uint64_t, const char*);
void SMCreate(RelStorageManager*);
void SMClose(RelStorageManager*);
int SMRead(RelStorageManager*, uint64_t, byte* buffer);
void SMWrite(RelStorageManager*, uint64_t, byte* buffer);

////////////////////////////////////////////////////
// B-Tree Index Code
////////////////////////////////////////////////////

// Description of an index defined on a table.
typedef struct IndexDef {
  size_t index_id;
  char* index_name;
  size_t* col_idxs;
  size_t table_def_idx;
} IndexDef;

Tuple* SerializeIndexDef(const IndexDef*);
void DeserializeIndexDef(Tuple*, IndexDef*);

void CreateBTreeIndex(const TableDef*, size_t*);

typedef struct BTreeMetaPageHeader {
  uint64_t root_page_id;
} BTreeMetaPageheader;

void BTreeMetaPageInit(Page);

#ifdef __cplusplus
}
#endif

#endif  // STORAGE_H
