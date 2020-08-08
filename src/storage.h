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

typedef struct Tuple {
  size_t length;
  size_t num_cols;
  byte null_bitmap[];
} Tuple;

#define GetDataLocs(tuple) \
  (DataLoc*)((byte*)tuple + sizeof(Tuple) + tuple->num_cols * sizeof(byte))

#define GetDataPtr(tuple)                                         \
  (byte*)tuple + sizeof(Tuple) + tuple->num_cols * sizeof(byte) + \
      tuple->num_cols * sizeof(DataLoc)

Tuple* MakeTuple(TableDef*);
size_t GetColIdx(Tuple*, const char*, TableDef*, bool*);
Datum GetCol(Tuple*, const char*, TableDef*);
Tuple* SetCol(Tuple*, const char*, Datum, TableDef*);
Tuple* CopyTuple(Tuple*, TableDef*);

typedef Tuple** Table;

extern Table* Tables;  // sbarr

void InsertTuple(size_t, Tuple*);
// TODO: This appears to be unused.
void UpdateTuple(size_t, Tuple*, size_t);
Tuple* GetTuple(size_t, size_t);
void DeleteHeapTuple(size_t, size_t);

void CreateTable(TableDef*);

void InitSystemTables();

#define PAGE_SIZE 8192

typedef unsigned char* Page;

typedef struct ItemLoc {
  uint16_t offset;
  uint16_t length;
} ItemLoc;

typedef struct PageHeader {
  uint16_t free_lower_offset;
  uint16_t free_upper_offset;
  uint16_t num_locs;
  ItemLoc item_locs[];
} PageHeader;

#define GetPageHeader(page) (PageHeader*)page

void PageInit(Page page);
void PageAddItem(Page page, unsigned char* item, size_t size);

#ifdef __cplusplus
}
#endif

#endif  // STORAGE_H
