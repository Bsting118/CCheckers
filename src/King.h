#ifndef KING_H
#define KING_H

/* --THIS SHOULD INHERIT FROM CHECKER BUT ADD ON ITS OWN UNIQUE MOVESET-- */
#include "Checker.h"

typedef struct King
{
    Checker super; //< inherits from Checker
} King;

King *king_Me(void);                                  // equivalent to a "new King" operation (king symbols should be re-initialized to either '^' or 'v' depending on last team symbol)
void move_King(King *thisKing, char (*boardArray)[]); // Kings have a special move where they can move diagonally up OR diagonally down ('X' fashion)

#endif