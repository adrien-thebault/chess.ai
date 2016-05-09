/**
*
*  Queen function headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef QUEEN_H
#define QUEEN_H

void ChessGame_Queen_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length);

#endif
