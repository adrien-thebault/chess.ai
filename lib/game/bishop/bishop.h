/**
*
*  Bishop function headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef BISHOP_H
#define BISHOP_H

void ChessGame_Bishop_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length);

#endif
