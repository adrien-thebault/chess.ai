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

#include "../game/game.h"
#include "../output/output.h"
#include "./player.h"

/** Global variables */

static node *current;
static unsigned char player;
static unsigned int nb_children;
static unsigned char best_move[2][2];
static unsigned char time_left;

/**
*
*  Tries to guess the best move to play
*
*/

void ChessPlayer_Play(game *g, unsigned char p, unsigned char move[2][2]) {

  /** Initialisation */

  player = p;
  time_left = g->round_end_time - time(NULL);

  current = malloc(sizeof(node));
  current->game = g;
  current->nb_children = 0;
  current->valuation = 0;

  /** Let's play! */

  ChessPlayer_IDAlphaBeta();
  memcpy(move, best_move, 4*sizeof(unsigned char));

}

/**
*
*  Iterative Deepening Alpha/Beta
*  @TODO sort children
*
*/

void ChessPlayer_IDAlphaBeta() {

  printf(INFO("Iterative deepening AlphaBeta with MAX_DEPTH = %d"), MAX_DEPTH);
  unsigned char depth = 1; nb_children = 0;

  while(depth <= MAX_DEPTH) {

    printf(INFO("|\tTime left : %hhus"), time_left);
    printf(INFO("|\tDepth : %hhu"), depth);

    current->valuation = ChessPlayer_AlphaBeta(depth, 1, current, -INF, INF, MAX);
    printf(INFO("|\t|\tWent through %u possibilities"), nb_children);
    printf(INFO("|\tBest move has a valuation of %.2f"), current->valuation);

    for(unsigned int i = 0; i < current->nb_children; i++) {
      if(current->children[i]->node->valuation == current->valuation) {
        best_move[0][0] = current->children[i]->from[0];
        best_move[0][1] = current->children[i]->from[1];
        best_move[1][0] = current->children[i]->to[0];
        best_move[1][1] = current->children[i]->to[1];
        break;
      }
    }

    printf(INFO("|\tMove : %c%hhu -> %c%hhu"), best_move[0][1]+97, best_move[0][0]+1, best_move[1][1]+97, best_move[1][0]+1);
    printf(INFO("|"));
    depth++;

  }

}

/**
*
*  AlphaBeta
*
*/

int ChessPlayer_AlphaBeta(unsigned char limit, unsigned char depth, node *n, int alpha, int beta, unsigned char min_or_max) {

  float valuation, res;
  if(depth == 1) printf(INFO("|\t|\tAlphaBeta with limit = %d"), limit);

  if(depth <= limit) {

    if(n->nb_children == 0) {

      ChessPlayer_Children(n);
      nb_children += n->nb_children;

    }

    if(min_or_max == MIN) {

      valuation = INF;
      for(unsigned char i = 0; i < n->nb_children; i++) {

        res = ChessPlayer_AlphaBeta(limit, depth+1, n->children[i]->node, alpha, beta, MAX);
        valuation = (valuation < res) ? valuation : res;

        if(alpha >= valuation) break;
        beta = (beta < valuation) ? beta : valuation;

      }

    } else {

      valuation = -INF;
      for(unsigned char i = 0; i < n->nb_children; i++) {

        res = ChessPlayer_AlphaBeta(limit, depth+1, n->children[i]->node, alpha, beta, MIN);
        valuation = (valuation > res) ? valuation : res;

        if(beta <= valuation) break;
        alpha = (alpha > valuation) ? alpha : valuation;

      }

    }

  } else valuation = ChessGame_Valuation(n->game, player);

  n->valuation = valuation;
  return n->valuation;

}

/**
*
*  Computes every children of a given node
*  @TODO parallelisation
*
*/

void ChessPlayer_Children(node *parent) {
  if(parent->nb_children == 0) {

    unsigned char row[2], possible_moves[POSSIBLE_MOVES_SIZE][2], possible_moves_length = 0;
    game *g; node *n; struct child *c;

    for(unsigned char i = 0; i<64; i++) {

      row[0] = i/8; row[1] = i%8;
      if(parent->game->chessboard[row[0]][row[1]] != -1 && (parent->game->chessboard[row[0]][row[1]] & MASK_PLAYER) == parent->game->round_player) {

        possible_moves_length = 0;

        ChessGame_PossibleMoves(parent->game, row, possible_moves, &possible_moves_length);

        for(unsigned char j = 0; j<possible_moves_length; j++) {

          g = malloc(sizeof(game));
          memcpy(g, parent->game, sizeof(game));

          n = malloc(sizeof(node));
          n->game = g;
          n->nb_children = 0;
          n->valuation = (float) 0;

          c = malloc(sizeof(struct child));
          c->from[0] = row[0];
          c->from[1] = row[1];
          c->to[0] = possible_moves[j][0];
          c->to[1] = possible_moves[j][1];
          c->node = n;

          parent->children[parent->nb_children] = c;
          parent->nb_children++;

        }

      }

    }

  }
}
