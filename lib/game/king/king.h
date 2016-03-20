/**
*
*  King function headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef KING_H
#define KING_H

void ChessGame_King_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length);

#endif
