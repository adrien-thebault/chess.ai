/**
*
*  ChessPlayer functions headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef PLAYER_H
#define PLAYER_H

/** Constants */

#define ALPHABETA   0x00
#define MTDF        0x01
#define PVS         0x02

#define MIN 0
#define MAX 1
#define INF 999999

#define MAX_DEPTH 6
#define NB_CORES 8

/** Functions headers */

void ChessPlayer_Play(game *g, unsigned char p, unsigned char move[2][2], unsigned char algorithm);
void ChessPlayer_ID(game *g, unsigned char move[2][2], unsigned char p, unsigned char algorithm);
float ChessPlayer_MTDF(game *g, unsigned char player, float f, unsigned char depth, unsigned char move[2][2], unsigned long long int *visited);
float ChessPlayer_AlphaBeta(unsigned char limit, unsigned char depth, game *g, unsigned char player, unsigned char move[2][2], unsigned long long int *visited, float alpha, float beta, unsigned char min_or_max);
float ChessPlayer_PVSplit(unsigned char limit, unsigned char depth, game *g, unsigned char player, unsigned char move[2][2], unsigned long long int *visited, float alpha, float beta, unsigned char min_or_max);
void ChessPlayer_PossibleMoves(game *g, unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length);

#endif
