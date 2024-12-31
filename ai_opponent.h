#include <stdint.h>
#include "typedefs.h"

#ifndef __HEADER_AI__
#define __HEADER_AI__

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

int minimax(Playfield* p, int depth, int maximization, int* best_move, int alpha, int beta, int* nodes, p_table* tt) {
  (*nodes)++;

  if (TT_ENABLE) {
    uint64_t pos_hash = hashPosition(p);
    int result[3];

    if (get_from_hashtable(tt, pos_hash, result)) {
      if (result[2] == depth) {
	return result[1];
      }
    }
  }

  int winner = getWinner(p);
  if (winner == 1) {
    return (MAX_DEPTH - depth);
  } else if (winner == 2) {
    return -depth;
  }

  if (depth == 0) {
    return 0;
  }

  int moves[8] = {-1};
  orderMoves(p, moves);

  if (maximization) {
    int best_eval = -1000000;

    for (int i = 0; i < 8; i++) {
      if (moves[i] == -1) {
	break;
      }

      int move = moves[i];
      
      if (*best_move == -1) {
	*best_move = i;
      }

      Playfield playfield_new;
      memcpy(&playfield_new, p, sizeof(Playfield));
      generateMove(&playfield_new, move);



      int eval;
      if (TT_ENABLE) {
	uint64_t child_hash = hashPosition(&playfield_new);
	int child_result[3];
	if (get_from_hashtable(tt, child_hash, child_result) && child_result[2] == depth) {
	  eval = child_result[1];
	  (*nodes)++;
	} else {
	  eval = minimax(&playfield_new, depth - 1, 0, best_move, alpha, beta, nodes, tt);
	  add_to_hashtable(tt, child_hash, depth, eval, move);
	}
      } else {
	eval = minimax(&playfield_new, depth - 1, 0, best_move, alpha, beta, nodes, tt);
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
      if (moves[i] == -1) {
	break;
      }

      int move = moves[i];

      Playfield playfield_new;
      memcpy(&playfield_new, p, sizeof(Playfield));
      generateMove(&playfield_new, move);


      int eval;

      if (TT_ENABLE) {
	uint64_t child_hash = hashPosition(&playfield_new);
	int child_result[3];
	if (get_from_hashtable(tt, child_hash, child_result) && child_result[2] == depth) {
	  eval = child_result[1];
	  (*nodes)++;
	} else {
	  eval = minimax(&playfield_new, depth - 1, 1, best_move, alpha, beta, nodes, tt);
	  add_to_hashtable(tt, child_hash, depth, eval, move);
	}
      } else {
	eval = minimax(&playfield_new, depth - 1, 1, best_move, alpha, beta, nodes, tt);
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
