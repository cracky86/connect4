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
      index = __builtin_ia32_tzcnt_u64(p1_bitboard);

      // Horizontal 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0x1f1f1f1f1f1f1f1f)) {
	mask = (((uint64_t)0xf) << index);
	if ((p->p1_bitboard & mask) == mask) {
	  return P1;
	}
      }

      // Vertical 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0xffffffffff)) {
	mask = (((uint64_t)0x1010101) << index);
	if ((p->p1_bitboard & mask) == mask) {
	  return P1;
	}
      }

      // Diagonal top-left to bottom-right
      if (((uint64_t)1 << index) & ((uint64_t)0x1f1f1f1f1f)) {
	mask = (((uint64_t)0x8040201) << index);
	if ((p->p1_bitboard & mask) == mask) {
	  return P1;
	}
      }

      // Diagonal bottom-left to top-right
      if (((uint64_t)1 << index) & ((uint64_t)0xf8f8f8f8f8)) {
	mask = (((uint64_t)0x1020408) << (index - 3));
	if ((p->p1_bitboard & mask) == mask) {
	  return P1;
	}
      }
    
      p1_bitboard &= p1_bitboard - 1;
    }
  } else {
    while (p2_bitboard) {
      index = __builtin_ia32_tzcnt_u64(p2_bitboard);

      // Horizontal 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0x1f1f1f1f1f1f1f1f)) {
	mask = (((uint64_t)0xf) << index);
	if ((p->p2_bitboard & mask) == mask) {
	  return P2;
	}
      }

      // Vertical 4 in a row
      if (((uint64_t)1 << index) & ((uint64_t)0xffffffffff)) {
	mask = (((uint64_t)0x1010101) << index);
	if ((p->p2_bitboard & mask) == mask) {
	  return P2;
	}
      }

      // Diagonal top-left to bottom-right
      if (((uint64_t)1 << index) & ((uint64_t)0x1f1f1f1f1f)) {
	mask = (((uint64_t)0x8040201) << index);
	if ((p->p2_bitboard & mask) == mask) {
	  return P2;
	}
      }

      // Diagonal bottom-left to top-right
      if (((uint64_t)1 << index) & ((uint64_t)0xf8f8f8f8f8)) {
	mask = (((uint64_t)0x1020408) << (index - 3));
	if ((p->p2_bitboard & mask) == mask) {
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
  int temp = __builtin_ia32_lzcnt_u64(move);

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
  for (int i = 0; i < 8; i++) {
    columnMask = ((uint64_t)0x101010101010101) << i;

    // Check if the column has space for a new piece
    m[i] = ((columnMask & p->occupancy) ^ columnMask) != 0; // If there's an empty space
  }
}

void swap (int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void orderMoves(Playfield* p, int m[]) {
  int legal[8];
  generateLegalMoves(p, legal);

  int winningMoves[8];
  int otherMoves[8];
  int winningLength = 0;
  int otherLength = 0;

  for (int i = 0; i < 8; i++) {
    if (legal[i] == 0) {
      continue;
    }

    Playfield childPlayfield;
    memcpy(&childPlayfield, p, sizeof(Playfield));
    
    if (!generateMove(&childPlayfield, i)) {
      continue;
    }

    if (getWinner(&childPlayfield)) {
      winningMoves[winningLength++] = i;
    } else {
      otherMoves[otherLength++] = i;
    }
  }
  
  // Copy winning moves first
  for (int i = 0; i < winningLength; i++) {
    m[i] = winningMoves[i];
  }

  // Copy other moves
  for (int i = winningLength; i < 8; i++) {
    m[i] = otherMoves[i];
  }
}


#endif
