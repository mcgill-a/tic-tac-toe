#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 3

void displayBoard(int[BOARD_SIZE][BOARD_SIZE]);
int updateBoard(int board[BOARD_SIZE][BOARD_SIZE], int, int);

int main(void)
{
    printf("Tic Tac Toe (%dx%d)\n", BOARD_SIZE, BOARD_SIZE);
    
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    displayBoard(board);

    int gameOver = -1;
    int count = 0;
    int currentPlayer = 0;
    while(gameOver == -1)
    {
        if(count % 2 == 0)
        {
            printf("(X) Enter a position 1-9\n");
            currentPlayer = 1;
        }
        else
        {
            printf("(O) Enter a position 1-9\n");
            currentPlayer = 2;
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
                    count += 1;
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

    return 0;
}

int updateBoard(int board[BOARD_SIZE][BOARD_SIZE], int position, int value)
{
    return 0;

    // return -1 if position is already taken
}

void displayBoard(int board[BOARD_SIZE][BOARD_SIZE])
{
    // Top edge
    for (int i=0; i < BOARD_SIZE; i++)
    {
        printf("=======");
    }
    printf("\n");

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
            if (board[idx][i] == 0)
            {
                printf("|     |", board[idx][i]);
            }
            else if (board[idx][i] == 1)
            {
                printf("|  X  |", board[idx][i]);
            }
            else if (board[idx][i] == 2)
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

/*
    STATIC 3x3 BOARD:
    printf("=======================\n");
    printf("||     ||     ||     ||\n");
    printf("||  %d  ||  %d  ||  %d  ||\n", board[0][0], board[0][1], board[0][2]);
    printf("||     ||     ||     ||\n");
    printf("=======================\n");
    printf("||     ||     ||     ||\n");
    printf("||  %d  ||  %d  ||  %d  ||\n", board[1][0], board[1][1], board[1][2]);
    printf("||     ||     ||     ||\n");
    printf("=======================\n");
    printf("||     ||     ||     ||\n");
    printf("||  %d  ||  %d  ||  %d  ||\n", board[2][0], board[2][1], board[2][2]);
    printf("||     ||     ||     ||\n");
    printf("=======================\n");

*/