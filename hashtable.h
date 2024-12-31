#include <stdint.h>
#include "typedefs.h"

#ifndef __HEADER_HASHTABLE__
#define __HEADER_HASHTABLE__

void init_hashtable(p_table* tt) {
  tt->nodeArray = (Node*)calloc(TT_SIZE, sizeof(Node));
  tt->inserted_values = 0;
}

uint64_t hashPosition(Playfield* p) {
  uint64_t hash = (uint64_t)0xaa60a660eecd;
  hash ^= p->p1_bitboard * (uint64_t)0x371d27a014c5;
  hash ^= p->p2_bitboard * (uint64_t)0xd2af7c11a396;
  if (p->turn == 1) { return hash; } else { return ~hash; }
  return hash;
}

uint64_t checksum(uint64_t v) {
  uint64_t hash = (uint64_t)0xaa60a660eecd;
  hash ^= v * (uint64_t)0x371d27a014c5;
  return hash;
}

void add_to_hashtable(p_table* tt, uint64_t hash, int depth, int eval, int move) {
  uint64_t index = hash & (TT_SIZE - 1);
  Node *ptr = &tt->nodeArray[index];

  ptr->chk = hash;
  ptr->move = (unsigned char)move;
  ptr->eval = (signed char)eval;
  ptr->depth = (unsigned char)depth;
  tt->inserted_values++;
}

int get_from_hashtable(p_table* tt, uint64_t hash, int result[]) {
  uint64_t index = hash & (TT_SIZE - 1);
  Node *ptr = &tt->nodeArray[index];

  if (ptr->chk == hash) {
    result[0] = (int)ptr->move;
    result[1] = (int)ptr->eval;
    result[2] = (int)ptr->depth;
    return 1;
  }
  return 0;
}
#endif
