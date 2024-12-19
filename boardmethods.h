#include <stdint.h>
#include "typedefs.h"

#ifndef __HEADER_METHODS__
#define __HEADER_METHODS__

void clearPlayfield(Playfield* p) {
  p->turn = P1;
  p->p1_bitboard = 0;
  p->p2_bitboard = 0;
  p->occupancy = 0;
}

int getWinner(Playfield* p) {
  uint64_t p1_bitboard = p->p1_bitboard;
  uint64_t p2_bitboard = p->p2_bitboard;

  int index;
  uint64_t mask;

  if (p->turn == P2) {
    while (p1_bitboard) {
      index = __builtin_ctzll(p1_bitboard);

      // Horizontal 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0x1f1f1f1f1f1f1f1f)) {
	mask = (((uint64_t)0xf) << index);
	if ((p->p1_bitboard & mask) - mask == 0) {
	  return P1;
	}
      }

      // Vertical 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0xffffffffff) && index <= 36) {
	mask = (((uint64_t)0x1010101) << index);
	if ((p->p1_bitboard & mask) - mask == 0) {
	  return P1;
	}
      }

      // Diagonal top-left to bottom-right
      if (((uint64_t)1 << index) & ((uint64_t)0x1f1f1f1f1f)) {
	mask = (((uint64_t)0x8040201) << index);
	if ((p->p1_bitboard & mask) - mask == 0) {
	  return P1;
	}
      }

      // Diagonal bottom-left to top-right
      if (((uint64_t)1 << index) & ((uint64_t)0x10101018183878f8) && index <= 40 && index >= 3) {
	mask = (((uint64_t)0x1020408) << (index - 3));
	if ((p->p1_bitboard & mask) - mask == 0) {
	  return P1;
	}
      }
    
      p1_bitboard &= p1_bitboard - 1;
    }
  } else {
    while (p2_bitboard) {
      index = __builtin_ctzll(p2_bitboard);

      // Horizontal 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0x1f1f1f1f1f1f1f1f)) {
	mask = (((uint64_t)0xf) << index);
	if ((p->p2_bitboard & mask) - mask == 0) {
	  return P2;
	}
      }

      // Vertical 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0xffffffffff) && index <= 36) {
	mask = (((uint64_t)0x1010101) << index);
	if ((p->p2_bitboard & mask) - mask == 0) {
	  return P2;
	}
      }

      // Diagonal top-left to bottom-right
      if (((uint64_t)1 << index) & ((uint64_t)0xf87838181c1e1f1f) && index <= 36) {
	mask = (((uint64_t)0x8040201) << index);
	if ((p->p2_bitboard & mask) - mask == 0) {
	  return P2;
	}
      }

      // Diagonal bottom-left to top-right
      if (((uint64_t)1 << index) & ((uint64_t)0x1e1c1818183878ff) && index <= 36 && index >= 3) {
	mask = (((uint64_t)0x1020408) << (index - 3));
	if ((p->p2_bitboard & mask) - mask == 0) {
	  return P2;
	}
      }
    
      p2_bitboard &= p2_bitboard - 1;
    }
  }
  return 0;
}


int generateMove(Playfield* p, int column) {
  p->occupancy = p->p1_bitboard | p->p2_bitboard;
  uint64_t columnMask = ((uint64_t)0x101010101010101) << column;

  // Return early if column full
  if (!(columnMask & p->occupancy - columnMask)) {
    return 0;
  }

  // Find shift amount to set bit
  uint64_t move = columnMask & ~p->occupancy;
  int temp = __builtin_clzll(move);
  
  move = 0x8000000000000000ULL >> temp;

  // Update correct players bitboard
  uint64_t* currentPlayerBitboard = (p->turn == 1) ? &p->p1_bitboard : &p->p2_bitboard;
  *currentPlayerBitboard |= move;
  p->turn = 3 - p->turn;
  return 1;
}

void generateLegalMoves(Playfield* p, int m[]) {
  p->occupancy = p->p1_bitboard | p->p2_bitboard;
  uint64_t columnMask;
  for (int i = 0; i <= 7; i++) {
    columnMask = ((uint64_t)0x101010101010101) << i;
    if (!(columnMask & p->occupancy - columnMask)) {
      m[i] = 0;
    } else {
      m[i] = 1;
    }
  }
}

#endif
