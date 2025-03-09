#include <stdint.h>
#include "typedefs.h"

/*
  boardmethods.h - provide methods on the playfield such as clearing, determining winner and generating legal moves
*/

#ifndef __HEADER_METHODS__
#define __HEADER_METHODS__

// Clear and initialize playfield
void clearPlayfield(Playfield* p) {
  p->turn = P1;
  p->p1_bitboard = 0;
  p->p2_bitboard = 0;
  p->occupancy = 0;
  p->halfTurns = 0;
  p->last_idx = 0;
  p->stack_length = 0;
}

int isWin(uint64_t bitboard, int index) {
  uint64_t offset_bitboard;
  uint32_t top_row;

  // Vertical 4 in a row
  if (((bitboard >> (index)) & (uint64_t)0x1010101) == (uint64_t)0x1010101) {
    return 1;
  }

  // Horizontal 4 in a row
  top_row = (bitboard >> (index & ~7)) & 0xff;
  if (((top_row & 0xf) == 0xf) || ((top_row & 0x1e) == 0x1e) || ((top_row & 0x3c) == 0x3c) || ((top_row & 0x78) == 0x78) || ((top_row & 0xf0) == 0xf0)) {
    return 1;
  }

  // Diagonal 4 in a row
  offset_bitboard = bitboard >> (index >> 3 << 3);
  for (int i=0; i<5; i++) {
    offset_bitboard = bitboard >> i*8;
    if (((offset_bitboard & 0x8040201) == 0x8040201) || ((offset_bitboard & 0x10080402) == 0x10080402) || ((offset_bitboard & 0x20100804) == 0x20100804) || ((offset_bitboard & 0x40201008) == 0x40201008) || ((offset_bitboard & 0x80402010) == 0x80402010) || ((offset_bitboard & 0x1020408) == 0x1020408) || ((offset_bitboard & 0x2040810) == 0x2040810) || ((offset_bitboard & 0x4081020) == 0x4081020) || ((offset_bitboard & 0x8102040) == 0x8102040) || ((offset_bitboard & 0x10204080) == 0x10204080)) {
      return 1;
    }    
  }
  return 0;
}

// Given a playfield, determine the win state, returns 0 in non terminal states
int getWinner(Playfield* p) {
  // Improve performance by only checking for the opposite player's win state

  if (p->turn == P2) {
    if (isWin(p->p1_bitboard, p->last_idx)) {
      return 1;
    }
  } else {
    if (isWin(p->p2_bitboard, p->last_idx)) {
      return 2;
    }
  }
  
  return 0;
}

// Generate a move, return 0 on failure (illegal move provided)
int generateMove(Playfield* p, int column) {
  // Create occupancy bitboard
  p->occupancy = p->p1_bitboard | p->p2_bitboard;

  // Create column mask for determining move legality and making the move
  uint64_t columnMask = ((uint64_t)0x101010101010101 << ((8-HEIGHT) * 8)) << column;

  // Return early if column full
  if (!(columnMask & p->occupancy - columnMask) || column >= WIDTH) {
    return 0;
  }

  // Find shift amount to set bit
  uint64_t move = columnMask & ~p->occupancy;
  int temp = __builtin_clzll(move);

  move = 0x8000000000000000ULL >> temp;

  p->last_idx = __builtin_ctzll(move);

  // Update playfield
  uint64_t* currentPlayerBitboard = (p->turn == 1) ? &p->p1_bitboard : &p->p2_bitboard;
  *currentPlayerBitboard |= move;
  p->occupancy |= move;
  p->turn = 3 - p->turn;
  p->halfTurns++;
  p->stack[p->stack_length] = temp;
  p->stack_length++;
  return 1;
}

void pop(Playfield* p) {
  int index = p->stack[p->stack_length-1];
  p->stack_length--;
  p->turn = 3 - p->turn;
  uint64_t move = 0x8000000000000000ULL >> index;

  p->p1_bitboard &= ~move;
  p->p2_bitboard &= ~move;
  p->halfTurns--;

  p->last_idx = p->stack[p->stack_length-1];
  p->occupancy = p->p1_bitboard | p->p2_bitboard;
}

// Generate an array of legal moves
void generateLegalMoves(Playfield* p, int* m) {
  *m = p->occupancy & (0xff << (8-HEIGHT * 8));
}

// Order column indices, with winning moves at the beginning of the array
void orderMoves(Playfield* p, uint64_t* m) {
  int legal;
  generateLegalMoves(p, &legal);

  uint64_t winningMoves[8];
  uint64_t otherMoves[8];
  int winningLength = 0;
  int otherLength = 0;

  for (int i = 0; i < 8; i++) {
    if (legal & (1<<i)) {
      continue;
    }
    
    if (!generateMove(p, i)) {
      continue;
    }

    if (getWinner(p)) {
      winningMoves[winningLength++] = i;
    } else {
      otherMoves[otherLength++] = i;
    }

    pop(p);
  }
  
  for (int i = 0; i < winningLength+otherLength; i++) {
    if (i < winningLength) {
      *m |= (uint64_t)(winningMoves[i] & 0xff) << i*8;
    } else {
      *m |= (uint64_t)(otherMoves[i] & 0xff) << i*8;
    }
  }
}


#endif
