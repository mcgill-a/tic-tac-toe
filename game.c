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
    "Change Board Size",
    "Match History",
    "Replay Match",
    "Exit",
};

#define OPTION_COUNT (sizeof (options) / sizeof (const char *))
// Clear the buffer to avoid problems with input when game is reset
#define CLEARBUF() char ch; while ((ch = getchar()) != '\n' && ch != EOF);

typedef struct
{
    char date[50];
    int mode;
    int boardSize;
    int result;
    // 64 because maximum board size is 8. 8x8 >> 64
    int moves[64];
    int moveCount;
} Result;

void interact();
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
int checkStatus(int** board, int);
int checkHorizontal(int** board, int);
int checkVertical(int** board, int);
int checkDiagonal(int** board, int);
void push(struct stack*, int, int);
void *pop(struct stack*);
void *pop_all(struct stack*);
void storeResults(struct stack*, int, int, int);
int fileExists(const char*);
void loadResults(int*, Result*);
void processResults(int*, Result*);
void removeNewline(char*);
void replayMatch(Result, int);
void replayMatchDisplay(int** board, int, int);
void getMatchNumber();
void computerVsComputer(int);
int playerMove(int** board, int, int*, int*, struct stack*, struct stack*, int, int, int);
void computerMove(int** board, int, struct stack*, int);

struct stack
{
    int moves[64];
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
    // Default board size == 3
    int boardSize = 3;
    displayMenuOptions("");
    do
    {
        fgets(userInput, 100, stdin);
        option = atoi(userInput);
        switch (option)
        {
            case 1:
                match(1, boardSize);
                break;
            case 2:
                displayMenuOptions("");
                match(2, boardSize);
                break;
            case 3:
                computerVsComputer(boardSize);
                break;
            case 4:
                changeBoardSize(&boardSize);
                
                char info[50] = "New board size: ";
                char boardSizeStr[5];
                strcat(info, itoa(boardSize, boardSizeStr, 10));
                
                displayMenuOptions(info);
                break;
            case 5:
                displayMenuOptions("");
                int count = 0;
                Result* results = (Result*) malloc(sizeof(Result)  * 100);
                loadResults(&count, results);
                processResults(&count, results);
                free(results); 
                break;
            case 6:
                displayMenuOptions("");
                getMatchNumber();
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

    return 0;
}

void removeNewline(char *s) {
    while(*s && *s != '\n' && *s != '\r') s++;
    *s = 0;
}

void displayMenuOptions(char info[50])
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
    // If there was information provided to the method, display it
    if (strcmp(info, "") != 0)
    {
        printf("[INFO] %s\n\n", info);
    }
    printf("Select an option (1-%d): ", OPTION_COUNT);
}

void changeBoardSize(int *boardSize)
{
    int maxBoardSize = 8;
    int minBoardSize = 3;
    displayMenuOptions("");
    printf("4\n\nCurrent board size: %d\n", *boardSize);
    printf("Select a new board size (%d-%d): ", minBoardSize, maxBoardSize);
    char userInput[5];
    int option = -1;
    fgets(userInput, 100, stdin);
    option = atoi(userInput);
    while (option < minBoardSize || option > maxBoardSize)
    {
        displayMenuOptions("");
        printf("4\n\nCurrent board size: %d\n", *boardSize);
        printf("Invalid input. Select a new board size (%d-%d): ", minBoardSize, maxBoardSize);
        fgets(userInput, 100, stdin);
        option = atoi(userInput);
    }
    *boardSize = option;
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
        // Allocate memory for each row
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

int play(int** board, int boardSize, int playerOneScore, int playerTwoScore, int startingPlayer, int mode)
{
    // A stack for storing player moves
    struct stack moveStack;
    struct stack redoStack;
    init_stack(&moveStack);
    init_stack(&redoStack);

    int currentPlayer = startingPlayer;

    displayBoard(board, boardSize, playerOneScore, playerTwoScore);
    int gameOver = 0;
    int count = 0;
    int result = 0;
    
    while(!gameOver && count < boardSize*boardSize)
    {
        count++;
        if (mode == 2 && currentPlayer == -1)
        {
            computerMove(board, boardSize, &moveStack, -1);
            //Sleep(2000);
        }
        else
        {
            playerMove(board, boardSize, &count, &currentPlayer, &moveStack, &redoStack, playerOneScore, playerTwoScore, mode);
        }
        
        displayBoard(board, boardSize, playerOneScore, playerTwoScore);
        // Switch player
        currentPlayer *= (-1); 
        
        result = checkStatus(board, boardSize);
        if (result != 0)
        {
            gameOver = true;
        }
    }

    // Write results to file 
    storeResults(&moveStack, result, mode, boardSize);
    return result;
}

int updateBoard(int** board, int boardSize, int position, int value, int override)
{
    // Make positon positive if it is negative
    if (position < 0)
    {
        position *= -1;
    }

    // position / BOARD_SIZE (rounded to nearest whole number)
    // Subtract 1 because arrays start at 0
    int row = ((position -1) / boardSize + 1) - 1;
    // Use the row and board size to work out which column the position is in
    int column = (boardSize -1) - (((row+1) * boardSize) - position);
    
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

int checkStatus(int** board, int boardSize)
{
    int status = 0;
    // Check for a horizontal win
    status = checkHorizontal(board, boardSize);
    if(status != 0)
    {
        return status;
    }
    // Check for a vertical win
    status = checkVertical(board, boardSize);
    if(status != 0)
    {
        return status;
    }
    // Check for a diagonal win
    status = checkDiagonal(board, boardSize);
    if(status != 0)
    {
        return status;
    }
    //  0 >> No winner
    // +1 >> X wins
    // -1 >> O wins
    return 0;
}

int checkHorizontal(int** board, int boardSize)
{
    int currentGroup;
    // Loop through each row
    for(int i = 0; i < boardSize; i++)
    {
        // Loop through each column in the row
        for (int j = 0; j < boardSize-2; j++)
        {
            currentGroup = 0;
            currentGroup += board[i][j];
            currentGroup += board[i][j+1];
            currentGroup += board[i][j+2];
            if (currentGroup == 3)
            {
                // X wins
                return 1;
            }
            else if (currentGroup == -3)
            {
                // O wins
                return -1;
            }
        }
    }
    // No winner yet
    return 0;
}

int checkVertical(int** board, int boardSize)
{
    int currentGroup;
    // Loop through each column
    for(int i = 0; i < boardSize; i++)
    {
        // Loop through each row in the column
        for (int j = 0; j < boardSize-2; j++)
        {
            currentGroup = 0;
            currentGroup += board[j][i];
            currentGroup += board[j+1][i];
            currentGroup += board[j+2][i];
            if (currentGroup == 3)
            {
                // X wins
                return 1;
            }
            else if (currentGroup == -3)
            {
                // O wins
                return -1;
            }
        }
    }
    // No winner yet
    return 0;
}

int checkDiagonal(int** board, int boardSize)
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
            return 1;
        }
        else if (currentGroup == -3)
        {
            // O wins
            return -1;
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
            return 1;
        }
        else if (currentGroup == -3)
        {
            // O wins
            return -1;
        }
        level--;
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
    system("cls");
    printf("Tic Tac Toe (%dx%d)\n", boardSize, boardSize);
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

void push(struct stack *s, int item, int boardSize)
{
    if (s->top == (boardSize*boardSize)-1)
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

void storeResults(struct stack *moveStack, int result, int mode, int boardSize)
{  
    time_t timer;
    char currentDateTime[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(currentDateTime, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    int *moves = (int *) malloc((boardSize*boardSize) * sizeof(int *));
    int *i = NULL;
    i = pop(moveStack);
    int count = 0;

    FILE *output = NULL;
    char outputName[50] = "tic-tac-toe_results.csv";
    output = fopen(outputName, "a");
    fprintf(output, "%s,%d,%d,%d,", currentDateTime, mode, boardSize, result);
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
        char *array[5];
        int i = 0;
        // Split the current row into the char array
        while (p != NULL)
        {
            array[i++] = p;
            p = strtok (NULL, ",");
        }       
        int boardSize = atoi(array[2]);
        char *charMoves = strtok (strdup(array[4]), " ");
        int *intMoves = (int *) malloc((boardSize*boardSize) * sizeof(int *));
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
        temp.boardSize = boardSize;
        temp.result = atoi(array[3]);
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
        printf("5\n\nNo matches found.\n");
    }
    else
    {
        printf("5\n\nPrevious Matches:\n\n");
        printf("Games Played: %d | Games Tied: %d\n", gamesPlayed, gamesTied);
        printf("X Wins: %d (%.0f%%) | O Wins: %d (%.0f%%)\n\n", playerOneWins, playerOneWinPercentage, playerTwoWins, playerTwoWinpercentage);
        
        // Print previous matches
        printf(" # |         DATE        | MODE | BOARD | RESULT | MOVES\n");
        for (int i = 0; i < *count; i++)
        {
            // NUMBER AND DATE
            if (i+1 < 10)
            {
                printf(" %d | %s | ", i+1, results[i].date);
            }
            else
            {
                printf("%d | %s | ", i+1, results[i].date);
            }
            
            
            // MODE
            if (results[i].mode == 1)
            {
                printf("PvP  | ");
            }
            else if (results[i].mode == 2)
            {
                printf("PvE  | ");
            }

            // SIZE
            printf(" %dx%d  | ", results[i].boardSize, results[i].boardSize);

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
            displayMenuOptions("");
            printf("6\n\nSelect a previous match number to replay (1-%d):\n", count);
            fgets(userInput, 100, stdin);
            option = atoi(userInput);
        } while (option < 1 || option > count);
        replayMatch(results[option-1], option);
        free(results);
    }
}

void replayMatch(Result result, int matchNumber)
{
    int **board = (int **) malloc(result.boardSize * sizeof(int *));
    initialiseBoard(board, result.boardSize);
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
        replayMatchDisplay(board, matchNumber, result.boardSize);
        Sleep(1500);
        updateBoard(board, result.boardSize, result.moves[i], currentPlayer, 0);
    }
    replayMatchDisplay(board, matchNumber, result.boardSize);
    printf("Match Replay Completed.\n");
    Sleep(1500);
    displayMenuOptions("");
    unallocateBoard(board, result.boardSize);
}

void replayMatchDisplay(int **board, int matchNumber, int boardSize)
{
    displayMenuOptions("");
    printf("REPLAY OF MATCH #%d\n", matchNumber);
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

void computerVsComputer(int boardSize)
{
    // Allocate memory for the board 2D array
    int **board = (int **) malloc(boardSize * sizeof(int *));
    initialiseBoard(board, boardSize);
    
    struct stack moveStack;
    init_stack(&moveStack);

    bool currentComputer = false;
    int count = 0;
    bool gameOver = false;
    int result = 0;

    displayBoard(board, boardSize, 0, 0);
    while(!gameOver && count < boardSize*boardSize)
    {
        count++;
        // Display board
        if (currentComputer)
        {
            computerMove(board, boardSize, &moveStack, 1);
        }
        else
        {
            computerMove(board, boardSize, &moveStack, -1);
        }
        Sleep(2000);
        displayBoard(board, boardSize, 0, 0);
        currentComputer = !currentComputer;
        result = checkStatus(board, boardSize);
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
    unallocateBoard(board, boardSize);
}

int playerMove(int **board, int boardSize, int *count, int *currentPlayer, struct stack *moveStack, struct stack *redoStack, int playerOneScore, int playerTwoScore, int mode)
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
            if (strcmp(buf, "undo") == 0)
            {
                // Dont let the player undo the first computer move
                if (mode == 2 && (*count != 2 && *currentPlayer == 1))
                {
                    // When playing against the computer, undo 2 moves at once
                    // So the player cannot change their opponents move
                    int *popped = NULL;
                    popped = pop(moveStack);
                    if (popped != NULL)
                    {
                        push(redoStack, *popped, boardSize);
                        *count -= 1;
                        *currentPlayer *= -1;
                        updateBoard(board, boardSize, *popped, *currentPlayer, 1);
                    }
                }
                if (mode == 2 && (*count == 2 && *currentPlayer == 1))
                {
                    displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                    printf("Cannot undo your opponents first move\n");
                }
                else
                {
                    int *popped = NULL;
                    popped = pop(moveStack);
                    if (popped != NULL)
                    {
                        push(redoStack, *popped, boardSize);
                        *count -= 1;
                        *currentPlayer *= -1;
                        updateBoard(board, boardSize, *popped, *currentPlayer, 1);
                        displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                        break;
                    }
                    else
                    {
                        displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                        printf("Nothing to undo\n");
                        break;
                    }
                }     
            }
            else if (strcmp(buf, "redo") == 0)
            {
                if (mode == 2)
                {
                    // When playing against the computer, redo 2 moves at once
                    // So the player cannot change their opponents move
                    int *popped = NULL;
                    popped = pop(redoStack);
                    if (popped != NULL)
                    {
                        push(moveStack, *popped, boardSize);
                        *count += 1;
                        updateBoard(board, boardSize, *popped, *currentPlayer, 0);
                        *currentPlayer *= -1;
                    }
                }
                int *popped = NULL;
                popped = pop(redoStack);
                if (popped != NULL)
                {
                    push(moveStack, *popped, boardSize);
                    *count += 1;
                    updateBoard(board, boardSize, *popped, *currentPlayer, 0);
                    displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                    *currentPlayer *= -1;
                    break;
                }
                else
                {
                    displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                    printf("Nothing to redo\n");
                    break;
                }
            }
            else
            {
                int input = strtol(buf, &end, 10);
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
                        pop_all(redoStack);
                        input *= *currentPlayer;
                        push(moveStack, input, boardSize);
                        break;
                    }
                }
                else
                {
                    displayBoard(board, boardSize, playerOneScore, playerTwoScore);
                    printf("Invalid input\n");
                    break;
                }
            }
        }
        while (end != buf + strlen(buf));
    }
    return 0;
}

int minimax(int **board, int boardSize, int player)
{
    // Check the position for the current player on the board
    int winner = checkStatus(board, boardSize);
    if (winner != 0)
    {
        return winner*player;
    }

    int row = -1;
    // Initialise score with -2 (a losing move is better than no move)
    int score = -2;
    int i, j;
    for (i = 0; i < boardSize; i++)
    {
        for (j = 0; j < boardSize; j++)
        {
            if(board[i][j] == 0)
            {
                board[i][j] = player;
                // Check the resulting score of the current move
                int currentScore = -minimax(board, boardSize, player*-1);
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

void computerMove(int **board, int boardSize, struct stack *moveStack, int value)
{
    if (value == 1)
    {
        printf("Computer (X) is thinking..\n");
    }
    else if (value == -1)
    {
        printf("Computer (0) is thinking..\n");
    }
    int row = -1;
    int col = -1;
    int score = -2;
    int i, j;
    for (i = 0; i < boardSize; i++)
    {
        for (j = 0; j < boardSize; j++)
        {
            if(board[i][j] == 0)
            {
                board[i][j] = value;
                int currentScore = -minimax(board, boardSize, value * (-1));
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
    // Value will be 1 or -1. Multiply the cell by the value to specify X or O
    int number = value * ((row * 3) + (col + 1));
    push(moveStack, number, boardSize);
}