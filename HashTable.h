/*
 * This file is part of the BSGS distribution (https://github.com/JeanLucPons/Kangaroo).
 * Copyright (c) 2020 Jean Luc PONS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HASHTABLEH
#define HASHTABLEH

#include <string>
#include <vector>
#include "SECPK1/Point.h"
#ifdef WIN64
#include <Windows.h>
#endif

#define HASH_SIZE_BIT 18
#define HASH_SIZE (1<<HASH_SIZE_BIT)
#define HASH_MASK (HASH_SIZE-1)

#define ADD_OK        0
#define ADD_DUPLICATE 1
#define ADD_COLLISION 2

union int128_s {

  uint8_t  i8[16];
  uint16_t i16[8];
  uint32_t i32[4];
  uint64_t i64[2];

};

typedef union int128_s int128_t;

union int256_s {

  uint8_t  i8[32];
  uint16_t i16[16];
  uint32_t i32[8];
  uint64_t i64[4];

};

typedef union int256_s int256_t;

#define safe_free(x) if(x) {free(x);x=NULL;}

// Distance field width. The sign and kangaroo-type flags live in the top two
// bits, exactly as in the original 128bit layout, so the magnitude gets
// b253..b0. That is 254 bits for a search that needs at most ~160, i.e. plenty
// of headroom -- moving the flags out to a separate byte would only pad ENTRY
// from 48 to 56 bytes and grow every DP table by 17% for nothing.
#define DIST_MAG_BITS     254
#define MAX_INTERVAL_BITS 253
#define DIST_SIGN_MASK    0x8000000000000000ULL   // b255, in i64[3]
#define DIST_TYPE_MASK    0x4000000000000000ULL   // b254, in i64[3]
#define DIST_MAG_MASK     0x3FFFFFFFFFFFFFFFULL   // b253..b192, in i64[3]

// We store only 128 (+18) bit a the x value which give a probabilty a wrong collision after 2^73 entries

typedef struct {

  int128_t  x;    // Poisition of kangaroo (128bit LSB)
  int256_t  d;    // Travelled distance (b255=sign b254=kangaroo type, b253..b0 distance

} ENTRY;

// On-disk and on-wire size of one ENTRY. Hard-coded rather than sizeof() at
// each use site so a layout change cannot silently reinterpret old files.
#define ENTRY_SIZE 48
static_assert(sizeof(ENTRY) == ENTRY_SIZE,"ENTRY must stay packed at 48 bytes");

typedef struct {

  uint32_t   nbItem;
  uint32_t   maxItem;
  ENTRY    **items;

} HASH_ENTRY;

class HashTable {

public:

  HashTable();
  int Add(Int *x,Int *d,uint32_t type);
  int Add(uint64_t h,int128_t *x,int256_t *d);
  int Add(uint64_t h,ENTRY *e);
  uint64_t GetNbItem();
  void Reset();
  std::string GetSizeInfo();
  void PrintInfo();
  void SaveTable(FILE *f);
  void SaveTable(FILE* f,uint32_t from,uint32_t to,bool printPoint=true);
  void LoadTable(FILE *f);
  void LoadTable(FILE* f,uint32_t from,uint32_t to);
  void ReAllocate(uint64_t h,uint32_t add);
  void SeekNbItem(FILE* f,bool restorePos = false);
  void SeekNbItem(FILE* f,uint32_t from,uint32_t to);

  HASH_ENTRY    E[HASH_SIZE];
  // Collision info
  Int      kDist;
  uint32_t kType;

  static void Convert(Int *x,Int *d,uint32_t type,uint64_t *h,int128_t *X,int256_t *D);
  static int MergeH(uint32_t h,FILE* f1,FILE* f2,FILE* fd,uint32_t *nbDP,uint32_t* duplicate,
                    Int* d1,uint32_t* k1,Int* d2,uint32_t* k2);
  static void CalcDistAndType(int256_t d,Int* kDist,uint32_t* kType);

private:

  ENTRY *CreateEntry(int128_t *x,int256_t *d);
  static int compare(int128_t *i1,int128_t *i2);
  static bool sameDist(int256_t *a,int256_t *b);
  std::string GetStr(int128_t *i);

};

#endif // HASHTABLEH
