#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 3

int play(int board[BOARD_SIZE][BOARD_SIZE]);
void displayBoard(int[BOARD_SIZE][BOARD_SIZE]);
int updateBoard(int board[BOARD_SIZE][BOARD_SIZE], int, int);
int checkStatus(int board[BOARD_SIZE][BOARD_SIZE]);
int checkHorizontal(int board[BOARD_SIZE][BOARD_SIZE]);
int checkVertical(int board[BOARD_SIZE][BOARD_SIZE]);
int checkDiagonal(int board[BOARD_SIZE][BOARD_SIZE]);

int main(void)
{
    int board[BOARD_SIZE][BOARD_SIZE] = {0}; 
    int result = play(board);
    return 0;
}

int play(int board[BOARD_SIZE][BOARD_SIZE])
{
    displayBoard(board);
    int gameOver = 0;
    int count = 0;
    int currentPlayer = 0;
    while(gameOver == 0)
    {
        // If there are no possible moves left
        if (count == BOARD_SIZE * BOARD_SIZE)
        {
            // Game ends in a draw
            return -1;
        }

        if(count % 2 == 0)
        {
            printf("(X) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
            currentPlayer = 2;
        }
        else
        {
            printf("(O) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
            currentPlayer = -2;
        }
        
        char *end;
        char buf[5];

        do {
            if (!fgets(buf, sizeof buf, stdin))
            {
                break;
            }

            // Strip the newline character (\n)
            buf[strlen(buf) - 1] = 0;

            int input = strtol(buf, &end, 10);
            if(input > 0 && input <= (BOARD_SIZE * BOARD_SIZE))
            {
                int valid = updateBoard(board, input, currentPlayer);
                if(valid != -1)
                {
                    displayBoard(board);
                    count++;
                    gameOver = checkStatus(board);
                    if (gameOver == 2)
                    {
                        printf("Player one (X) wins");
                        return gameOver;
                    }
                    else if (gameOver == -2)
                    {
                        printf("Player two (O) wins");
                        return gameOver;
                    }
                }
                else
                {
                    printf("Position %d is already taken\n", input);
                }
            }
            else
            {
                printf("Invalid input\n");
            }
        }
        while (end != buf + strlen(buf));
    }

    return -1;
}

int updateBoard(int board[BOARD_SIZE][BOARD_SIZE], int position, int value)
{
    // position / BOARD_SIZE (rounded to nearest whole number)
    // Subtract 1 because arrays start at 0
    int row = ((position -1) / BOARD_SIZE + 1) - 1;
    // Use the row and board size to work out which column the position is in
    int column = (BOARD_SIZE -1) - (((row+1) * BOARD_SIZE) - position);
    
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

int checkStatus(int board[BOARD_SIZE][BOARD_SIZE])
{
    int status = 0;

    // Check for a horizontal win
    status = checkHorizontal(board);
    if(status != 0)
    {
        return status;
    }

    status = checkVertical(board);
    if(status != 0)
    {
        return status;
    }

    status = checkDiagonal(board);
    if(status != 0)
    {
        return status;
    }

    //  0 >> No winner
    // +2 >> X wins
    // -2 >> O wins
    return 0;
}

int checkHorizontal(int board[BOARD_SIZE][BOARD_SIZE])
{
    int rowTotal;
    // Loop through each row
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        rowTotal = 0;
        // Loop through each column in the row
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            rowTotal += board[i][j];
        }
        if (rowTotal == BOARD_SIZE * 2)
        {
            // X wins
            return 2;
        }
        else if (rowTotal == BOARD_SIZE * (-2))
        {
            // O wins
            return -2;
        }
    }
    // No winner yet
    return 0;
}

int checkVertical(int board[BOARD_SIZE][BOARD_SIZE])
{
    int columnTotal;
    // Loop through each column
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        columnTotal = 0;
        // Loop through each row in the column
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            columnTotal += board[j][i];
        }
        if (columnTotal == BOARD_SIZE * 2)
        {
            // X wins
            return 2;
        }
        else if (columnTotal == BOARD_SIZE * (-2))
        {
            // O wins
            return -2;
        }
    }
    // No winner yet
    return 0;
}

int checkDiagonal(int board[BOARD_SIZE][BOARD_SIZE])
{
    int rightDiagonal = 0, leftDiagonal = 0;

    /*
        Right Diagonal:
        (3x3) >> Positions 1, 5, 9 || 0, 4, 8
        Therefore, skip amount >> BOARD_SIZE + 1
    */

    // Loop through the right diagonal values
    for(int i = 0; i < BOARD_SIZE * BOARD_SIZE; i += (BOARD_SIZE + 1))
    {
        int row = ((i - 1) / BOARD_SIZE + 1) - 1;
        int column = (BOARD_SIZE -1) - (((row+1) * BOARD_SIZE) - (i+1));
        rightDiagonal += board[row][column];       
    }

    if (rightDiagonal == BOARD_SIZE * 2)
    {
        // X wins
        return 2;
    }
    else if (rightDiagonal == BOARD_SIZE * (-2))
    {
        // O wins
        return -2;
    }

    /*
        Left Diagonal:
        (3x3) >> Positions 3, 5, 7 || 2, 4, 6
        Therefore, skip amount >> BOARD_SIZE - 1
    */

   // Loop through the right diagonal values
    for(int i = BOARD_SIZE-1; i < BOARD_SIZE * BOARD_SIZE; i += (BOARD_SIZE - 1))
    {
        int row = ((i - 1) / BOARD_SIZE + 1) - 1;
        int column = (BOARD_SIZE -1) - (((row+1) * BOARD_SIZE) - (i+1));
        leftDiagonal += board[row][column];       
    }

    if (leftDiagonal == BOARD_SIZE * 2)
    {
        // X wins
        return 2;
    }
    else if (leftDiagonal == BOARD_SIZE * (-2))
    {
        // O wins
        return -2;
    }

    // No winner yet
    return 0;
}

void displayBoard(int board[BOARD_SIZE][BOARD_SIZE])
{
    system("cls");
    printf("Tic Tac Toe (%dx%d)\n", BOARD_SIZE, BOARD_SIZE);

    // Top edge
    for (int i=0; i < BOARD_SIZE; i++)
    {
        printf("=======");
    }
    printf("\n");

    int counter = 0;
    for (int idx=0; idx < BOARD_SIZE; idx++)
    {
        // Spacer 1
        for (int i=0; i < BOARD_SIZE; i++)
        {
            printf("|     |");
        }
        printf("\n");

        // Block value
        for (int i=0; i < BOARD_SIZE; i++)
        {
            counter++;
            if (board[idx][i] == 0)
            {
                //printf("|     |");
                printf("|  %d  |", counter);
            }
            else if (board[idx][i] == 2)
            {
                printf("|  X  |", board[idx][i]);
            }
            else if (board[idx][i] == -2)
            {
                printf("|  O  |", board[idx][i]);
            }
        }
        printf("\n");

        // Spacer 2
        for (int i=0; i < BOARD_SIZE; i++)
        {
            printf("|     |");
        }
        printf("\n");
        
        // Bottom edge
        for (int i=0; i < BOARD_SIZE; i++)
        {
            printf("=======");
        }
        printf("\n");
    }
}