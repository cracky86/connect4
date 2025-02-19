#include <stdint.h>
#include "typedefs.h"

#ifndef __HEADER_HASHTABLE__
#define __HEADER_HASHTABLE__

void init_hashtable(p_table* tt) {
  tt->nodeArray = (uint16_t*)calloc(TT_SIZE, sizeof(uint16_t));
  tt->inserted_values = 0;
}

uint64_t hashPosition(Playfield* p) {
  uint64_t hash = (uint64_t)0x404bc32fff2ceed5;
  hash ^= p->p1_bitboard * (uint64_t)0x2f5aba6ac0027231;
  hash ^= p->p2_bitboard * (uint64_t)0xa871de377e953593;
  return hash;
}

void pack(uint16_t* node, int eval, int depth) {
  *node = (signed char)eval << 8;
  *node |= (unsigned char)depth;
}

void unpack(uint16_t node, int* eval, int* depth) {
  *eval = (node >> 8) & 0xff;
  *depth = node & 0xff;
  if (*eval > 127) { *eval -= 256; }
}

void add_to_hashtable(p_table* tt, uint64_t hash, int depth, int eval, int move) {
  if (depth == 0) {
    return;
  }
  uint64_t index = hash % TT_SIZE;
  uint16_t *ptr = &tt->nodeArray[index];

  int e; int d;
  
  unpack(*ptr, &e, &d);
  
  if (*ptr == 0) {
    tt->inserted_values++;
  }
  if (depth > d || *ptr == 0) {
    pack(ptr, eval, depth);
  }
}

int get_from_hashtable(p_table* tt, uint64_t hash, int result[]) {
  uint64_t index = hash % TT_SIZE;
  uint16_t *ptr = &tt->nodeArray[index];
  
  int eval; int depth;
  
  unpack(*ptr, &eval, &depth);
  
  if (*ptr) {
    result[0] = eval;
    result[1] = depth;
    *ptr = 0;
    tt->inserted_values--;
    return 1;
  }
  return 0;
}
#endif
