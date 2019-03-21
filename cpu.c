#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

const char * options[] = {
    "Player vs Computer",
    "Exit",
};

#define OPTION_COUNT (sizeof (options) / sizeof (const char *))
#define BOARD_SIZE 3
// Clear the buffer to avoid problems with input when game is reset
#define CLEARBUF() char ch; while ((ch = getchar()) != '\n' && ch != EOF);

void interact();
void displayMenuOptions();

int play(int board[BOARD_SIZE][BOARD_SIZE], int, int, int, struct stack*, struct stack*);
void displayBoard(int[BOARD_SIZE][BOARD_SIZE], int, int, int);
void resetBoard(int board[BOARD_SIZE][BOARD_SIZE]);
int updateBoard(int board[BOARD_SIZE][BOARD_SIZE], int, int, int);
int checkStatus(int board[BOARD_SIZE][BOARD_SIZE]);
int checkHorizontal(int board[BOARD_SIZE][BOARD_SIZE]);
int checkVertical(int board[BOARD_SIZE][BOARD_SIZE]);
int checkDiagonal(int board[BOARD_SIZE][BOARD_SIZE]);
void push(struct stack*, int);
void *pop(struct stack*);
void *pop_all(struct stack*);
void removeNewline(char*);
int* availablePositions(int board[BOARD_SIZE][BOARD_SIZE]);
void playerVsComputer();

struct stack
{
    int moves[BOARD_SIZE*BOARD_SIZE];
    int top;
};

void init_stack(struct stack *s)
{
    s->top = -1;
}

int main(void)
{
    char userInput[5];
    int option = -1;
    
    displayMenuOptions();
    do
    {
        fgets(userInput, 100, stdin);
        option = atoi(userInput);
        switch (option)
        {
            case 1:
                playerVsComputer();
                break;
            case OPTION_COUNT:
                displayMenuOptions();
                printf("Selected Exit\n");
                exit(0);
                break;
            default:
                displayMenuOptions();
                printf("Please enter a value between 1-%d:\n", OPTION_COUNT);
                break;
        }
    } while (option != 7);
}

void removeNewline(char *s) {
    while(*s && *s != '\n' && *s != '\r') s++;
    *s = 0;
}

void displayMenuOptions()
{
    system("cls");
    printf("Tic Tac Toe by Alex McGill\n\n");
    printf("MAIN MENU:\n"); 
    
    int i;
    for (i=0; i < OPTION_COUNT; i++)
    {
        printf("[%d] %s\n", i+1, options[i]);
    }
    printf("\n");
}

int* availablePositions(int board[BOARD_SIZE][BOARD_SIZE])
{
    int *available = calloc(BOARD_SIZE*BOARD_SIZE, sizeof *available);
    int i, j, count = 0;
    for (i=0; i < BOARD_SIZE; i++)
    {
        for (j=0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] == 0)
            {
                available[count] = (i*3) + (j+1);;
                count++;
            }
        }
    }
    return available;
}

void playerVsComputer()
{
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    bool playerTurn = true;
    int count = 0;
    while(checkStatus == 0 && count < BOARD_SIZE*BOARD_SIZE)
    {
        // Display board
        if (playerTurn)
        {
            // playerMove(board);
        }
        else
        {
            // computerMove(board)
        }
        playerTurn = !playerTurn;
    }
    return 0; // return show
}

int play(int board[BOARD_SIZE][BOARD_SIZE], int playerOneScore, int playerTwoScore, int startingPlayer, struct stack *moveStack, struct stack *redoStack)
{
    displayBoard(board, playerOneScore, playerTwoScore, 1);
    int gameOver = 0;
    int count = 0;
    int currentPlayer = 0;
    
    while(gameOver == 0)
    {
        // If there are no possible moves left
        if (count == BOARD_SIZE * BOARD_SIZE)
        {
            // Game ends in a draw
            printf("Game ended in a draw\n");
            return -1;
        }
        if (count == 0)
        {
            if (startingPlayer % 2 == 0)
            {
                printf("(X) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
                currentPlayer = 2;
            }
            else
            {
                printf("(O) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
                currentPlayer = -2;
            }
            
        }
        else if(currentPlayer == 2)
        {
            printf("(X) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
        }
        else
        {
            printf("(O) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
        }
    }
    return -1;
}

int updateBoard(int board[BOARD_SIZE][BOARD_SIZE], int position, int value, int override)
{
    // Make positon positive if it is negative
    if (position < 0)
    {
        position *= -1;
    }

    // position / BOARD_SIZE (rounded to nearest whole number)
    // Subtract 1 because arrays start at 0
    int row = ((position -1) / BOARD_SIZE + 1) - 1;
    // Use the row and board size to work out which column the position is in
    int column = (BOARD_SIZE -1) - (((row+1) * BOARD_SIZE) - position);
    
    // If undo (override) is selected:
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

int checkStatus(int board[BOARD_SIZE][BOARD_SIZE])
{
    int status = 0;
    // Check for a horizontal win
    status = checkHorizontal(board);
    if(status != 0)
    {
        return status;
    }
    // Check for a vertical win
    status = checkVertical(board);
    if(status != 0)
    {
        return status;
    }
    // Check for a diagonal win
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
    int rightDiagonal = 0, leftDiagonal = 0, row = 0, column = 0;

    /*
        Right Diagonal:
        (3x3) >> Positions 1, 5, 9 || 0, 4, 8
        Therefore, skip amount >> BOARD_SIZE + 1
    */

    // Loop through the right diagonal values
    for(int i = 0; i < BOARD_SIZE * BOARD_SIZE; i += (BOARD_SIZE + 1))
    {
        row = ((i - 1) / BOARD_SIZE + 1) - 1;
        column = (BOARD_SIZE -1) - (((row+1) * BOARD_SIZE) - (i+1));
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
    for(int i = BOARD_SIZE-1; i < BOARD_SIZE * BOARD_SIZE - (BOARD_SIZE-1); i += (BOARD_SIZE - 1))
    {
        row = ((i - 1) / BOARD_SIZE + 1) - 1;
        column = (BOARD_SIZE -1) - (((row+1) * BOARD_SIZE) - (i+1));
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

void resetBoard(int board[BOARD_SIZE][BOARD_SIZE])
{
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        // Loop through each column in the row
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board[i][j] = 0;
        }
    }
}

void displayBoard(int board[BOARD_SIZE][BOARD_SIZE], int playerOneScore, int playerTwoScore, int showPositions)
{
    system("cls");
    printf("Tic Tac Toe (%dx%d)\n", BOARD_SIZE, BOARD_SIZE);
    printf("SCORE: (X) %d - %d (O)\n", playerOneScore, playerTwoScore);
    
    // Top edge
    for (int i=0; i < BOARD_SIZE; i++)
    {
        printf("=======");
    }
    printf("\n");

    int counter = 0;
    for (int i=0; i < BOARD_SIZE; i++)
    {
        // Spacer 1
        for (int j=0; j < BOARD_SIZE; j++)
        {
            printf("|     |");
        }
        printf("\n");

        // Block value
        for (int j=0; j < BOARD_SIZE; j++)
        {
            counter++;
            if (board[i][j] == 0)
            {
                // Switch between showing number positons on the board
                if (showPositions == 0)
                {
                    printf("|     |");
                }
                else 
                {
                    printf("|  %d  |", counter);
                }
                
            }
            else if (board[i][j] == 2)
            {
                printf("|  X  |", board[i][j]);
            }
            else if (board[i][j] == -2)
            {
                printf("|  O  |", board[i][j]);
            }
        }
        printf("\n");

        // Spacer 2
        for (int j=0; j < BOARD_SIZE; j++)
        {
            printf("|     |");
        }
        printf("\n");
        
        // Bottom edge
        for (int j=0; j < BOARD_SIZE; j++)
        {
            printf("=======");
        }
        printf("\n");
    }
}

void push(struct stack *s, int item)
{
    if (s->top == (BOARD_SIZE*BOARD_SIZE)-1)
    {
        printf("Stack is full. Couldn't push %d onto stack\n", item);
        return;
    }
    s->top++;
    s->moves[s->top] = item;
    //printf("Push: %d\n", item);
}

void *pop(struct stack *s)
{
    int *data;
    if (s->top == -1)
    {
        //printf("Stack is empty\n");
        return NULL;
    }
    data = &s->moves[s->top];
    s->top--;
    //printf("Pop: %p\n", data);
    return data;
}

void *pop_all(struct stack *s)
{
    int *data;
    if (s->top == -1)
    {
        return NULL;
    }
    while (s->top != -1)
    {
        data = &s->moves[s->top];
        s->top--;
    }
    
    return NULL;
}