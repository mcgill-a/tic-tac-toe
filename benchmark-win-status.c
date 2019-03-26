#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>
// Main menu options
const char * options[] = {
    "Player vs Player",
    "Change Board Size",
    "Exit",
};
#define OPTION_COUNT (sizeof (options) / sizeof (const char *))

// Clear the buffer to avoid problems with input when game is reset
#define CLEARBUF() char ch; while ((ch = getchar()) != '\n' && ch != EOF);

void printCharacter(char, int);
void displayMenuOptions(char[50]);
void changeBoardSize(int*);
void initialiseBoard(int**, int);
void unallocateBoard(int**, int);
int match(int, int);
int play(int** board, int, int, int, int, int);
void displayBoard(int**, int, int, int);
void resetBoard(int** board, int);
int updateBoard(int** board, int, int, int, int);
int checkStatus(int** board, int, int);
int checkHorizontal(int** board, int, int);
int checkVertical(int** board, int, int);
int checkDiagonal(int** board, int, int);
int playerMove(int** board, int, int*, int*, int, int, int);
int exhaustiveCheckStatus(int**, int);
int exhaustiveCheckHorizontal(int**, int);
int exhaustiveCheckVertical(int**, int);
int exhaustiveCheckDiagonal(int**, int);

int main(void)
{
    char userInput[5];
    int option = -1;
    // Default board size == 3
    int boardSize = 3;
    displayMenuOptions("");
    do
    {
        // Get the user input from the main menu
        fgets(userInput, 100, stdin);
        option = atoi(userInput);
        switch (option)
        {
            case 1:
                match(1, boardSize);
                break;
            case 2:
                changeBoardSize(&boardSize);
                char info[50] = "New board size: ";
                char boardSizeStr[5];
                strcat(info, itoa(boardSize, boardSizeStr, 10));
                displayMenuOptions(info);
                break;
            case OPTION_COUNT:
                displayMenuOptions("");
                printf("%d\n\nGame Closed.\n", OPTION_COUNT);
                exit(0);
                break;
            default:
                displayMenuOptions("");
                break;
        }
    } while (option != 7);
}

int play(int** board, int boardSize, int playerOneScore, int playerTwoScore, int startingPlayer, int mode)
{

    int currentPlayer = startingPlayer;
    int latestPosition;
    displayBoard(board, boardSize, playerOneScore, playerTwoScore);
    
    int gameOver = 0;
    int count = 0;
    int result = 0;
    
    while(!gameOver && count < boardSize*boardSize)
    {
        count++;

        latestPosition = playerMove(board, boardSize, &count, &currentPlayer,playerOneScore, playerTwoScore, mode);
        
        displayBoard(board, boardSize, playerOneScore, playerTwoScore);
        // Switch player
        currentPlayer *= (-1); 
        
        // Only check the result if it is possible to win yet
        if(boardSize == 3 && count >= 5)
        {
            result = checkStatus(board, boardSize, latestPosition);
        }
        else if(boardSize > 3 && count >= 7)
        {
            result = checkStatus(board, boardSize, latestPosition);
        }

        // BENCHMARK FOR CURRENT POSITION STARTS HERE
        clock_t tStart, tEnd;
        long double difference;
        long double selectionAverage = 0;
        for(int idx = 0; idx < 10; idx++)
        {
            tStart=clock();
            for (int i = 0; i < 100000; i++)
            {
                result = checkStatus(board, boardSize, latestPosition);
            }
            tEnd=clock();
            difference = ((long double) tEnd - (long double) tStart) / CLOCKS_PER_SEC;
            selectionAverage += difference;
        }

        long double exhaustiveAverage = 0;
        for(int idx = 0; idx < 10; idx++)
        {
            tStart=clock();
            for (int i = 0; i < 100000; i++)
            {
                result = exhaustiveCheckStatus(board, boardSize);
            }
            tEnd=clock();
            difference = ((long double) tEnd - (long double) tStart) / CLOCKS_PER_SEC;
            exhaustiveAverage += difference;
        }
        FILE *fp;
        // Append the average results to the csv file for the current board size
        char filename[20] = "3x3.csv";
        fp = fopen(filename,"a");
        fprintf(fp, "%Lf,%Lf\n", selectionAverage, exhaustiveAverage);
        fflush(fp);
        // BENCHMARK FOR CURRENT POSITION ENDS HERE
        if (result != 0)
        {
            //gameOver = true;
        }
    }
    return result;
}

void removeNewline(char *s) {
    while(*s && *s != '\n' && *s != '\r') s++;
    *s = 0;
}

void displayMenuOptions(char info[50])
{
    system("cls");
    printf("Tic Tac Toe by Alex McGill\n\n");
    printf("BENCHMARK VERSION:\n");
    printf("- Game will continue until all positions are occupied\n");
    printf("- Each position entered runs all possible checks for worst case scenario\n");
    printf("- The benchmark compares a custom selective search vs an exhaustive search\n");
    printf("- Results are exported to a csv file\n\n");
    printf("MAIN MENU:\n");
    
    int i;
    for (i=0; i < OPTION_COUNT; i++)
    {
        printf("[%d] %s\n", i+1, options[i]);
    }
    printf("\n");
    // If there was information provided to the function, display it
    if (strcmp(info, "") != 0)
    {
        printf("[INFO] %s\n\n", info);
    }
    printf("Select an option (1-%d): ", OPTION_COUNT);
}

void changeBoardSize(int *boardSize)
{
    displayMenuOptions("");
    printf("4\n\nCurrent board size: %d\n", *boardSize);
    printf("Select a new board size (3, 5, 7): ");
    char userInput[5];
    int option = -1;
    bool valid = false;
    do
    {
        fgets(userInput, 100, stdin);
        option = atoi(userInput);
        switch (option)
        {
            case 3:
            case 5:
            case 7:
                valid = true;
                *boardSize = option;
                break;
            default:
                displayMenuOptions("");
                printf("4\n\nCurrent board size: %d\n", *boardSize);
                printf("Invalid input. Select a new board size (3, 5, 7): ");
                break;
        }
    } while (!valid);
}

void initialiseBoard(int** board, int boardSize)
{
    for(int i = 0; i < boardSize; i++)
    {
        // Allocate memory for each row
        board[i] = (int *) malloc(boardSize * sizeof(int));
        // Loop through each column in the row
        for (int j = 0; j < boardSize; j++)
        {
            board[i][j] = 0;
        }
    }
}

void unallocateBoard(int **board, int boardSize)
{
    for(int i = 0; i < boardSize; i++)
    {
        // Free the memory allocated for each row
        free(board[i]);
    }
    free(board);
}

int match(int mode, int boardSize)
{
    // Allocate memory for the board 2D array
    int **board = (int **) malloc(boardSize * sizeof(int *));
    initialiseBoard(board, boardSize);
    int playerOneScore = 0;
    int playerTwoScore = 0;
    int result = 0;
    int playAgain = 0;
    srand( time(NULL) );
    int startingPlayer = rand() % 2;
    if (startingPlayer == 0)
    {
        startingPlayer = -1;
    }
    do {
        // Random player starts first game, then alternate
        result = play(board, boardSize, playerOneScore, playerTwoScore, startingPlayer, mode);
        startingPlayer *= (-1);
        
        if (result == 1)
        {
            playerOneScore += 1;
        }
        else if (result == -1)
        {
            playerTwoScore += 1;
        }
        
        char repeat = 'A';
        char userInput[5];
        int done = 0;
        do
        {
            displayBoard(board, boardSize, playerOneScore, playerTwoScore);
            if (result == 1)
            {
                printf("Player one (X) wins\n");
            }
            else if (result == -1)
            {
                printf("Player two (O) wins\n");
            }
            else if (result == 0)
            {
                printf("Game ended in a draw\n");
            }

            printf("Play again? (Y/N): ");
            fgets(userInput, 100, stdin);
            repeat = toupper(userInput[0]);
            switch (repeat)
            {
                case 'Y':
                    playAgain = 1;
                    // Reset the board
                    resetBoard(board, boardSize);
                    done = 1;
                    break;
                case 'N':
                    playAgain = 0;
                    done = 1;
                    displayMenuOptions("");
                    break;
                default:
                    continue;
            }
        } while (done == 0);
    } while(playAgain == 1);
    unallocateBoard(board, boardSize);
    return 0;
}

int updateBoard(int** board, int boardSize, int position, int value, int override)
{
    // Make positon positive if it is negative
    if (position < 0)
    {
        position *= -1;
    }

    // position / BOARD_SIZE (but rounded to nearest whole number)
    // Then subtract 1 because arrays start at 0
    int row = ((position -1) / boardSize + 1) - 1;
    // Use the row and board size to work out which column the position is in
    int column = (boardSize -1) - (((row+1) * boardSize) - position);
    
    // If undo (override) is selected, reset the current positon
    if (override == 1)
    {
        board[row][column] = 0;
        return 0;
    }
    else
    {
        // If the position is not already occupied
        if (board[row][column] == 0)
        {
            board[row][column] = value;
            return 0;
        }
        else
        {
            return -1;
        }
    }    
}

int checkStatus(int** board, int boardSize, int position)
{
    int status = 0;
    // Check for a horizontal win
    status = checkHorizontal(board, boardSize, position);
    if(status != 0)
    {
        return status;
    }
    // Check for a vertical win
    status = checkVertical(board, boardSize, position);
    if(status != 0)
    {
        return status;
    }
    // Check for a diagonal win
    status = checkDiagonal(board, boardSize, position);
    if(status != 0)
    {
        return status;
    }
    //  0 >> No winner
    // +1 >> X wins
    // -1 >> O wins
    return 0;
}

// Check for a horizontal win in line with the current position
int checkHorizontal(int** board, int boardSize, int position)
{
    int row = ((position -1) / boardSize + 1) - 1;
    int column = (boardSize -1) - (((row+1) * boardSize) - position);
    int currentGroup;
    // If board size > 3, need 4 in a row instead of 3
    int required = 3;
    if (boardSize > 3)
    {
        required = 4;
    }
    
    // Loop once for 3x3, (boardSize - 2) times for 5x5 and 7x7
    int loopAmount = boardSize - 3;
    if (boardSize == 3)
    {
        loopAmount = 1;
    }
    for (int i = 0; i < loopAmount; i++)
    {
        currentGroup = 0;
        // Sum the current group of values and check if they meet the win requirement
        for (int j = i; j < i+required; j++)
        {
            currentGroup += board[row][j];
        }
        if (currentGroup == required)
        {
            //return 1;
        }
        else if (currentGroup == (-1) * required)
        {
            //return -1;
        }
        else
        {
            currentGroup = 0;
        }
    }
    // No winner yet
    return 0;
}

// Check for a vertical win in line with the current position
int checkVertical(int** board, int boardSize, int position)
{
    int row = ((position -1) / boardSize + 1) - 1;
    int column = (boardSize -1) - (((row+1) * boardSize) - position);
    int currentGroup;
    // If board size > 3, need 4 in a row instead of 3
    int required = 3;
    if (boardSize > 3)
    {
        required = 4;
    }
    
    // Loop once for 3x3, (boardSize - 2) times for 5x5 and 7x7
    int loopAmount = boardSize - 3;
    if (boardSize == 3)
    {
        loopAmount = 1;
    }
    for (int i = 0; i < loopAmount; i++)
    {
        currentGroup = 0;
        // Sum the current group of values and check if they meet the win requirement
        for (int j = i; j < i+required; j++)
        {
            currentGroup += board[j][column];
        }
        if (currentGroup == required)
        {
            //return 1;
        }
        else if (currentGroup == (-1) * required)
        {
            //return -1;
        }
        else
        {
            currentGroup = 0;
        }
    }
    // No winner yet
    return 0;
}

// Check for a diagonal win in line with the current position
int checkDiagonal(int** board, int boardSize, int position)
{
    int row = ((position -1) / boardSize + 1) - 1;
    int column = (boardSize -1) - (((row+1) * boardSize) - position);

    // If board size > 3, need 4 in a row instead of 3
    int currentGroup, loopAmount;
    int required = 3;
    if (boardSize > 3)
    {
        required = 4;
    }
    int tempColumn = column;
    int tempRow = row;

    // For the diagonals starting top right and going down to the left:

    // Move up to the top right edge
    while (tempColumn < boardSize -1 && tempRow > 0)
    {
        tempColumn += 1;
        tempRow -= 1;
    }

    loopAmount = tempColumn - tempRow - required + 2;
    
    if (loopAmount > 0)
    {
        for(int i = 0; i < loopAmount; i++)
        {
            
            currentGroup = 0;
            // Sum the current group of values and check if they meet the win requirement
            for (int j = i; j < i+required; j++)
            {
                int currentValue = board[tempRow+j][tempColumn-j];
                if (currentValue == 1 || currentValue == -1)
                {
                    currentGroup += currentValue;
                }
            }
            if (currentGroup == required)
            {
                //return 1;
            }
            else if (currentGroup == (-1) * required)
            {
                //return -1;
            }
            else
            {
                currentGroup = 0;
            }
        }
    }

    // For the diagonals starting top left and going down to the right:
    // Get the provided column and row again to undo previous changes to temp value
    tempColumn = column;
    tempRow = row;

    // Move up to the top left edge
    while (tempColumn > 0 && tempRow > 0)
    {
        tempColumn -= 1;
        tempRow -= 1;
    }
    loopAmount = required - tempColumn - tempRow - 2;
    if (loopAmount > 0)
    {
        for(int i = 0; i < loopAmount; i++)
        {
            currentGroup = 0;
            // Sum the current group of values and check if they meet the win requirement
            for (int j = i; j < i+required; j++)
            {
                int currentValue = board[tempRow+j][tempColumn+j];
                if (currentValue == 1 || currentValue == -1)
                {
                    currentGroup += currentValue;
                }
            }
            if (currentGroup == required)
            {
                //return 1;
            }
            else if (currentGroup == (-1) * required)
            {
                //return -1;
            }
            else
            {
                currentGroup = 0;
            }
        }
    }

    // No winner yet
    return 0;
}

void resetBoard(int** board, int boardSize)
{
    for(int i = 0; i < boardSize; i++)
    {
        // Loop through each column in the row
        for (int j = 0; j < boardSize; j++)
        {
            board[i][j] = 0;
        }
    }
}

void printCharacter(char value, int line)
{
    char x[6][14] = {
        " db    db\0",
        " `8b  d8'\0",
        "  `8bd8' \0",
        "  .dPYb. \0",
        " .8P  Y8.\0",
        " YP    YP\0"
    };
    char o[6][14]= {
        "  .d88b. \0",
        " .8P  Y8.\0",
        " 88    88\0",
        " 88    88\0",
        " .8P  Y8.\0",
        "  `Y88P' \0"
    };

    if (value == 'X')
    {
        printf("%s ", x[line]);
    }
    else if (value == 'O')
    {
        printf("%s ", o[line]);
    }

}

void displayBoard(int** board, int boardSize, int playerOneScore, int playerTwoScore)
{
    //system("cls");
    if (boardSize == 3)
    {
        printf("Tic Tac Toe (%dx%d) - 3 in a row to win\n", boardSize, boardSize);
    }
    else
    {
        printf("Tic Tac Toe (%dx%d) - 4 in a row to win\n", boardSize, boardSize);
    }
    
    printf("Commands: 'undo', 'redo'\n");
    printf("SCORE: (X) %d - %d (O)\n", playerOneScore, playerTwoScore);
    // Top edge
    for (int i=0; i < boardSize; i++)
    {
        if(i == 0)
        {
            printf("o----------");
        }
        else if (i == boardSize-1)
        {
            printf("o----------o");
        }
        else
        {
            printf("o----------");
        }
        
    }
    printf("\n");
    
    // Cell content
    int counter = 0;
    for (int i=0; i < boardSize; i++)
    {
        for (int line=0; line < 6; line++)
        {
            printf("|");
            for (int j=0; j < boardSize; j++)
            {
                if(line == 3)
                {
                    counter++;
                }
                if (board[i][j] == 1)
                {
                    printCharacter('X', line);
                    
                }
                else if (board[i][j] == -1)
                {
                    printCharacter('O', line);
                }
                else
                {
                    if (line == 3)
                    {
                        if (counter < 10)
                        {
                            printf("    0%d    ", counter);
                        }
                        else
                        {
                            printf("    %d    ", counter);
                        }
                    }
                    else
                    {
                        printf("          ");
                    }
                    
                }
                printf("|");
            }
            printf("\n");
        }

        // Bottom edge
        for (int i=0; i < boardSize; i++)
        {
            if(i == 0)
            {
                printf("o----------");
            }
            else if (i == boardSize-1)
            {
                printf("o----------o");
            }
            else
            {
                printf("o----------");
            }
        }
        printf("\n");
    }
}

int playerMove(int **board, int boardSize, int *count, int *currentPlayer, int playerOneScore, int playerTwoScore, int mode)
{
    char *end;
    char buf[100];
    bool validMove = false;
    while(!validMove)
    {
        if (*currentPlayer == 1)
        {
            printf("(X) Select a position (1-%d): ", boardSize*boardSize);
        }
        else if (*currentPlayer == -1)
        {
            printf("(O) Select a position (1-%d): ", boardSize*boardSize);
        }
        do {
            if (!fgets(buf, sizeof buf, stdin))
            {
                break;
            }
            // Strip the newline character (\n)
            buf[strlen(buf) - 1] = 0;
            int input = strtol(buf, &end, 10);
            // If the user entered the number of a position on the board
            if(input > 0 && input <= (boardSize * boardSize))
            {
                int valid = updateBoard(board, boardSize, input, *currentPlayer, 0);
                if(valid == -1)
                {
                    displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                    printf("Position %d is already taken\n", input);
                    break;
                }
                else
                {
                    validMove = true;
                    int position = input;
                    return position;
                }
            }
            else
            {
                displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                printf("Invalid input\n");
                break;
            }
        }
        while (end != buf + strlen(buf));
    }
    return 0;
}

int exhaustiveCheckStatus(int** board, int boardSize)
{
    int status = 0;
    // Check for a horizontal win
    status = exhaustiveCheckHorizontal(board, boardSize);
    if(status != 0)
    {
        return status;
    }
    // Check for a vertical win
    status = exhaustiveCheckVertical(board, boardSize);
    if(status != 0)
    {
        return status;
    }
    // Check for a diagonal win
    status = exhaustiveCheckDiagonal(board, boardSize);
    if(status != 0)
    {
        return status;
    }
    //  0 >> No winner
    // +1 >> X wins
    // -1 >> O wins
    return 0;
}

int exhaustiveCheckHorizontal(int** board, int boardSize)
{
    int required = 3;
    if (boardSize > 3)
    {
        required = 4;
    }
    int currentGroup;
    // Loop through each row
    for(int i = 0; i < boardSize; i++)
    {
        // Loop through each row in the column
        int loopAmount = boardSize - 3;
        if (loopAmount < 1)
        {
            loopAmount = 1;
        }
        // Loop through each column in the row
        for (int j = 0; j < loopAmount; j++)
        {
            currentGroup = 0;
            for (int k = 0; k < required; k++)
            {
                currentGroup += board[i][j+k];
            }
            if (currentGroup == required)
            {
                // X wins
                //return 1;
            }
            else if (currentGroup == (-1) * required)
            {
                // O wins
                //return -1;
            }
        }
    }
    // No winner yet
    return 0;
}

int exhaustiveCheckVertical(int** board, int boardSize)
{
    int required = 3;
    if (boardSize > 3)
    {
        required = 4;
    }
    int currentGroup;
    // Loop through each column
    for(int i = 0; i < boardSize; i++)
    {
        // Loop through each row in the column
        int loopAmount = boardSize - 3;
        if (loopAmount < 1)
        {
            loopAmount = 1;
        }
        for (int j = 0; j < loopAmount; j++)
        {
            currentGroup = 0;
            for (int k = 0; k < required; k++)
            {
                currentGroup += board[j+k][i];
            }
            if (currentGroup == required)
            {
                // X wins
                //return 1;
            }
            else if (currentGroup == (-1) * required)
            {
                // O wins
                //return -1;
            }
        }
    }
    // No winner yet
    return 0;
}

int exhaustiveCheckDiagonal(int** board, int boardSize)
{
    int level = 0;
    // Loop through the possible right diagonal groups of three
    for (int i = 0; i < boardSize - 2; i++)
    {
        int currentGroup = 0;
        currentGroup += board[i][level];
        currentGroup += board[i+1][level+1];
        currentGroup += board[i+2][level+2];
        if (currentGroup == 3)
        {
            // X wins
            //return 1;
        }
        else if (currentGroup == -3)
        {
            // O wins
            //return -1;
        }
        level++;
    }

    level = boardSize - 1;
    // Loop through the possible left diagonal groups of three
    for (int i = 0; i < boardSize - 2; i++)
    {
        int currentGroup = 0;
        currentGroup += board[i][level];
        currentGroup += board[i+1][level-1];
        currentGroup += board[i+2][level-2];
        if (currentGroup == 3)
        {
            // X wins
            //return 1;
        }
        else if (currentGroup == -3)
        {
            // O wins
            //return -1;
        }
        level--;
    }

    // No winner yet
    return 0;
}