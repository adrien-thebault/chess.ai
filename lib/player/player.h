/**
*
*  ChessPlayer functions headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef PLAYER_H
#define PLAYER_H

/** Constants */

#define MIN 0
#define MAX 1
#define INF 999999

#define MAX_DEPTH 8
#define MAX_CHILDREN 64

/** Types */

struct child;

typedef struct {

  game *game;
  struct child *children[MAX_CHILDREN];
  unsigned char nb_children;
  float valuation;

} node;

struct child {

  unsigned char from[2];
  unsigned char to[2];
  node *node;

};

/** Functions headers */

void ChessPlayer_Play(game *g, unsigned char p, unsigned char move[2][2]);
void ChessPlayer_IDAlphaBeta();
int ChessPlayer_AlphaBeta(unsigned char limit, unsigned char depth, node *n, int alpha, int beta, unsigned char min_or_max);
void ChessPlayer_Children(node *n);

#endif
