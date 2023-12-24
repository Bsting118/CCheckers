#include <stdio.h>
#include <stdbool.h> //bool was not cool until C99
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "Checker.c"

long strToLong(char *string)
{
	long result = 0;
	int sign = 1;
	int strIndex = 0;
	// If first character (0th char) is a '-', it's a negative sign...so:
	if (string[strIndex] == '-')
	{
		sign = -1;
		strIndex++; // update that first digit to read is actually one past negative sign
	}
	while (string[strIndex] != '\0')
	{
		// ASCII manipulation with the (- '0'); '0' = value 048, '1' = value 049, etc.
		//(result * 10) will shift last value up a numerical place; such as to tens or hundreds (per each new iteration)
		result = (result * 10) + (string[strIndex] - '0');
		strIndex++;
	}
	return sign * result; // apply found sign at the end
}

uintmax_t findIntegralDecimalPlaces(intmax_t bigNum)
{
	uintmax_t numOfPlaces;

	// If we can do the decimal place evaluation in near constant time with log10 math function, do so; else, do it the hard way:
	if (abs(bigNum) <= 99999999)
	{
		numOfPlaces = (uintmax_t)(log10(bigNum) + 1);
	}
	else
	{
		numOfPlaces = 0;
		intmax_t bigNumCopy = bigNum;
		while (bigNumCopy != 0)
		{
			numOfPlaces++;
			bigNumCopy /= 10;
		}
	}
	return numOfPlaces;
}
intmax_t generateRandomNumBetween(intmax_t from, intmax_t to)
{
	// rand() can generate any number from 0 to a general constant RAND_MAX (is at least 32767)
	//(to-from + 1) is formula for length of a range (that may start at any point; not just 0)
	// X % (to-from) would exclude the divisor as being a potential max result, so a +1 is necessary to make (to-from+1) be an inclusive modulus algorithm
	//	-Once it wraps around, say 24 % 5 = 4 then 25 % 5 would = 0, if we wanted to actually include 5 as a max in random range, we would do 25 % 6 instead to = 5.
	// Need a (+ minimum or +from) to ensure that even rand() generates 0, in that case where remainder is 0, it boosts up to the minimum in the range
	//	-Also, all subsequent remainders from the mod (range length) are incremented up within the range, and with the max being = greatest remainder + from = to
	return (intmax_t)((rand() % (to - from + 1)) + from);
}

char **allocateToBoardC90(const int nthDim)
{
	// The calloc() function reserves storage space for an array of NUM elements, each of length SIZE bytes:
	// calloc(size_t NUM, size_t SIZE)
	char *values = calloc(nthDim * nthDim, sizeof(char)); // In our 2D board array, we would have nxn (char)elements (8x8=64 elements)

	// Simple way is to allocate a memory block of size row and access its elements using simple pointer arithmetic:
	char **rows = malloc(nthDim * sizeof(char)); // pointer to overall space

	// Iterate through each *row; 0 to nthRow
	for (int rowIndex = 0; rowIndex < nthDim; rowIndex++)
	{
		rows[rowIndex] = values + (rowIndex * nthDim);
	}
	// Returns an array of pointers (array of arrays)
	return rows;
}

char **getCheckeredBoardC90(const int nthDim)
{
	bool startWithWhiteTile = true;

	//**array[rowIndex][columnIndex]
	// char **arr[nthDim][nthDim];

	// C90 version (lotta pointers; be aware of performance overhead):
	char **arr = allocateToBoardC90(nthDim);

	// Make the board checkered by alternating char assignments based on a bool latch
	for (int rowIndex = 0; rowIndex < nthDim; rowIndex++)
	{
		if (startWithWhiteTile)
		{
			for (int columnIndex = 0; columnIndex < nthDim; columnIndex++)
			{
				arr[rowIndex][columnIndex] = (columnIndex % 2 == 0) ? 'W' : 'B';
			}
			startWithWhiteTile = false;
		}
		else
		{
			for (int columnIndex = 0; columnIndex < nthDim; columnIndex++)
			{
				arr[rowIndex][columnIndex] = (columnIndex % 2 == 0) ? 'B' : 'W';
			}
			startWithWhiteTile = true;
		}
	}
	return arr;
}

void allocateToBoardC99(size_t nthDim, char (**boardArray)[nthDim])
{
	*boardArray = malloc(sizeof(char[nthDim][nthDim]));
}

void getCheckeredBoardC99(size_t nthDim, char (*boardArray)[nthDim])
{
	bool startWithWhiteTile = true;

	// C99 version (more elegant):

	// Make the board checkered by alternating char assignments based on a bool latch
	for (intmax_t rowIndex = 0; rowIndex < nthDim; rowIndex++)
	{
		if (startWithWhiteTile)
		{
			for (intmax_t columnIndex = 0; columnIndex < nthDim; columnIndex++)
			{
				boardArray[rowIndex][columnIndex] = (columnIndex % 2 == 0) ? 'W' : 'B';
			}
			startWithWhiteTile = false;
		}
		else
		{
			for (intmax_t columnIndex = 0; columnIndex < nthDim; columnIndex++)
			{
				boardArray[rowIndex][columnIndex] = (columnIndex % 2 == 0) ? 'B' : 'W';
			}
			startWithWhiteTile = true;
		}
	}
}

// Gets the number of checkers per starting row at the beginning of the game:
//(used to make sure we don't go overboard when placing checkers at the start)
size_t numOfCheckersPerRow(size_t nDim)
{
	return (size_t)(nDim / 2);
}

// Finds out how many rows of starting checkers there should be (2 rows are blank in middle, remaining is filled)
//(2 rows are blank in middle, remaining is filled; 8x8 has 3 rows of checkers and 2 blank in middle, 10x10 has 4 rows and 2 blank in middle)
size_t numOfStartingRows(size_t nDim)
{
	return (size_t)((nDim - 2) / 2);
}

// Locates the first valid black tile square in this checkerboard's row:
intmax_t findFirstBlackTileInRow(size_t row, size_t nDim, char (*boardArray)[nDim])
{
	intmax_t col = 0;
	while (boardArray[row][col] != 'B')
	{
		col++;
		if (col >= nDim)
		{
			col = -1;
			break;
		}
	}
	return col;
}

char whoStartsOnBottom(size_t nDim, char (*boardArray)[nDim])
{
	// A checkered pattern, must be either black tile followed by white tile in start of a row, or white tile followed by black tile:
	if ((boardArray[nDim - 1][0] != 'B') && (boardArray[nDim - 1][0] != 'W'))
	{
		return boardArray[nDim - 1][0];
	}
	else if ((boardArray[nDim - 1][1] != 'B') && (boardArray[nDim - 1][1] != 'W'))
	{
		return boardArray[nDim - 1][1];
	}
	else
	{
		return 'B'; // nobody is on bottom!
	}
}

void initCheckerTeams(size_t nDim, char (*boardArray)[nDim], Checker *(*team1Checkers)[nDim], Checker *(*team2Checkers)[nDim])
{
	char teamOne = 'O';
	char teamTwo = 'X';

	size_t teamsRows = numOfStartingRows(nDim);
	size_t teamsCheckersInRow = numOfCheckersPerRow(nDim);

	// Set all spots to NULL in each object array first (then set the correct spots to objects)
	for (intmax_t row = 0; row < nDim; row++)
	{
		for (intmax_t col = 0; col < nDim; col++)
		{
			team1Checkers[row][col] = NULL;
			team2Checkers[row][col] = NULL;
		}
	}

	// start from row 0 and fill each row until teamsRows has been met
	// stop
	// start from row 5 and fill each row until teamsRows has been met again
	intmax_t row = 0;
	while (row < teamsRows) // fill with 1st team's checkers
	{
		int firstTile = findFirstBlackTileInRow(row, nDim, boardArray);
		for (size_t teamsChecker = 1; teamsChecker <= teamsCheckersInRow; teamsChecker++)
		{
			boardArray[row][firstTile] = teamOne;
			team1Checkers[row][firstTile] = new_Checkeriv(allocTo_Checker(), teamOne, firstTile, row);
			firstTile = firstTile + 2;
		}
		row++;
	}
	row = row + 2; // jump over the 2-wide blank mid-field

	while (row < nDim) // fill in remaining rows past blank mid-field with other team's checkers
	{
		int firstTile = findFirstBlackTileInRow(row, nDim, boardArray);
		for (size_t teamsChecker = 1; teamsChecker <= teamsCheckersInRow; teamsChecker++)
		{
			boardArray[row][firstTile] = teamTwo;
			team2Checkers[row][firstTile] = new_Checkeriv(allocTo_Checker(), teamTwo, firstTile, row);
			firstTile = firstTile + 2;
		}
		row++;
	}
}

void printAvailCheckersToSelect(size_t nDim, Checker *(*currTeamsCheckers)[nDim])
{
	size_t teamsMaxCheckers = numOfCheckersPerRow(nDim) * numOfStartingRows(nDim);
	char *availCheckers[teamsMaxCheckers];
	size_t currIndex = 0;
	size_t newLineCounter = 0;
	size_t amtToPrintEachLine = numOfCheckersPerRow(nDim);

	printf("\nHere are your available checkers to select:\n");
	for (intmax_t row = 0; row < nDim; row++)
	{
		for (intmax_t col = 0; col < nDim; col++)
		{
			if (currTeamsCheckers[row][col] != NULL)
			{
				if ((newLineCounter % (amtToPrintEachLine)) == 0)
				{
					printf("\n>");
				}
				printf("R%dC%d ", row, col);
				newLineCounter++;
			}
		}
	}

	printf("\n");
}

void printCheckersDisplayBoard(size_t nDim, char (*boardArray)[nDim])
{
	printf("\n");

	for (intmax_t row = 0; row < nDim; row++)
	{
		printf("[");
		for (intmax_t column = 0; column < nDim; column++)
		{
			printf("%c ", boardArray[row][column]);
		}
		printf("]\n");
	}
}

void selectAChecker(size_t nDim, Checker *(*currTeamsCheckers)[nDim], char (*boardArray)[nDim], Checker *(*enemyTeamsCheckers)[nDim], size_t currTeamsScore)
{
	bool isValidSelection = false;
	do
	{
		printf("Please select one of your checkers to move, using R1C1 notation.");
		printAvailCheckersToSelect(nDim, currTeamsCheckers); // Added this so we can see what we can do
		size_t decimalPlaces = findIntegralDecimalPlaces(nDim);
		size_t inputLen = 2 + (2 * decimalPlaces) + 1; // if 1s place dimension = 2 + (2 * 1), if 10s place dimension =  2 + (2 * 2), if 100s place dimension = 2 + (2 * 3), etc. (need extra +1 for '\0')
		char input[inputLen];						   // this works just for testing purposes; later on will need a way to allow for maximum input (such as 100 board with R99C99 = 6 characters)

		// Dynamically build the input scan buffer:
		char inpBuffer[inputLen + 2];
		char strInputLen[inputLen];
		snprintf(strInputLen, sizeof strInputLen, "%zu", inputLen); // TODO: Change to use intmax_T converts.
		strcpy(inpBuffer, "%");
		strcat(inpBuffer, strInputLen);
		strcat(inpBuffer, "s");

		scanf(inpBuffer, input); // inputBuffer is more dynamic than say "%4s"

		// R1C1 = R at index 0, C at index 2 = str length is 4
		// R11C11 = R at index 0, C at index 3 = str length is 6
		// R111C111 = R at index 0, C at index 4 = str length is 8
		// R1111C1111 = R at index 0, C at index 5 = str length is 10
		// Thus, R = index 0, C = index (length/2)

		// Extract the number that comes after 'R' but before 'C' (meaning the row number):
		size_t numberLen = (size_t)(strlen(input) / 2);
		char strInpRow[numberLen];
		for (size_t rowEnum = 1; rowEnum < (strlen(input) / 2); rowEnum++)
		{
			strInpRow[rowEnum - 1] = input[rowEnum];
		}
		size_t inpRow;
		sscanf(strInpRow, "%zu", &inpRow); // converts string to size_t figure (more type safe than str to long version; available in C99+)
		//(in "%zu", z is a length modifier and u is for unsigned type)

		// Repeat for column input conversion:
		char strInpCol[numberLen];
		for (size_t colEnum = (strlen(input) / 2) + 1; colEnum < strlen(input); colEnum++)
		{
			strInpCol[colEnum - ((strlen(input) / 2) + 1)] = input[colEnum];
		}
		size_t inpCol;
		sscanf(strInpCol, "%zu", &inpCol);

		if (currTeamsCheckers[inpRow][inpCol] != NULL)
		{
			isValidSelection = true;
			// Give possible moves for the valid checker:
			Checker *selectedChecker = currTeamsCheckers[inpRow][inpCol];
			// TODO: Edit move method to feedback if the chosen checker was actually moved to a new space or not; if not say valid selection false so they can select again
			//--SEG FAULT IN MOVE_CHECKER--
			bool didTheyMakeTheirMove = move_Checker(selectedChecker, nDim, boardArray, currTeamsCheckers, enemyTeamsCheckers, currTeamsScore);
			isValidSelection = (didTheyMakeTheirMove) ? true : false;
		}
		else
		{
			isValidSelection = false;
			printf("Sorry, that's not one of your team's checkers. Please try again.\n");
		}
	} while (isValidSelection == false);
}

void translateAcrossBoard(Checker *thisChecker, size_t nDim, char (*boardArray)[nDim], Checker *(*thisCheckersBoard)[nDim])
{
	// Get the pre-translate positions:
	intmax_t row = get_CheckerY(thisChecker);
	intmax_t col = get_CheckerX(thisChecker);

	// Calculate new translated positions using previous position to across board distance:
	intmax_t newRow = (nDim - 1) - get_CheckerY(thisCheckersBoard[row][col]);
	intmax_t newCol = (nDim - 1) - get_CheckerX(thisCheckersBoard[row][col]);
	// Checker *checkerToTranslate = thisChecker;

	// Update object encapsulated attributes:
	set_CheckerY(thisChecker, newRow);
	set_CheckerX(thisChecker, newCol);

	// Update array board pointers to new position; and old position is nullified:
	thisCheckersBoard[newRow][newCol] = thisChecker;
	thisCheckersBoard[row][col] = NULL;

	// If this old tile has already been changed to be top-to-bottom checker, don't change tile sign; else, reset it.
	boardArray[row][col] = (boardArray[row][col] != get_CheckerTeam(thisChecker)) ? boardArray[row][col] : 'B';
	boardArray[newRow][newCol] = get_CheckerTeam(thisChecker);
}

void swapBoard(size_t nDim, Checker *(*currTeamsCheckers)[nDim], char (*boardArray)[nDim], Checker *(*enemyTeamsCheckers)[nDim])
{
	// currTeam is on bottom of board, enemyTeam is on top of the board; must also reflect rotation on display boardArray:

	// For each top checker:
	for (intmax_t row = 0; row < nDim; row++)
	{
		for (intmax_t col = 0; col < nDim; col++)
		{
			if (enemyTeamsCheckers[row][col] != NULL)
			{
				/*
				// A valid top checker object:
				intmax_t newRow = (nDim - 1) - get_CheckerY(enemyTeamsCheckers[row][col]);
				intmax_t newCol = (nDim - 1) - get_CheckerX(enemyTeamsCheckers[row][col]);
				Checker *thisChecker = enemyTeamsCheckers[row][col];
				enemyTeamsCheckers[newRow][newCol] = thisChecker;
				enemyTeamsCheckers[row][col] = NULL;
				// Update object encapsulated attributes:
				set_CheckerY(enemyTeamsCheckers[newRow][newCol], newRow);
				set_CheckerX(enemyTeamsCheckers[newRow][newCol], newCol);
				// If this old tile has already been changed to be bottom-to-top checker, don't change tile sign; else, reset it.
				boardArray[row][col] = (boardArray[row][col] != get_CheckerTeam(thisChecker)) ? boardArray[row][col] : 'B';
				boardArray[newRow][newCol] = get_CheckerTeam(thisChecker);*/
				translateAcrossBoard(enemyTeamsCheckers[row][col], nDim, boardArray, enemyTeamsCheckers);
			}

			if (currTeamsCheckers[row][col] != NULL)
			{
				/*
				// A valid bottom checker object:
				intmax_t newRow = (nDim - 1) - get_CheckerY(currTeamsCheckers[row][col]);
				intmax_t newCol = (nDim - 1) - get_CheckerX(currTeamsCheckers[row][col]);
				Checker *thisChecker = currTeamsCheckers[row][col];
				currTeamsCheckers[newRow][newCol] = thisChecker;
				currTeamsCheckers[row][col] = NULL;
				// Update object encapsulated attributes:
				set_CheckerY(currTeamsCheckers[newRow][newCol], newRow);
				set_CheckerX(currTeamsCheckers[newRow][newCol], newCol);
				// If this old tile has already been changed to be top-to-bottom checker, don't change tile sign; else, reset it.
				boardArray[row][col] = (boardArray[row][col] != get_CheckerTeam(thisChecker)) ? boardArray[row][col] : 'B';
				boardArray[newRow][newCol] = get_CheckerTeam(thisChecker);*/
				translateAcrossBoard(currTeamsCheckers[row][col], nDim, boardArray, currTeamsCheckers);
			}
			printCheckersDisplayBoard(nDim, boardArray);
		}
	}
}

/***********************************************************************************************/
/* Function: main(int argc, char * argv[])                                                     */
/* Purpose: the main function to perform parent's piping to child sub-processes.               */
/* Parameter(s): char *argv[] - the command-line arguments for different sub-process inputs.   */
/* int argc - the number of command line arguments used.                                       */
/* Return type: int - returns 0 for completion of task (no warnings).                          */
/***********************************************************************************************/
int main(int argc, char *argv[])
{
	// printf("Start of main");
	//  Startup a seed for random generator (only do this once):
	srand(time(0));										// set to current time; varies
	intmax_t coinFlip = generateRandomNumBetween(0, 1); //<<If you ever want to use a coinflip as to who goes first instead of black rules, use this<<

	// Initialize dimension of the checker board:
	size_t dim = 8;

	// Initialize scores:
	size_t team1Score = 0;
	size_t team2Score = 0;

	// char **boardMatrix = getCheckeredBoardC90(dim); // Works

	// WORKS:
	char(*boardMatrix)[dim];
	allocateToBoardC99(dim, &boardMatrix);
	getCheckeredBoardC99(dim, boardMatrix);
	// printf("Allocated to boards");

	Checker *(*team1Checkers)[dim]; // O
	Checker *(*team2Checkers)[dim]; // X
	allocateToCheckerOverlapMatrixC99(dim, &team1Checkers);
	allocateToCheckerOverlapMatrixC99(dim, &team2Checkers);
	initCheckerTeams(dim, boardMatrix, team1Checkers, team2Checkers);
	/* --TODO: Change all ulong types in Checker.h to size_t --
	  (size_t is larger and flexible than unsigned long, as seen in https://stackoverflow.com/questions/15637228/what-is-the-downside-of-replacing-size-t-with-unsigned-long)
	*/
	// char whosFirst = whoStartsOnBottom(dim, boardMatrix);
	bool hasBlackGone = false;
	bool hasWhiteGone = false;
	bool isFirstTurn = true;
	// printf("getting ready to go in the loop");
	//  Team X will always be black, team O will always be white (X or team2 goes first always):
	//(TODO: improve while loop win condition exits to also include time, entrapment/no more moves, forfeits)
	while ((team1Score != (numOfCheckersPerRow(dim) * numOfStartingRows(dim))) && (team2Score != (numOfCheckersPerRow(dim) * numOfStartingRows(dim))))
	{
		// printf("I am running in loop");
		if (isFirstTurn || hasWhiteGone)
		{
			printCheckersDisplayBoard(dim, boardMatrix);
			//--SEG FAULT IN SELECT--
			selectAChecker(dim, team2Checkers, boardMatrix, team1Checkers, team2Score); // includes their move, their capture(s), etc.
			hasBlackGone = true;
			hasWhiteGone = false;
			isFirstTurn = false;									   // so that we can say black's first turn is done
			swapBoard(dim, team2Checkers, boardMatrix, team1Checkers); // this teams turn is done, setup for next team
		}

		if (hasBlackGone)
		{
			printCheckersDisplayBoard(dim, boardMatrix);
			selectAChecker(dim, team1Checkers, boardMatrix, team2Checkers, team1Score); // includes their move, their capture(s), etc.
			hasWhiteGone = true;
			hasBlackGone = false;
			swapBoard(dim, team1Checkers, boardMatrix, team2Checkers); // this teams turn is done, setup for next team
		}
	}

	// After move is made, you swap the board for next team's selection:

	// printf("hello world");

	/*printf("\n");

	for (int row = 0; row < dim; row++)
	{
		printf("[");
		for (int column = 0; column < dim; column++)
		{
			printf("%c ", boardMatrix[row][column]);
		}
		printf("]\n");
	}*/
	// printCheckersDisplayBoard(dim, boardMatrix); // Above printf for loop block has been put inside this function

	//===FOR TESTING STUFF:===
	/*int x = 2;
	int y = 5;
	int num = 3;

	if ((y > num) && (num > x))
	{
		printf("True");
	}
	else
	{
		printf("False");
	}

	printf("\nRandom number between 0 and 1 = %d", generateRandomNumBetween(0, 1));*/

	return 0;
}