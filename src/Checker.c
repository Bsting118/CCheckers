#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> //bool was not cool until C99
#include <stdint.h>
#include <string.h>
#include <ctype.h>
/*=====Checker class' interface=====*/
#include "Checker.h"
/*==================================*/

/* -===- CLASS CHANGE: Changed all size_t types for Checker object's xPos and yPos member vars to intmax_t -===-
(this included changing function parameter and body types that used col/row references to use intmax_t instead)*/

// Allocate enough memory to new checker piece:
Checker *allocTo_Checker()
{
    return (Checker *)malloc(sizeof(Checker));
}

void allocateToCheckerOverlapMatrixC99(size_t nthDim, Checker *(**checkerObjBoard)[nthDim])
{
    *checkerObjBoard = malloc(sizeof(Checker *[nthDim][nthDim]));
}

/*====================== NEW CONSTRUCTORS ======================*/
// (Default) Construct the values to this new checker piece:
Checker *new_Checkeri(Checker *alloc_Checker)
{
    alloc_Checker->teamSymbol = '-';
    alloc_Checker->xPos = 0;
    alloc_Checker->yPos = 0;
}

Checker *new_Checkerii(Checker *alloc_Checker, char tSymbol)
{
    alloc_Checker->teamSymbol = tSymbol;
    alloc_Checker->xPos = 0;
    alloc_Checker->yPos = 0;
}

Checker *new_Checkeriii(Checker *alloc_Checker, intmax_t x, intmax_t y)
{
    alloc_Checker->teamSymbol = '-';
    alloc_Checker->xPos = x;
    alloc_Checker->yPos = y;
}

Checker *new_Checkeriv(Checker *alloc_Checker, char tSymbol, intmax_t x, intmax_t y)
{
    alloc_Checker->teamSymbol = tSymbol;
    alloc_Checker->xPos = x; // columns
    alloc_Checker->yPos = y; // rows
}
/*==================================================================*/

/*====================== GET/ACCESSORS ======================*/
size_t get_CheckerX(Checker *thisChecker)
{
    return thisChecker->xPos; // stored column position
}
size_t get_CheckerY(Checker *thisChecker)
{
    //--SEG FAULT HAPPENED FROM THIS EXPRESSION
    return thisChecker->yPos; // stored row position
}
char get_CheckerTeam(Checker *thisChecker)
{
    return thisChecker->teamSymbol; // such as 'X' or 'O'
}
Checker *get_EnemyCheckerAt(intmax_t matrixRow, intmax_t matrixCol, size_t nDim, Checker *(*enemyCheckerMatrix)[nDim])
{
    return enemyCheckerMatrix[matrixRow][matrixCol]; // returns a matrix-accessed checkers object, or Checker*
}
Checker *get_AllyCheckerAt(intmax_t matrixRow, intmax_t matrixCol, size_t nDim, Checker *(*allyCheckerMatrix)[nDim])
{
    return allyCheckerMatrix[matrixRow][matrixCol]; // returns a matrix-accessed checkers object, or Checker*
}
Checker *get_SampleChecker(size_t nDim, Checker *(*sampledCheckerMatrix)[nDim])
{
    for (intmax_t row = 0; row < nDim; row++)
    {
        for (intmax_t col = 0; col < nDim; col++)
        {
            if (sampledCheckerMatrix[row][col] != NULL)
            {
                return sampledCheckerMatrix[row][col];
            }
        }
    }
}
/*============================================================*/

/*====================== SET/MUTATORS ======================*/
int set_CheckerX(Checker *thisChecker, intmax_t newX)
{
    if (thisChecker)
    {
        thisChecker->xPos = newX;
        return 1; // represents success
    }
    else
    {
        return 0; // represents failure
    }
}
int set_CheckerY(Checker *thisChecker, intmax_t newY)
{
    if (thisChecker)
    {
        thisChecker->yPos = newY;
        return 1; // represents success
    }
    else
    {
        return 0; // represents failure
    }
}
char set_CheckerTeam(Checker *thisChecker, char newTeamSymbol)
{
    if (thisChecker)
    {
        thisChecker->teamSymbol = newTeamSymbol;
        return newTeamSymbol; // represents success
    }
    else
    {
        return 0; // represents failure (NUL char)
    }
}
/*=============================================================*/

/*=====================================ACTION FUNCTIONS=========================================*/
void place_Checker(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim])
{
    // y is vertical rows, x is horizontal columns of board
    intmax_t row = get_CheckerY(thisChecker);
    intmax_t col = get_CheckerX(thisChecker);
    char team = get_CheckerTeam(thisChecker);

    if (boardArray[row][col] == 'B')
    {
        boardArray[row][col] = team;
        thisTeamsBoard[row][col] = thisChecker;
    }
    else
    {
        printf("Cannot place a checker on a invalid tile! \n");
    }
}

bool move_Checker(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim], Checker *(*enemyCheckerBoard)[nDim],
                  size_t thisTeamsScore)
{
    bool didMove = false; // Main thing to return and inform program if user moved or not

    char input[4]; // must reserve a fifth element for NULL terminating character
    intmax_t row = get_CheckerY(thisChecker);
    intmax_t col = get_CheckerX(thisChecker);
    char team = get_CheckerTeam(thisChecker);
    printf("Choose how you'd like to move this checker: \n");
    // Account for borders of board matrix
    Checker *sampleEnemy = get_SampleChecker(nDim, enemyCheckerBoard); // should get first actual enemy checker in its overlay matrix
    // Plan for Checker objects:
    //  -There will be 2 matrices for each Checker team (one for the enemy, one for the allied)
    //  -Both will overlap and align with the base board that has the tile and checker characters
    //  -When the base board transforms/rotates to next player, both the enemy matrix and ally matrix will rotate accordingly to fit the next player's view
    //  -There will be Checker * matrix functions to:
    //      -get a Checker object at certain position on overall board
    //      -sample a Checker from its matrix (for checking team signs mainly)
    //      -rotate thisMatrix (albeit a team 1 or team 2 matrix of checkers)
    char enemyCheckerTeam = get_CheckerTeam(sampleEnemy);
    intmax_t colToLeft = col - 1;
    intmax_t colToRight = col + 1;
    intmax_t rowAbove = row - 1;
    intmax_t rowBelow = row + 1;
    // Bugs are here below (expression are not evaluating properly for some reason...)
    bool enemyUpToTheLeft = (boardArray[row - 1][col - 1] == enemyCheckerTeam) ? true : false;
    bool enemyUpToTheRight = (boardArray[row - 1][col + 1] == enemyCheckerTeam) ? true : false;
    bool allyUpToTheLeft = (boardArray[row - 1][col - 1] == team) ? true : false;
    bool allyUpToTheRight = (boardArray[row - 1][col + 1] == team) ? true : false;
    // bool upRowIsWithinBoard = ((((row)-1) > -1) && (nDim > ((row)-1))) ? true : false;
    // bool upRowIsWithinBoard = ((row - 1 > -1) && (nDim > row - 1)) ? true : false;
    bool upRowIsWithinBoard = ((rowAbove > -1) && (nDim > rowAbove)) ? true : false;
    // printf("The answer to if the above row is within the board is: %d", upRowIsWithinBoard);
    //  bool leftColIsInBoard = ((((col)-1) > -1) && (nDim > ((col)-1))) ? true : false;
    bool leftColIsInBoard = ((colToLeft > -1) && (nDim > colToLeft)) ? true : false;
    // bool rightColIsInBoard = ((((col) + 1) > -1) && (nDim > ((col) + 1))) ? true : false;
    bool rightColIsInBoard = ((colToRight > -1) && (nDim > colToRight)) ? true : false;
    bool leftMoveGoAhead = false;
    bool rightMoveGoAhead = false;
    bool leftCaptureGoAhead = false;
    bool rightCaptureGoAhead = false;
    // Both directions are not out of bounds of the board (also can't jump over your own man):
    if (upRowIsWithinBoard && leftColIsInBoard && rightColIsInBoard && (allyUpToTheLeft != true) && (allyUpToTheRight != true))
    {
        if ((enemyUpToTheLeft != true) && (enemyUpToTheRight != true))
        {
            printf(">L1U1 \n");
            printf(">R1U1 \n");
            scanf("%4s", input);
            leftMoveGoAhead = true;
            rightMoveGoAhead = true;
        }
        else if ((enemyUpToTheLeft != true) && (enemyUpToTheRight == true)) // there's an enemy to the right
        {
            // Proceed with either no moves or capturing
            // checkCapture(Checker *thisChecker, unsigned long nDim, char (*boardArray)[nDim], unsigned long enemyX, unsigned long enemyY, char directionChar[], char enemyCheckerTeam)
            int canCaptureUR = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col + 1), "UR", enemyCheckerTeam);
            if (canCaptureUR)
            {
                printf(">R2U2 \n");
                scanf("%4s", input);
                rightCaptureGoAhead = true;
            }
            else
            {
                printf("This checker's move has been blocked by the enemy. \n");
            }
            // A capture vs dead end traversal is just a binary search tree!
        }
        else if ((enemyUpToTheRight != true) && (enemyUpToTheLeft == true)) // there's an enemy to the left
        {
            // Proceed with either no moves or capturing

            int canCaptureUL = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col - 1), "UL", enemyCheckerTeam);
            if (canCaptureUL)
            {
                printf(">L2U2 \n");
                scanf("%4s", input);
                leftCaptureGoAhead = true;
            }
            else
            {
                printf("This checker's move has been blocked by the enemy. \n");
            }
        }
        else
        {
            // There's enemies on both sides!

            int canCaptureUR = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col + 1), "UR", enemyCheckerTeam);
            int canCaptureUL = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col - 1), "UL", enemyCheckerTeam);
            if (canCaptureUR)
            {
                printf(">R2U2 \n");
                rightCaptureGoAhead = true;
            }
            if (canCaptureUL)
            {
                printf(">L2U2 \n");
                leftCaptureGoAhead = true;
            }

            if (canCaptureUR || canCaptureUL)
            {
                scanf("%4s", input);
            }
            else
            {
                printf("This checker's move has been blocked by the enemy. \n");
            }
        }
    }
    // The left direction is out of bounds of the board (also can't jump over your own man):
    else if (upRowIsWithinBoard && rightColIsInBoard && (allyUpToTheRight != true))
    {
        if ((enemyUpToTheRight != true))
        {
            printf(">R1U1 \n");
            scanf("%4s", input);
            rightMoveGoAhead = true;
        }
        else // there's an enemy to the right
        {
            // Proceed with either no moves or capturing

            int canCaptureUR = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col + 1), "UR", enemyCheckerTeam);
            if (canCaptureUR)
            {
                printf(">R2U2 \n");
                scanf("%4s", input);
                rightCaptureGoAhead = true;
            }
            else
            {
                printf("This checker's move has been blocked by the enemy. \n");
            }
        }
    }
    // The right direction is out of bounds of the board (also can't jump over your own man):
    else if (upRowIsWithinBoard && leftColIsInBoard && (allyUpToTheLeft != true))
    {
        if ((enemyUpToTheLeft != true))
        {
            printf(">L1U1 \n");
            scanf("%4s", input);
            leftMoveGoAhead = true;
        }
        else // there's an enemy to the left
        {
            // Proceed with either no moves or capturing
            int canCaptureUL = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col - 1), "UL", enemyCheckerTeam);
            if (canCaptureUL)
            {
                printf(">L2U2 \n");
                scanf("%4s", input);
                leftCaptureGoAhead = true;
            }
            else
            {
                printf("This checker's move has been blocked by the enemy. \n");
            }
        }
    }
    // Both moves are out of bounds of the board:
    else
    {
        printf("No available moves for this checker, please select a different one. \n");
    }

    int wasACapture = carryOut_UsersCheckerMove(input, thisChecker, nDim, boardArray, thisTeamsBoard, enemyCheckerBoard, thisTeamsScore, leftMoveGoAhead, rightMoveGoAhead, leftCaptureGoAhead, rightCaptureGoAhead);
    if (wasACapture >= 0) //-1 means invalid or no move
    {
        didMove = true;
    }

    // while(wasACapture):
    //   wasACapture = 0; (aka, false)
    //   Call repeatCapture_with_Checker here
    //   Call wasACapture = carryOut_UsersCheckerMove again here
    //(basic idea is they can keep going until no more available captures or they quit doing a repeat capture)
    //(note: will have to have some way to pass the canDoLeftCapture and canDoRightCapture from repeatCapture to the carryOut function)
    while (wasACapture == 1)
    {
        wasACapture = 0;
        repeatCapture_with_Checker(input, thisChecker, nDim, boardArray, thisTeamsBoard, enemyCheckerBoard, thisTeamsScore, leftCaptureGoAhead, rightCaptureGoAhead);
        wasACapture = carryOut_UsersCheckerMove(input, thisChecker, nDim, boardArray, thisTeamsBoard, enemyCheckerBoard, thisTeamsScore, false, false, leftCaptureGoAhead, rightCaptureGoAhead);
    }

    // TO HAVE REPEAT CAPTURES JUST MAKE RECURSIVE CALLS AFTER CAPTURE HAS BEEN MADE
    //  -For free-move capture style:
    //      -If a capture is made repeat scan move process but a second move can only be made IF AND ONLY IF that move is another capture.
    //      -Else, the player would end their turn after their previous capture.
    //          -Going to use a bool recursive parameter to have as a flag that a repeat capture is being checked
    //          -Going outside of board, blocked in, or no spaces to capture will be base cases

    return didMove;
}

int carryOut_UsersCheckerMove(char input[], Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim], Checker *(*enemyCheckerBoard)[nDim], size_t thisTeamsScore,
                              bool canFreelyGoLeft, bool canFreelyGoRight, bool canDoLeftCapture, bool canDoRightCapture)
{
    intmax_t row = get_CheckerY(thisChecker);
    intmax_t col = get_CheckerX(thisChecker);
    char team = get_CheckerTeam(thisChecker);
    int didCapture = 0;

    if (strlen(input) > 0)
    {
        int inpCharIndex = 0;
        // Format input to be consistently all uppercase:
        while (input[inpCharIndex]) // terminates at NUL char which compares to 0
        {
            input[inpCharIndex] = toupper(input[inpCharIndex]);
            inpCharIndex++;
        }

        // Note: move() perspective is always based on the current player playing from the bottom of the board:
        if ((canFreelyGoLeft) && (strstr(input, "L") && strstr(input, "1") && strstr(input, "U") != 0) && (strstr(input, "2") == 0)) // strstr is a substring check; returns pointer to first character (0 is the same thing as NULL when it comes to pointers)
        {
            boardArray[row][col] = 'B'; // previous tile is now empty (checkers can only be on Black tiles)
            thisTeamsBoard[row][col] = NULL;
            // boardArray[(row)-1][(col)-1] = team;
            set_CheckerY(thisChecker, (row - 1));
            set_CheckerX(thisChecker, (col - 1));
            place_Checker(thisChecker, nDim, boardArray, thisTeamsBoard); // this replaces the commented-out lines
            // thisTeamsBoard[(row)-1][(col)-1] = thisChecker;
        }
        else if ((canFreelyGoRight) && (strstr(input, "R") && strstr(input, "1") && strstr(input, "U") != 0) && (strstr(input, "2") == 0)) //>R1U1
        {
            boardArray[row][col] = 'B'; // previous tile is now empty (checkers can only be on Black tiles)
            thisTeamsBoard[row][col] = NULL;
            set_CheckerY(thisChecker, (row - 1));
            set_CheckerX(thisChecker, (col + 1));
            place_Checker(thisChecker, nDim, boardArray, thisTeamsBoard);
        }
        else if ((canDoLeftCapture) && (strstr(input, "L") && strstr(input, "2") && strstr(input, "U") != 0) && (strstr(input, "1") == 0))
        {
            // They made a left capture; do extra steps to remove enemy checker from board and add to this player's score:
            boardArray[row][col] = 'B';
            thisTeamsBoard[row][col] = NULL;
            Checker *enemyCheckerTarget = get_EnemyCheckerAt((row - 1), (col - 1), nDim, enemyCheckerBoard); // fetch the target enemy from its checker objects array
            capture_ThisChecker(enemyCheckerTarget, nDim, boardArray, thisTeamsScore);
            enemyCheckerBoard[(row)-1][(col)-1] = NULL;
            //(make a Checker class function that assumes the enemy object array is made and can iterate over each, looking into each member values to find correct one)
            // boardArray[(row)-2][(col)-2] = get_CheckerTeam(thisChecker);
            set_CheckerY(thisChecker, (row - 2));
            set_CheckerX(thisChecker, (col - 2));
            // thisTeamsBoard[(row)-2][(col)-2] = thisChecker;
            place_Checker(thisChecker, nDim, boardArray, thisTeamsBoard);
            // UPDATE RESPONSE ON WHAT USER DID (THEY CAPTURED):
            didCapture = 1;
        }
        else if ((canDoRightCapture) && (strstr(input, "R") && strstr(input, "2") && strstr(input, "U") != 0) && (strstr(input, "1") == 0))
        {
            // They made a left capture; do extra steps to remove enemy checker from board and add to this player's score:
            boardArray[row][col] = 'B';
            thisTeamsBoard[row][col] = NULL;
            Checker *enemyCheckerTarget = get_EnemyCheckerAt((row - 1), (col + 1), nDim, enemyCheckerBoard); // fetch the target enemy from its checker objects array
            capture_ThisChecker(enemyCheckerTarget, nDim, boardArray, thisTeamsScore);                       // NEED A WAY TO GET CORRESONDING ENEMY CHECKER OBJECT --> Store team checker objects in own obj array
            enemyCheckerBoard[(row)-1][(col) + 1] = NULL;
            //(make a Checker class function that assumes the enemy object array is made and can iterate over each, looking into each member values to find correct one)
            set_CheckerY(thisChecker, (row - 2));
            set_CheckerX(thisChecker, (col + 2));
            place_Checker(thisChecker, nDim, boardArray, thisTeamsBoard);
            // UPDATE RESPONSE ON WHAT USER DID (THEY CAPTURED):
            didCapture = 1;
        }
        else
        {
            printf("Unrecognized move; exiting this selection...\n");
            didCapture = -1;
        }
    }
    else
    {
        printf("You chose not to enter a move; exiting this selection...\n");
        didCapture = -1;
    }

    return didCapture; //-1 = no move made, 0 = move made but no capture, 1 = capture move was made
}

void repeatCapture_with_Checker(char input[], Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisTeamsBoard)[nDim],
                                Checker *(*enemyCheckerBoard)[nDim], size_t thisTeamsScore, bool leftCaptureGoAhead, bool rightCaptureGoAhead)
{
    intmax_t row = get_CheckerY(thisChecker);
    intmax_t col = get_CheckerX(thisChecker);
    char team = get_CheckerTeam(thisChecker);
    Checker *sampleEnemy = get_SampleChecker(nDim, enemyCheckerBoard);
    char enemyCheckerTeam = get_CheckerTeam(sampleEnemy);
    bool enemyUpToTheLeft = (boardArray[row - 1][col - 1] == enemyCheckerTeam) ? true : false;
    bool enemyUpToTheRight = (boardArray[row - 1][col + 1] == enemyCheckerTeam) ? true : false;
    bool allyUpToTheLeft = (boardArray[row - 1][col - 1] == team) ? true : false;
    bool allyUpToTheRight = (boardArray[row - 1][col + 1] == team) ? true : false;
    bool upRowIsWithinBoard = ((((row)-1) > -1) && (nDim > ((row)-1))) ? true : false;
    bool leftColIsInBoard = ((((col)-1) > -1) && (nDim > ((col)-1))) ? true : false;
    bool rightColIsInBoard = ((((col) + 1) > -1) && (nDim > ((col) + 1))) ? true : false;
    leftCaptureGoAhead = false;
    rightCaptureGoAhead = false;

    if (upRowIsWithinBoard && leftColIsInBoard && rightColIsInBoard && (allyUpToTheLeft != true) && (allyUpToTheRight != true))
    {
        if ((enemyUpToTheLeft != true) && (enemyUpToTheRight != true))
        {
            printf("No further captures can be made. \n");
        }
        else if ((enemyUpToTheLeft != true) && (enemyUpToTheRight == true)) // there's an enemy to the right
        {
            int canCaptureUR = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col + 1), "UR", enemyCheckerTeam);
            if (canCaptureUR)
            {
                rightCaptureGoAhead = true;
            }
        }
        else if ((enemyUpToTheRight != true) && (enemyUpToTheLeft == true)) // there's an enemy to the left
        {
            int canCaptureUL = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col - 1), "UL", enemyCheckerTeam);
            if (canCaptureUL)
            {
                leftCaptureGoAhead = true;
            }
        }
        else // there's an enemy on both sides
        {
            int canCaptureUR = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col + 1), "UR", enemyCheckerTeam);
            int canCaptureUL = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col - 1), "UL", enemyCheckerTeam);
            if (canCaptureUR)
            {
                rightCaptureGoAhead = true;
            }
            if (canCaptureUL)
            {
                leftCaptureGoAhead = true;
            }
        }
    }
    // The left direction is out of bounds of the board (also can't jump over your own man):
    else if (upRowIsWithinBoard && rightColIsInBoard && (allyUpToTheRight != true))
    {
        if (enemyUpToTheRight == true)
        {
            int canCaptureUR = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col + 1), "UR", enemyCheckerTeam);
            if (canCaptureUR)
            {
                rightCaptureGoAhead = true;
            }
        }
    }
    // The right direction is out of bounds of the board (also can't jump over your own man):
    else if (upRowIsWithinBoard && leftColIsInBoard && (allyUpToTheLeft != true))
    {
        if (enemyUpToTheLeft == true)
        {
            int canCaptureUL = checkCapture(thisChecker, nDim, boardArray, abs(row - 1), abs(col - 1), "UL", enemyCheckerTeam);
            if (canCaptureUL)
            {
                leftCaptureGoAhead = true;
            }
        }
    }
    // Both moves are out of bounds of the board or both allies are there:
    else
    {
        printf("\nNo further captures can be made. \n");
    }

    // Begin outputing options IF there is another capture available:
    if (rightCaptureGoAhead || leftCaptureGoAhead)
    {
        printf("There are repeat captures available at:\n");
    }
    if (rightCaptureGoAhead && leftCaptureGoAhead)
    {
        printf(">R2U2 \n");
        printf(">L2U2 \n");
        scanf("%4s", input);
    }
    else if (rightCaptureGoAhead)
    {
        printf(">R2U2 \n");
        scanf("%4s", input);
    }
    else if (leftCaptureGoAhead)
    {
        printf(">L2U2 \n");
        scanf("%4s", input);
    }
    // IF INPUT IS ANYTHING BUT THESE OPTIONS THEN THE USER HAS OPTED TO NOT DO ANOTHER CAPTURE; END THEIR TURN
}

// int will be boolean-based; 1 = true, 0 = false
int checkCapture(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], intmax_t enemyX, intmax_t enemyY, char directionChar[], char enemyCheckerTeam)
{
    int canCapture;
    // Check if tile behind current enemy tile is blank or has a 'B'

    // First, make sure the enemy and this checker are diagonal neighbors (within range)
    int yDistance = abs(get_CheckerY(thisChecker) - enemyY);
    int xDistance = abs(get_CheckerX(thisChecker) - enemyX);
    bool inRange = (yDistance == 1 && xDistance == 1) ? true : false;
    char enemyTile = boardArray[enemyY][enemyX];

    // Next, make sure the enemy tile isn't blank and its an enemy there (not ally):
    bool isEnemyPresent = (enemyTile == enemyCheckerTeam) ? true : false;

    if (isEnemyPresent && inRange)
    {
        if (directionChar == "UL") // King will have its own version of this method; this is for regular checker...
        {
            // If it's a blank Black tile behind neighboring enemy (in same direction), we good to capture!
            canCapture = (boardArray[enemyY - 1][enemyX - 1] == 'B') ? 1 : 0;
        }
        else //"UR" (upper-right)
        {
            canCapture = (boardArray[enemyY - 1][enemyX + 1] == 'B') ? 1 : 0;
        }
    }
    else
    {
        canCapture = 0;
    }

    return canCapture;
}

void capture_ThisChecker(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], size_t enemyCounter)
{
    // Each checker object pertains and stores info to one checker on the board
    intmax_t row = get_CheckerY(thisChecker);
    intmax_t col = get_CheckerX(thisChecker);
    enemyCounter = enemyCounter + 1;
    boardArray[row][col] = 'B';
    free(thisChecker); // it literally gets deleted from memory!
}

void wipe_ThisCheckers_Team(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim])
{
    char teamToWipe = get_CheckerTeam(thisChecker);
    for (int row = 0; row < nDim; row++)
    {
        for (int col = 0; col < nDim; col++)
        {
            if (boardArray[row][col] == teamToWipe)
            {
                boardArray[row][col] = 'B';
            }
        }
    }
    free(thisChecker);
}
/*===============================================================================================================*/
