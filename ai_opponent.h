/*
  Header defining methods for a computer opponent, minimax with alpha-beta pruning and Monte Carlo tree search (not yet optimized)
*/

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
  // Set alpha and beta values at root node
  int orig_a = alpha;
  int orig_b = beta;
  
  (*nodes)++;

  // Check for terminal states (win, draw)
  int winner = getWinner(p);
  if (winner == 1) {
    return depth+1;
  } else if (winner == 2) {
    return -depth-1;
  }
  
  if (~p->occupancy == 0 || depth == 0) {
    return 0;
  }

  // Check for transposition table hit
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


  // Generate ordered moves from current position
  uint64_t moves = 0;
  orderMoves(p, &moves);

  if (maximization) {
    int best_eval = -1000000;

    // Explore all legal moves
    for (int i = 0; i < 8; i++) {
      int move = (moves >> i*8) & 0xff;

      if (generateMove(p, move) == 0) {
	continue;
      }

      // Explore node after generating move
      int eval;
      eval = minimax(p, depth - 1, 0, best_move, alpha, beta, nodes, tt);
      pop(p);

      // If node is the best so far, update best_eval and best_move if searching from root
      if (eval > best_eval) {
	best_eval = eval;
	if (depth == MAX_DEPTH) {
	  *best_move = move;
	}
      }

      // Alpha cutoff
      alpha = max(alpha, eval);
      if (beta <= alpha) {
	break;
      }
    }

    // Store position in the transposition table
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
      int move = (moves >> i*8) & 0xff;

      if (generateMove(p, move) == 0) {
	continue;
      }

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

// Monte Carlo tree search, not yet optimized
int mcts(Playfield* p, int depth, int rollouts, int player, int* best_move, int* nodes, p_table* tt) {
  (*nodes)++;

  // Check for terminal states
  int winner = getWinner(p);
  if (winner) {
    return (depth + 1) * player;
  }

  if (~p->occupancy == 0 || depth == 0) {
    return 0;
  }

  // Check transposition table hit
  uint64_t pos_hash = hashPosition(p);
  if (TT_ENABLE) {
    int result[3];

    if (get_from_hashtable(tt, pos_hash, result)) {
      if (result[1] >= depth && result[2] == EXACT) {
	return result[0];
      }
    }
  }

  // Generate legal moves
  
  int move;
  int best_eval = -999999;
  uint64_t moves = 0;
  orderMoves(p, &moves);

  // Generate n random rollouts
  for (int i = 0; i < rollouts; i++) {
    // Ensure valid move selection
    if (i < 8 && 0) {
      move = (moves >> (i * 8)) & 0xff;
    } else {
      do {
	move = rand() & 7;  // Get a random move
      } while (generateMove(p, move) == 0);  // Ensure the move is valid
    }

    // Get inverse value
    int eval = -mcts(p, depth - 1, rollouts, -player, best_move, nodes, tt);  
    pop(p);

    if (eval > best_eval) {  
      best_eval = eval;
      if (depth == MAX_DEPTH) {
	*best_move = move;
      }
    }
  }

  add_to_hashtable(tt, pos_hash, depth, best_eval, EXACT);

  return best_eval;
}


#endif
