#include <stdint.h>
#include "typedefs.h"

#ifndef __HEADER_AI__
#define __HEADER_AI__

#define UPPERBOUND 1
#define LOWERBOUND 2
#define EXACT 3

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int minimax(Playfield* p, int depth, int maximization, int* best_move, int alpha, int beta, int* nodes, p_table* tt) {
  int orig_a = alpha;
  int orig_b = beta;
  
  (*nodes)++;

  int winner = getWinner(p);
  if (winner == 1) {
    return depth+1;
  } else if (winner == 2) {
    return -depth-1;
  }
  
  if (~p->occupancy == 0 || depth == 0) {
    return 0;
  }

  uint64_t pos_hash = hashPosition(p);
  if (TT_ENABLE) {
    int result[3];

    if (get_from_hashtable(tt, pos_hash, result)) {
      if (result[1] >= depth) {
	if (result[2] == EXACT) {
	  return result[0];
	} else if (result[2] == LOWERBOUND) {
	  alpha = max(alpha, result[0]);
	} else if (result[2] == UPPERBOUND) {
	  beta = min(beta, result[0]);
	}

	if (alpha >= beta) {
	  return result[0];
	}
      }
    }
  }


  uint64_t moves = 0;
  int legal_moves;

  generateLegalMoves(p, &legal_moves);
  orderMoves(p, &moves);

  if (maximization) {
    int best_eval = -1000000;

    for (int i = 0; i < 8; i++) {
      if (1ULL << ((moves >> i*8) & 0xff) & legal_moves) {
	continue;
      }

      int move = (moves >> i*8) & 0xff;

      generateMove(p, move);

      int eval;
      eval = minimax(p, depth - 1, 0, best_move, alpha, beta, nodes, tt);

      pop(p);

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
    if (TT_ENABLE) {
      int flag;
      if (best_eval <= orig_a) {
	flag = UPPERBOUND;
      } else if (best_eval >= beta) {
	flag = LOWERBOUND;
      } else {
	flag = EXACT;
      }
      add_to_hashtable(tt, pos_hash, depth, best_eval, flag);
    }

    return best_eval;
  } else {
    int best_eval = 1000000;

    for (int i = 0; i < 8; i++) {
      if (1ULL << ((moves >> i*8) & 0xff) & legal_moves) {
	continue;
      }

      int move = (moves >> i*8) & 0xff;

      generateMove(p, move);

      int eval;      
      eval = minimax(p, depth - 1, 1, best_move, alpha, beta, nodes, tt);

      pop(p);
      
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
    if (TT_ENABLE) {
      int flag;
      if (best_eval <= orig_a) {
	flag = UPPERBOUND;
      } else if (best_eval >= beta) {
	flag = LOWERBOUND;
      } else {
	flag = EXACT;
      }
      add_to_hashtable(tt, pos_hash, depth, best_eval, flag);
    }
    
    return best_eval;
  }
}

#endif
