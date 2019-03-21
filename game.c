#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

const char * options[] = {
    "Player vs Player",
    "Player vs Computer",
    "Computer vs Computer",
    "Change Board Size (N/A)",
    "Match History",
    "Replay Match",
    "Exit",
};

#define OPTION_COUNT (sizeof (options) / sizeof (const char *))
#define BOARD_SIZE 3
// Clear the buffer to avoid problems with input when game is reset
#define CLEARBUF() char ch; while ((ch = getchar()) != '\n' && ch != EOF);

typedef struct
{
    char date[50];
    int mode;
    int result;
    int moves[1000];
    int moveCount;
} Result;

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
void storeResults(struct stack*, int, int);
int fileExists(const char*);
void loadResults(int*, Result*);
void processResults(int*, Result*);
void removeNewline(char*);
void replayMatch(Result, int);
void replayMatchDisplay(int board[BOARD_SIZE][BOARD_SIZE], int);
void getMatchNumber();
void computerVsComputer();
void playerVsComputer();
void playerMove(int board[BOARD_SIZE][BOARD_SIZE], bool);
void computerMove(int board[BOARD_SIZE][BOARD_SIZE], int);

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
                playerVsPlayer();
                break;
            case 2:
                displayMenuOptions();
                playerVsComputer();
                break;
            case 3:
                computerVsComputer();
                break;
            case 4:
                printf("Selected 4\n");
                break;
            case 5:
                displayMenuOptions();
                int count = 0;
                Result* results = (Result*) malloc(sizeof(Result)  * 100);
                loadResults(&count, results);
                processResults(&count, results);
                free(results); 
                break;
            case 6:
                displayMenuOptions();
                getMatchNumber();
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

    return 0;
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

int playerVsPlayer()
{
    int board[BOARD_SIZE][BOARD_SIZE] = {0}; 
    int playerOneScore = 0;
    int playerTwoScore = 0;
    int result = 0;
    int playAgain = 0;
    srand( time(NULL) );
    int startingPlayer = rand() % 2;
    // A stack for storing player moves
    struct stack moveStack;
    struct stack redoStack;
    init_stack(&moveStack);
    init_stack(&redoStack);

    do {
        // Random player starts first game, then alternate
        result = play(board, playerOneScore, playerTwoScore, startingPlayer, &moveStack, &redoStack);
        // Write results to file 
        storeResults(&moveStack, result, 1);
        startingPlayer++;
        
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
            displayBoard(board, playerOneScore, playerTwoScore, 0);
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

            printf("Play again? Y/N\n");
            fgets(userInput, 100, stdin);
            repeat = toupper(userInput[0]);
            switch (repeat)
            {
                case 'Y':
                    playAgain = 1;
                    // Reset the board
                    resetBoard(board);
                    fflush(stdin);
                    CLEARBUF()
                    done = 1;
                    break;
                case 'N':
                    playAgain = 0;
                    done = 1;
                    displayMenuOptions();
                    break;
                default:
                    continue;
            }
        } while (done == 0);
    } while(playAgain == 1);
    
    return 0;
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
            return 0;
        }
        if (count == 0)
        {
            if (startingPlayer % 1 == 0)
            {
                printf("(X) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
                currentPlayer = 1;
            }
            else
            {
                printf("(O) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
                currentPlayer = -1;
            }
            
        }
        else if(currentPlayer == 1)
        {
            printf("(X) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
        }
        else
        {
            printf("(O) Enter a position 1-%d\n", BOARD_SIZE*BOARD_SIZE);
        }
        
        char *end;
        char buf[100];

        do {
            if (!fgets(buf, sizeof buf, stdin))
            {
                break;
            }

            // Strip the newline character (\n)
            buf[strlen(buf) - 1] = 0;
            if (strcmp(buf, "undo") == 0)
            {
                int *popped = NULL;
                popped = pop(moveStack);
                if (popped != NULL)
                {
                    push(redoStack, *popped);
                    count--;
                    currentPlayer *= -1;
                    updateBoard(board, *popped, currentPlayer, 1);
                    displayBoard(board, playerOneScore, playerTwoScore, 0);
                    break;
                }
                else
                {
                    displayBoard(board, playerOneScore, playerTwoScore, 0);
                    printf("Nothing to undo\n");
                    break;
                }
            }
            else if (strcmp(buf, "redo") == 0)
            {
                int *popped = NULL;
                popped = pop(redoStack);
                if (popped != NULL)
                {
                    push(moveStack, *popped);
                    count++;
                    updateBoard(board, *popped, currentPlayer, 0);
                    displayBoard(board, playerOneScore, playerTwoScore, 0);
                    currentPlayer *= -1;
                    break;
                }
                else
                {
                    displayBoard(board, playerOneScore, playerTwoScore, 0);
                    printf("Nothing to redo\n");
                    break;
                }
            }
            else
            {
                int input = strtol(buf, &end, 10);
                if(input > 0 && input <= (BOARD_SIZE * BOARD_SIZE))
                {
                    int valid = updateBoard(board, input, currentPlayer, 0);
                    if(valid != -1)
                    {
                        // Clear the redo stack after a new move otherwise problems will occur
                        pop_all(redoStack);
                        if (currentPlayer == 1)
                        {
                            push(moveStack, input);
                            currentPlayer = -1;
                        }
                        else if (currentPlayer == -1)
                        {
                            push(moveStack, (-1) * input);
                            currentPlayer = 1;
                        }
                        displayBoard(board, playerOneScore, playerTwoScore, 0);
                        count++;
                        gameOver = checkStatus(board);
                        if (gameOver == 1)
                        {
                            return gameOver;
                        }
                        else if (gameOver == -1)
                        {
                            return gameOver;
                        }
                    }
                    else
                    {
                        displayBoard(board, playerOneScore, playerTwoScore, 0);
                        printf("Position %d is already taken\n", input);
                        break;
                    }
                }
                else
                {
                    displayBoard(board, playerOneScore, playerTwoScore, 0);
                    printf("Invalid input\n");
                    break;
                }
            }
        }
        while (end != buf + strlen(buf));
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
    // +1 >> X wins
    // -1 >> O wins
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
        if (rowTotal == BOARD_SIZE)
        {
            // X wins
            return 1;
        }
        else if (rowTotal == BOARD_SIZE * (-1))
        {
            // O wins
            return -1;
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
        if (columnTotal == BOARD_SIZE)
        {
            // X wins
            return 1;
        }
        else if (columnTotal == BOARD_SIZE * (-1))
        {
            // O wins
            return -1;
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

    if (rightDiagonal == BOARD_SIZE)
    {
        // X wins
        return 1;
    }
    else if (rightDiagonal == BOARD_SIZE * (-1))
    {
        // O wins
        return -1;
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

    if (leftDiagonal == BOARD_SIZE)
    {
        // X wins
        return 1;
    }
    else if (leftDiagonal == BOARD_SIZE * (-1))
    {
        // O wins
        return -1;
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
            else if (board[i][j] == 1)
            {
                printf("|  X  |", board[i][j]);
            }
            else if (board[i][j] == -1)
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

void storeResults(struct stack *moveStack, int result, int mode)
{  
    time_t timer;
    char currentDateTime[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(currentDateTime, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    int moves[BOARD_SIZE*BOARD_SIZE] = {0};

    int *i = NULL;
    i = pop(moveStack);
    int count = 0;

    FILE *output = NULL;
    char outputName[50] = "tic-tac-toe_results.csv";
    output = fopen(outputName, "a");
    fprintf(output, "%s,%d,%d,", currentDateTime, mode, result);
    if(i != NULL)
    {
        char space = ' ';
        char newline = '\n';
        do
        {
            fprintf(output, "%d", *i);
            i = pop(moveStack);
            if (i != NULL)
            {
                
                fprintf(output, "%c",space);
            }
            else
            {
                fprintf(output, "%c", newline);
            }
        }
        while(i != NULL);
    }
    fclose(output);
}

int fileExists(const char *fileName)
{
    FILE *file;
    if ((file = fopen(fileName, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void loadResults(int *count, Result* results)
{
    char fileName[50] = "tic-tac-toe_results.csv";
    // Make sure the file exists
    if (fileExists(fileName) != 1)
    {
        return;
    }

    FILE* resultsFile = fopen(fileName, "r");
    char row[1024];

    while (fgets(row, 1024, resultsFile))
    {
        removeNewline(row);
        char *p = strtok (strdup(row), ",");
        char *array[4];
        int i = 0;
        // Split the current row into the char array
        while (p != NULL)
        {
            array[i++] = p;
            p = strtok (NULL, ",");
        }       
        
        char *charMoves = strtok (strdup(array[3]), " ");
        int intMoves[BOARD_SIZE * BOARD_SIZE];
        // Split array[3] by " "
        // Convert each value to int
        // Add move to int array
        i = 0;
        while (charMoves != NULL)
        {
            intMoves[i++] = atoi(charMoves);
            charMoves = strtok (NULL, " ");
        }

        Result temp;
        strcpy(temp.date, array[0]);
        temp.mode = atoi(array[1]);
        temp.result = atoi(array[2]);
        memcpy(temp.moves, intMoves, sizeof(temp.moves)); 
        temp.moveCount = i;
        results[*count] = temp;
        *count += 1;
    }
}

void processResults(int *count, Result* results)
{
    int playerOneWins = 0, playerTwoWins = 0;
    int gamesPlayed = *count, gamesTied = 0;
    double playerOneWinPercentage = 0, playerTwoWinpercentage = 0;

    for (int i = 0; i < *count; i++)
    {
        if (results[i].result == 1)
        {
            playerOneWins++;
        }
        else if (results[i].result == -1)
        {
            playerTwoWins++;
        }
        else if (results[i].result == 0)
        {
            gamesTied++;
        }
    }
    if (playerOneWins != 0)
    {
        playerOneWinPercentage = (float)playerOneWins / gamesPlayed * 100.0;
    }
    
    if (playerTwoWins != 0)
    {
        playerTwoWinpercentage = (float)playerTwoWins / gamesPlayed * 100.0;
    }
    if (gamesPlayed == 0)
    {
        printf("No matches found.\n");
    }
    else
    {
        printf("Previous Matches:\n\n");
        printf("Games Played: %d | Games Tied: %d\n", gamesPlayed, gamesTied);
        printf("X Wins: %d (%.0f%%) | O Wins: %d (%.0f%%)\n\n", playerOneWins, playerOneWinPercentage, playerTwoWins, playerTwoWinpercentage);
        
        // Print previous matches
        printf("# |         DATE        | MODE | RESULT | MOVES\n");
        for (int i = 0; i < *count; i++)
        {
            // NUMBER AND DATE
            printf("%d | %s | ", i+1, results[i].date);
            
            // MODE
            if (results[i].mode == 1)
            {
                printf("PVP  | ");
            }
            else if (results[i].mode == 2)
            {
                printf("PVE  | ");
            }

            // RESULT
            if (results[i].result == 1)
            {
                printf("X WINS | ");
            }
            else if (results[i].result == -1)
            {
                printf("O WINS | ");
            }
            else if (results[i].result == 0)
            {
                printf(" TIED  | ");
            }

            // MOVES
            for (int j = results[i].moveCount - 1; j >= 0; j--)
            {
                if (results[i].moves[j] < 0) 
                {
                    printf("O%d ", (results[i].moves[j] * -1));
                }
                else
                {
                    printf("X%d ", results[i].moves[j]);
                }
            }
            printf("\n");
        }
    }
    printf("\n");
}

void getMatchNumber()
{
    int count = 0;
    Result* results = (Result*) malloc(sizeof(Result)  * 100);
    loadResults(&count, results);
    if (count == 0)
    {
        printf("No matches found\n");
    }
    else
    {
        char userInput[5];
        int option = -1;
        
        do
        {
            displayMenuOptions();
            printf("Please enter a previous match number to replay (1-%d):\n", count);
            fgets(userInput, 100, stdin);
            option = atoi(userInput);
        } while (option < 1 || option > count);
        replayMatch(results[option-1], option);
        free(results);
    }
}

void replayMatch(Result result, int matchNumber)
{
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    int currentPlayer = 0;
    for (int i = result.moveCount - 1; i >= 0; i--)
    {
        if (result.moves[i] < 0)
        {
            currentPlayer = -1;
        }
        else
        {
            currentPlayer = 1;
        }
        replayMatchDisplay(board, matchNumber);
        Sleep(1500);
        updateBoard(board, result.moves[i], currentPlayer, 0);
    }
    replayMatchDisplay(board, matchNumber);
    printf("Match Replay Completed.\n");
    Sleep(1500);
    displayMenuOptions();
}

void replayMatchDisplay(int board[BOARD_SIZE][BOARD_SIZE], int matchNumber)
{
    displayMenuOptions();
    printf("REPLAY OF MATCH #%d\n", matchNumber);
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
                printf("|     |");
            }
            else if (board[i][j] == 1)
            {
                printf("|  X  |", board[i][j]);
            }
            else if (board[i][j] == -1)
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

void computerVsComputer()
{
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    bool playerTurn = false;
    int count = 0;
    bool gameOver = false;
    int result = 0;
    displayBoard(board, 0, 1, 0);
    while(!gameOver && count < BOARD_SIZE*BOARD_SIZE)
    {
        Sleep(1000);
        count++;
        // Display board
        if (playerTurn)
        {
            printf("Player O Turn\n");
            computerMove(board, 1);
        }
        else
        {
            printf("Player X Turn\n");
            computerMove(board, -1);
        }
        
        displayBoard(board, 0, 0, 0);
        playerTurn = !playerTurn;
        result = checkStatus(board);
        if (result != 0)
        {
            gameOver = true;
        }
    }
    if (result == -1)
    {
        printf("X Wins\n");
    }
    else if (result == 1)
    {
        printf("O Wins\n");
    }
    else
    {
        printf("Match Tied\n");
    }
}

void playerVsComputer()
{
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
    bool playerTurn = true;
    int count = 0;
    bool gameOver = false;
    int result = 0;
    displayBoard(board, 0, 0, 1);
    while(!gameOver && count < BOARD_SIZE*BOARD_SIZE)
    {
        count++;
        // Display board
        if (playerTurn)
        {
            printf("Player Turn\n");
            if(count == 1)
            {
                playerMove(board, true);
            }
            else
            {
                playerMove(board, false);
            }
        }
        else
        {
            printf("Computer Turn\n");
            Sleep(1000);
            computerMove(board, -1);
        }
        displayBoard(board, 0, 0, 0);
        playerTurn = !playerTurn;
        
        result = checkStatus(board);
        if (result != 0)
        {
            gameOver = true;
        }
    }
    if (result == -1)
    {
        displayBoard(board, 0, 1, 0);
        printf("Computer (O) wins!\n");
    }
    else if (result == 1)
    {
        displayBoard(board, 1, 0, 0);
        printf("Player One (X) wins!\n");
    }
    else
    {
        printf("Match Tied\n");
    }
}

void playerMove(int board[BOARD_SIZE][BOARD_SIZE], bool firstMove)
{
    char *end;
    char buf[100];
    bool validMove = false;
    while(!validMove)
    {
        do {
            if (!fgets(buf, sizeof buf, stdin))
            {
                break;
            }

            // Strip the newline character (\n)
            buf[strlen(buf) - 1] = 0;
            if (strcmp(buf, "undo") == 0)
            {
                // undo
            }
            else if (strcmp(buf, "redo") == 0)
            {
                // redo
            }
            else
            {
                int input = strtol(buf, &end, 10);
                if(input > 0 && input <= (BOARD_SIZE * BOARD_SIZE))
                {
                    int valid = updateBoard(board, input, 1, 0);
                    if(valid == -1)
                    {
                        if (firstMove)
                        {
                            displayBoard(board, 0, 0, 1);
                        }
                        else
                        {
                            displayBoard(board, 0, 0, 0);
                        }
                        printf("Position %d is already taken\n", input);
                        break;
                    }
                    else
                    {
                        validMove = true;
                        break;
                    }
                }
                else
                {
                    if (firstMove)
                    {
                        displayBoard(board, 0, 0, 1);
                    }
                    else
                    {
                        displayBoard(board, 0, 0, 0);
                    }
                    printf("Invalid input\n");
                    break;
                }
            }
        }
        while (end != buf + strlen(buf));
    }
}

int minimax(int board[BOARD_SIZE][BOARD_SIZE], int player)
{
    // Check the position for the current player on the board
    int winner = checkStatus(board);
    if (winner != 0)
    {
        return winner*player;
    }

    int row = -1;
    // Initialise score with -2 (a losing move is better than no move)
    int score = -2;
    int i, j;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if(board[i][j] == 0)
            {
                board[i][j] = player;
                // Check the resulting score of the current move
                int currentScore = -minimax(board, player*-1);
                // Select the worst score for the opponent
                if(currentScore > score)
                {
                    score = currentScore;
                    row = i;
                }
                // Reset the current move on the board
                board[i][j] = 0;
            }
        }
    }
    if(row != -1)
    {
        return score;
    }
    else
    {
        return 0;
    }
}

void computerMove(int board[BOARD_SIZE][BOARD_SIZE], int value)
{
    int row = -1;
    int col = -1;
    int score = -2;
    int i, j;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        for (j = 0; j < BOARD_SIZE; j++)
        {
            if(board[i][j] == 0)
            {
                board[i][j] = value;
                int currentScore = -minimax(board, value * (-1));
                board[i][j] = 0;
                if(currentScore > score) {
                    score = currentScore;
                    row = i;
                    col = j;
                }
            }
        }
    }
    // Select the position based on the minimax tree result
    board[row][col] = value;
}