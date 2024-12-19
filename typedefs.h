#define P1 1
#define P2 2

#include <stdint.h>

#ifndef __HEADER_TYPEDEFS__
#define __HEADER_TYPEDEFS__

struct Playfield_t {
  int turn;
  uint64_t p1_bitboard;
  uint64_t p2_bitboard;
  uint64_t occupancy;
};
typedef struct Playfield_t Playfield;

struct Node_t {
  uint64_t chk;
  int depth;
  int eval;
  int move;
};
typedef struct Node_t Node;

struct p_table_t {
  Node* nodeArray;
  int inserted_values;
};
typedef struct p_table_t p_table;

#endif
