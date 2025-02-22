#include <stdint.h>
#include "typedefs.h"

#ifndef __HEADER_AI__
#define __HEADER_AI__

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int minimax(Playfield* p, int depth, int maximization, int* best_move, int alpha, int beta, int* nodes, p_table* tt) {
  (*nodes)++;

  if (~(p->p1_bitboard | p->p2_bitboard)==0) {
    return 0;
  }

  if (depth == 0) {
    return 0;
  }

  
  int winner = getWinner(p);
  if (winner == 1) {
    return depth+1;
  } else if (winner == 2) {
    return -depth-1;
  }

  if (TT_ENABLE) {
    uint64_t pos_hash = hashPosition(p);
    int result[2];

    if (get_from_hashtable(tt, pos_hash, result)) {
      if (result[1] == p->halfTurns) {
	return result[0];
      }
    }
  }


  int moves[8] = {-1};
  orderMoves(p, moves);

  if (maximization) {
    int best_eval = -1000000;

    for (int i = 0; i < 8; i++) {
      if (moves[i] > 7 || moves[i] < 0) {
	break;
      }

      int move = moves[i];

      Playfield childPlayfield;
      memcpy(&childPlayfield, p, sizeof(Playfield));
      generateMove(&childPlayfield, move);

      int eval;
      uint64_t child_hash = hashPosition(&childPlayfield);
      if (TT_ENABLE) {
	eval = minimax(&childPlayfield, depth - 1, 0, best_move, alpha, beta, nodes, tt);
	add_to_hashtable(tt, child_hash, p->halfTurns+1, eval, move);
      } else {
	eval = minimax(&childPlayfield, depth - 1, 0, best_move, alpha, beta, nodes, tt);
      }

      if (eval > best_eval) {
	best_eval = eval;
	if (depth == MAX_DEPTH) {
	  *best_move = move;
	}
      }

      alpha = max(alpha, eval);
      if (beta <= alpha) {
	break;
      }
    }
    return best_eval;
  } else {
    int best_eval = 1000000;

    for (int i = 0; i < 8; i++) {
      if (moves[i] > 7 || moves[i] < 0) {
	break;
      }

      int move = moves[i];

      Playfield childPlayfield;
      memcpy(&childPlayfield, p, sizeof(Playfield));
      generateMove(&childPlayfield, move);

      int eval;
      uint64_t child_hash = hashPosition(&childPlayfield);
      if (TT_ENABLE) {
	eval = minimax(&childPlayfield, depth - 1, 1, best_move, alpha, beta, nodes, tt);
	add_to_hashtable(tt, child_hash, p->halfTurns+1, eval, move);
      } else {
	eval = minimax(&childPlayfield, depth - 1, 1, best_move, alpha, beta, nodes, tt);
      }
      
      if (eval < best_eval) {
	best_eval = eval;

	if (depth == MAX_DEPTH) {
	  *best_move = move;
	}
      }

      beta = min(beta, eval);
      if (beta <= alpha) {
	break;
      }
    }
    return best_eval;
  }
}

#endif
