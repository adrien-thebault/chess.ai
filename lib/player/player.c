/**
*
*  ChessPlayer : actual artificial intelligence
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../game/game.h"
#include "../output/output.h"
#include "./player.h"

/**
*
*  Tries to guess the best move to play
*
*/

void ChessPlayer_Play(game *g, unsigned char player, unsigned char move[2][2], unsigned char algorithm) {

  if(algorithm == ALPHABETA || algorithm == MTDF || algorithm == PVS) ChessPlayer_ID(g, move, player, algorithm);

}

/**
*
*  Iterative Deepening
*
*/

void ChessPlayer_ID(game *g, unsigned char move[2][2], unsigned char p, unsigned char algorithm) {

  char* algo;
  unsigned char depth = 1;
  unsigned long long int visited = 0;
  float valuation = ChessGame_Valuation(g, p);

  if(algorithm == ALPHABETA) algo = "AlphaBeta";
  else if(algorithm == MTDF) algo = "MTD(f)";
  else if(algorithm == PVS) algo = "PVSplit";

  printf("\n");
  printf(INFO("Iterative deepening %s with MAX_DEPTH = %d"), algo, MAX_DEPTH);
  printf(INFO("Actual board has a valuation of %.2f"), valuation);

  while(depth <= MAX_DEPTH && g->round_end_time > time(NULL)) {

    printf(INFO("|\tTime left : %hhus"), (unsigned char)(g->round_end_time-time(NULL)));
    printf(INFO("|\tDepth : %hhu"), depth);
    printf(INFO("|\t|\t%s with limit = %d"), algo, depth);

    if(algorithm == ALPHABETA) {

      /** Reset valuation */
      g->valuation = -1;
      ChessGame_Valuation(g, p);

      /** AlphaBeta */
      valuation = ChessPlayer_AlphaBeta(depth, 0, g, p, move, &visited, -INF, INF, MAX);

    } else if(algorithm == PVS) {

      /** Reset valuation */
      g->valuation = -1;
      ChessGame_Valuation(g, p);

      /** AlphaBeta */
      valuation = ChessPlayer_PVSplit(depth, 0, g, p, move, &visited, -INF, INF, MAX);

    } else valuation = ChessPlayer_MTDF(g, p, valuation, depth, move, &visited);

    printf(INFO("|\t|\tWent through %llu possibilities"), visited);
    printf(INFO("|\tBest move has a valuation of %.2f"), valuation);

    printf(INFO("|\tMove : %c%hhu -> %c%hhu"), move[0][1]+97, move[0][0]+1, move[1][1]+97, move[1][0]+1);
    printf(INFO("|"));
    depth++;

  }

}

/**
*
*  MTD-f
*  /!\ Not really functionnal as it needs properly working transposition tables
*
*/

float ChessPlayer_MTDF(game* g, unsigned char player, float f, unsigned char depth, unsigned char move[2][2], unsigned long long int *visited) {

  float lower = -INF, upper = +INF, beta;
  while(lower < upper) {

    beta = (f > (lower + 1)) ? f : (lower + 1);
    f = ChessPlayer_PVSplit(depth, 0, g, player, move, visited, beta - 1, beta, MAX);

    if(f < beta) upper = f;
    else lower = f;

  }

  return f;

}

/**
*
*  AlphaBeta
*
*/

float ChessPlayer_AlphaBeta(unsigned char limit, unsigned char depth, game *g, unsigned char player, unsigned char move[2][2], unsigned long long int *visited, float alpha, float beta, unsigned char min_or_max) {

  if(depth < limit) {

    float valuation, res; signed char piece; bool promotion;
    unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], possible_moves_length = 0;

    ChessPlayer_PossibleMoves(g, possible_moves, &possible_moves_length);
    *visited += possible_moves_length;

    if(possible_moves_length == 0) return ChessGame_Valuation(g, player);
    else if(min_or_max == MIN) {

      valuation = INF;
      for(unsigned char i = 0; i < possible_moves_length; i++) {

        promotion = false;
        piece = ChessGame_Move(g, possible_moves[i][0], possible_moves[i][1], -1, &promotion);

        if((piece & MASK_PIECE) == KING) res = (MAX_DEPTH - depth) * ChessGame_Valuation(g, player);
        else res = ChessPlayer_AlphaBeta(limit, depth+1, g, player, move, visited, alpha, beta, MAX);

        ChessGame_Move(g, possible_moves[i][1], possible_moves[i][0], piece, &promotion);

        if(res < valuation) {

          valuation = res;
          if(depth == 0) memcpy(move, possible_moves[i], 4*sizeof(unsigned char));

        }

        if(alpha >= valuation) break;
        if(valuation < beta) beta = valuation;

      }

    } else {

      valuation = -INF;
      for(unsigned char i = 0; i < possible_moves_length; i++) {

        promotion = false;
        piece = ChessGame_Move(g, possible_moves[i][0], possible_moves[i][1], -1, &promotion);

        if((piece & MASK_PIECE) == KING) res = (MAX_DEPTH - depth) * ChessGame_Valuation(g, player);
        else res = ChessPlayer_AlphaBeta(limit, depth+1, g, player, move, visited, alpha, beta, MIN);

        ChessGame_Move(g, possible_moves[i][1], possible_moves[i][0], piece, &promotion);

        if(res > valuation) {

          valuation = res;
          if(depth == 0) memcpy(move, possible_moves[i], 4*sizeof(unsigned char));

        }

        if(beta <= valuation) break;
        if(valuation > alpha) alpha = valuation;

      }

    }

    return valuation;

  } else return ChessGame_Valuation(g, player);

}

/**
*
*  PVS : parallelized alpha-beta
*
*/

float ChessPlayer_PVSplit(unsigned char limit, unsigned char depth, game *g, unsigned char player, unsigned char move[2][2], unsigned long long int *visited, float alpha, float beta, unsigned char min_or_max) {

  if(depth < limit) {

    float valuation, res; signed char piece; bool promotion;
    unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], possible_moves_length = 0;

    ChessPlayer_PossibleMoves(g, possible_moves, &possible_moves_length);
    *visited += possible_moves_length;

    if(possible_moves_length == 0) return ChessGame_Valuation(g, player);
    else {

      unsigned char best_move[2][2], moves[POSSIBLE_MOVES_SIZE][2][2], moves_length, max, nb, nb_forks;
      unsigned long long int fork_visited;
      int pipes[NB_CORES][2], status;
      float fork_alpha, fork_beta;
      signed char child = -1;
      pid_t pid[NB_CORES];
      char message[256];

      promotion = false;
      piece = ChessGame_Move(g, possible_moves[0][0], possible_moves[0][1], -1, &promotion);
      valuation = ChessPlayer_PVSplit(limit, depth+1, g, player, move, visited, alpha, beta, (min_or_max == MIN) ? MAX : MIN);
      ChessGame_Move(g, possible_moves[0][1], possible_moves[0][0], piece, &promotion);

      if(depth == 0) memcpy(move, possible_moves[0], 4*sizeof(unsigned char));

      if(min_or_max == MIN) {

        if(valuation <= alpha) return valuation;
        if(valuation < beta) beta = valuation;

      } else {

        if(valuation >= beta) return valuation;
        if(valuation > alpha) alpha = valuation;

      }

      memcpy(moves, possible_moves, (POSSIBLE_MOVES_SIZE)*4*sizeof(unsigned char));
      moves_length = possible_moves_length - 1; possible_moves_length = 0;

      for(int i = 0; i < moves_length; i++) memcpy(moves[i], moves[i+1], 4*sizeof(unsigned char));

      if((nb = moves_length/NB_CORES) < 1) {
        nb = 1;
        nb_forks = moves_length;
      } else nb_forks = NB_CORES;

      for(unsigned char i = 0; i < nb_forks; i++) {

        pipe(pipes[i]);

        if((pid[i] = fork()) == 0) {

          close(pipes[i][0]);
          child = i;

          if(i == nb_forks - 1 || (i+1)*nb > moves_length) max = moves_length;
          else max = (i+1)*nb;

          for(unsigned char j = i*nb; j < max; j++) {

            memcpy(possible_moves[possible_moves_length], moves[j], POSSIBLE_MOVES_SIZE*4*sizeof(unsigned char));
            possible_moves_length++;

          }

          *visited = 0;
          break;

        } else {

          close(pipes[i][1]);
          child = -1;

        }

      }

      if(child > -1) {

        if(min_or_max == MIN) {

          valuation = INF;
          for(unsigned char i = 0; i < possible_moves_length; i++) {

            promotion = false;
            piece = ChessGame_Move(g, possible_moves[i][0], possible_moves[i][1], -1, &promotion);

            if((piece & MASK_PIECE) == KING) res = (MAX_DEPTH - depth) * ChessGame_Valuation(g, player);
            else res = ChessPlayer_AlphaBeta(limit, depth+1, g, player, move, visited, alpha, beta, MAX);

            ChessGame_Move(g, possible_moves[i][1], possible_moves[i][0], piece, &promotion);

            if(res < valuation) {

              valuation = res;
              if(depth == 0) memcpy(move, possible_moves[i], 4*sizeof(unsigned char));

            }

            if(alpha >= valuation) break;
            if(valuation < beta) beta = valuation;

          }

        } else {

          valuation = -INF;
          for(unsigned char i = 0; i < possible_moves_length; i++) {

            promotion = false;
            piece = ChessGame_Move(g, possible_moves[i][0], possible_moves[i][1], -1, &promotion);

            if((piece & MASK_PIECE) == KING) res = (MAX_DEPTH - depth) * ChessGame_Valuation(g, player);
            else res = ChessPlayer_AlphaBeta(limit, depth+1, g, player, move, visited, alpha, beta, MIN);

            ChessGame_Move(g, possible_moves[i][1], possible_moves[i][0], piece, &promotion);

            if(res > valuation) {

              valuation = res;
              if(depth == 0) memcpy(move, possible_moves[i], 4*sizeof(unsigned char));

            }

            if(beta <= valuation) break;
            if(valuation > alpha) alpha = valuation;

          }

        }

        sprintf(message, "%f | %llu | %f | %f | [%hhu, %hhu] -> [%hhu, %hhu]\n", valuation, *visited, alpha, beta, move[0][0], move[0][1], move[1][0], move[1][1]);
        write(pipes[child][1], message, strlen(message)+1);

        close(pipes[child][1]);
        exit(EXIT_SUCCESS);

      } else {

        wait(&status);

        for(unsigned char i = 0; i < NB_CORES; i++) {

          read(pipes[i][0], message, sizeof(message));
          sscanf(message, "%f | %llu | %f | %f | [%hhu, %hhu] -> [%hhu, %hhu]\n", &res, &fork_visited, &fork_alpha, &fork_beta, &best_move[0][0], &best_move[0][1], &best_move[1][0], &best_move[1][1]);

          if(fork_alpha > alpha) alpha = fork_alpha;
          if(fork_beta < beta) beta = fork_beta;

          *visited += fork_visited;

          if((min_or_max == MAX && res > valuation) || (min_or_max == MIN && res < valuation)) {

            valuation = res;
            memcpy(move, best_move, 4*sizeof(unsigned char));

          }

          close(pipes[i][0]);

        }

        return valuation;

      }

    }

  } else return ChessGame_Valuation(g, player);

}

/**
*
*
*  Computes every possible move
*
*/

void ChessPlayer_PossibleMoves(game *g, unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char* possible_moves_length) {

  unsigned char row[2];

  for(unsigned char i = 0; i<64; i++) {

    row[0] = i/8; row[1] = i%8;
    if(g->chessboard[row[0]][row[1]] != -1 && (g->chessboard[row[0]][row[1]] & MASK_PLAYER) == g->round_player) ChessGame_PossibleMoves(g, row, possible_moves, possible_moves_length);

  }

}
