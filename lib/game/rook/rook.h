/**
*
*  Rook function headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef ROOK_H
#define ROOK_H

void ChessGame_Rook_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length);

#endif
