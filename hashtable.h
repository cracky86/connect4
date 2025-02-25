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

void pack(uint16_t* node, int flag, int eval, int depth) {
  *node = 0;
  *node |= (flag & 0x03) << 14;
  *node |= ((unsigned char)depth & 0x7F) << 7;
  *node |= ((signed char)eval & 0x7F);
}

void unpack(uint16_t node, int* flag, int* eval, int* depth) {
  *flag = (node >> 14) & 0x03; 
  *depth = (node >> 7) & 0x7f;
  *eval = node & 0x7f;
  if (*eval > 63) { *eval -= 128; }
}

void add_to_hashtable(p_table* tt, uint64_t hash, int depth, int eval, int flag) {
  uint64_t index = hash % TT_SIZE;
  uint16_t *ptr = &tt->nodeArray[index];

  if (*ptr == 0) {
    tt->inserted_values++;
  }
  pack(ptr, flag, eval, depth);
}

int get_from_hashtable(p_table* tt, uint64_t hash, int result[]) {
  uint64_t index = hash % TT_SIZE;
  uint16_t *ptr = &tt->nodeArray[index];
  
  int flag; int eval; int depth;
  
  unpack(*ptr, &flag, &eval, &depth);
  //printf("%i\n",eval);
  
  if (*ptr) {
    result[0] = eval;
    result[1] = depth;
    result[2] = flag;
    return 1;
  }
  return 0;
}
#endif
