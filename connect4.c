#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TT_SIZE 13466917 
#define ALPHA -999999
#define BETA 999999

int MAX_DEPTH = 15;
int TT_ENABLE = 1;

#include "typedefs.h"
#include "boardmethods.h"
#include "hashtable.h"
#include "ai_opponent.h"

void main(int argc, char **argv) {

  int debug = 0;
  int ai_p1 = 0;
  int ai_p2 = 0;

  p_table transpositionTable_p1;
  init_hashtable(&transpositionTable_p1);
  p_table transpositionTable_p2;
  init_hashtable(&transpositionTable_p2);
  
  for (int i=0;i<argc;++i) {
    if (strcmp(argv[i],"--debug") == 0) {
      debug = 1;
    }
    if (strcmp(argv[i],"--ai-p1") == 0) {
      ai_p1 = 1;
    }
    if (strcmp(argv[i],"--ai-p2") == 0) {
      ai_p2 = 1;
    }
    if (strcmp(argv[i],"--tt-disable") == 0) {
      TT_ENABLE = 0;
    }
    if (strcmp(argv[i],"--depth") == 0) {
      MAX_DEPTH = atoi(argv[i+1]);
    }
  }
  
  Playfield playfield;
  struct Playfield_t *ptr = &playfield;
  
  clearPlayfield(ptr);

  int winner = 0;
  int column;
  int validMove;

  char columnOut[256];
  uint64_t check_row;
  printf("\e[1;1H\e[2J");
  printf("CONNECT 4\n\n");

  if (debug) {
    printf("%s%i%s","Hashtable is ", (sizeof(transpositionTable_p1.nodeArray) * TT_SIZE) / 1024, " kB long\n");
  }
  
  while (winner == 0) {

    if (debug) {
      printf("%s%llu%s","P1 bitboard: ",playfield.p1_bitboard,"\n");
      printf("%s%llu%s","P2 bitboard: ",playfield.p2_bitboard,"\n");
    }
    
    int outIndex = 0;
    memset(columnOut, 0, 256);

    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        uint64_t index = 0x1ULL << (row * 8 + col);
        char c;
        if (index & playfield.p1_bitboard) {
	  c = 'X';
        } else if (index & playfield.p2_bitboard) {
	  c = 'O';
        } else {
	  c = '-';
        }

        columnOut[outIndex++] = c;
      }
      columnOut[outIndex++] = '\n';
    }

    columnOut[outIndex] = '\0'; // null-terminate the string



    printf(columnOut);
    printf("12345678\n");
    printf("\n\n");

    winner = getWinner(ptr);

    if (winner != 0) {
      break;
    }
    
    if (playfield.turn == 1) {
      printf("Player 1 turn\n");
    } else {
      printf("Player 2 turn\n");
    }

    if (!ai_p1 && (playfield.turn == 1) || !ai_p2 && (playfield.turn == 2)) {
      validMove = 0;
      while (!(validMove)) {
	printf("Enter column index: ");
	scanf("%i", &column);
	column--;
      
	if (column < 0 || column > 7) {
	  printf("Invalid column index\n");
	  continue;
	}

	validMove = generateMove(ptr, column);
      
	if (!(validMove)) {
	  printf("Column full\n");
	}      
      }
    } else {
      int nodes = 0;
      int move = rand()%7;

      int eval = 0;
      int legal_moves[8];
      int *legal_p = legal_moves;
      generateLegalMoves(ptr, legal_p);
      
      while (!legal_moves[move]) {
	move = rand()%7;
      }

      clock_t start = clock(), diff;
      if (playfield.turn == 1) {
	eval = minimax(ptr, MAX_DEPTH, 1, &move, ALPHA, BETA, &nodes, &transpositionTable_p1);
      } else {
	eval = minimax(ptr, MAX_DEPTH, 0, &move, ALPHA, BETA, &nodes, &transpositionTable_p2);
      }
      
      generateMove(ptr, move);
      diff = clock() - start;
      float msec = diff * 1000 / CLOCKS_PER_SEC;

      float p_table_usage = transpositionTable_p2.inserted_values/TT_SIZE;
      
      printf("%s%i%s", "Searched ", nodes, " nodes\n");
      if (TT_ENABLE) { printf("%s%0.0f%s", "p_table occupancy: ",p_table_usage,"%\n"); }
      printf("%0.0f%s", nodes / (msec/1000)/1000000, " mN/s\n");

      if (eval > 0) {
        printf("%s%i%s","\nP1 wins in ", eval-13, " move(s)\n");
      } else if (eval < 0) {
        printf("%s%i%s","\nP2 wins in ", eval+13, " move(s)\n");
      }

    }
  }

  if (winner == 1) {
    printf("Player 1 won!\n");
  } else {
    printf("Player 2 won!\n");
  }
  
  
}
