#ifndef CHECKER_H
#define CHECKER_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
// To mimic classes in C (since it doesn't have them), we can use structs for member stuff and have function pointers (in struct) to point to pertaining functions

// To shorten unsigned long data type references:
typedef unsigned long ulong;
// To shorten (Checker *) data type references [used really for overloads]:
// typedef Checker *checkerPtr;

// Declare the Checker class' type: (defining a name for it) [THIS STRUCT STAYS PRIVATE IN .H]
typedef struct Checker
{
    char teamSymbol;
    // X's and y's on a checkerboard will never be negative in this implementation.
    intmax_t xPos;
    intmax_t yPos;
} Checker;

// typedef Checker *checkerPtr;

// Declare functions and accessors
Checker *allocTo_Checker(void); // void is a parameter here to ensure no arguments can be passed [() could mean any number of args]
void allocateToCheckerOverlapMatrixC99(size_t nthDim, Checker *(**checkerObjBoard)[nthDim]);

Checker *new_Checkeri(Checker *alloc_Checker);
Checker *new_Checkerii(Checker *alloc_Checker, char tSymbol);
Checker *new_Checkeriii(Checker *alloc_Checker, intmax_t x, intmax_t y);
Checker *new_Checkeriv(Checker *alloc_Checker, char tSymbol, intmax_t x, intmax_t y);
// Allow function overloads for the "new_Checker()":
/* -- NOTE: NOT INCLUDING ANYMORE DUE TO HUGE CODE BLOAT AND REQUIRES TEDIOUS EXTERNAL LIBRARY SETUP FOR GCC -- */
//#define new_Checker(...) OVERLOAD(new_Checker, (__VA_ARGS__),                  \
                                  (new_Checkeri, (Checker *)),                 \
                                  (new_Checkerii, (Checker *, char)),          \
                                  (new_Checkeriii, (Checker *, size_t, size_t)), \
                                  (new_Checkeriv, (Checker *, char, size_t, size_t)))
// #define OVERLOAD_FUNCTIONS (new_Checker)

// For board startup:
void place_Checker(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim]);
// For the player to move (will include checks for capture and board edges):
bool move_Checker(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim], Checker *(*enemyCheckerBoard)[nDim],
                  size_t thisTeamsScore);
// Process user input to a move_Checker possibility (return if user chose a capture to determine if should start checking for repeat captures):
int carryOut_UsersCheckerMove(char input[], Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim], Checker *(*enemyCheckerBoard)[nDim],
                              size_t thisTeamsScore, bool canFreelyGoLeft, bool canFreelyGoRight, bool canDoLeftCapture, bool canDoRightCapture);
// For doing n amount of capture moves that the current player can take (this should NOT be forced):
void repeatCapture_with_Checker(char input[], Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim],
                                Checker *(*enemyCheckerBoard)[nDim], size_t thisTeamsScore, bool leftCaptureGoAhead, bool rightCaptureGoAhead);
// See if the enemy in a direction is capturable (blank space behind it):
int checkCapture(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], intmax_t enemyX, intmax_t enemyY, char directionChar[], char enemyCheckerTeam);
// This is like a "free object" method to get it out of memory (could award points if possible; write 'B' on board where checker was removed):
void capture_ThisChecker(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], size_t enemyCounter);
void wipe_ThisCheckers_Team(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim]);

// OBS note: these used to be (unsigned long *) and (char *) return types:
size_t get_CheckerX(Checker *thisChecker);  // second subscript in board[][x] (column)
size_t get_CheckerY(Checker *thisChecker);  // first subscript in board[y][] (row)
char get_CheckerTeam(Checker *thisChecker); // e.g. "red vs black", which can be represented via 'X' vs 'O'

// Overlapping checker matrix will hold object pointers to Checker objects, same size as base checkered board:
Checker *get_EnemyCheckerAt(intmax_t matrixRow, intmax_t matrixCol, size_t nDim, Checker *(*enemyCheckerMatrix)[nDim]); // how you can get an enemy's Checker obj
Checker *get_AllyCheckerAt(intmax_t matrixRow, intmax_t matrixCol, size_t nDim, Checker *(*allyCheckerMatrix)[nDim]);   // how you can get an allied Checker obj
Checker *get_SampleChecker(size_t nDim, Checker *(*sampledCheckerMatrix)[nDim]);

int set_CheckerX(Checker *thisChecker, intmax_t newX);
int set_CheckerY(Checker *thisChecker, intmax_t newY);
char set_CheckerTeam(Checker *thisChecker, char newTeamSymbol);

#endif
