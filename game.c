#include <stdio.h>

#define BOARD_SIZE 3

void displayBoard(int[BOARD_SIZE][BOARD_SIZE]);

int main(void)
{
    printf("Tic Tac Toe (%dx%d)\n", BOARD_SIZE, BOARD_SIZE);
    
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    displayBoard(board);
    return 0;
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
            printf("|  %d  |", board[idx][i]);
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