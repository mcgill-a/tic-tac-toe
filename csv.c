#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char date[50];
    int mode;
    int result;
    int moves[1000];
    int moveCount;
} Result;

void removeNewline(char *s) {
    while(*s && *s != '\n' && *s != '\r') s++;
    *s = 0;
}

int main()
{
    FILE* resultsFile = fopen("tic-tac-toe_results.csv", "r");

    char row[1024];
    Result* results = (Result*) malloc(sizeof(Result)  * 100);
    int count = 0;
    while (fgets(row, 1024, resultsFile))
    {
        removeNewline(row);
        //char* values = strdup(row);
        //printf("VALUES: %s\n", values);
        //printf("%d: %s\n", count, row);
        char *p = strtok (strdup(row), ",");
        char *array[4];
        int i = 0;
        while (p != NULL)
        {
            array[i++] = p;
            p = strtok (NULL, ",");
        }

        // split array[3] by " "
        // convert each value to int
        // push it to temp[moves]
        char *charMoves = strtok (strdup(array[3]), " ");
        // [BOARD_SIZE * BOARD_SIZE]
        int intMoves[1000];
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
        //printf("%s\n", values);
        //printf("%s,%d,%d\n", temp.date, temp.mode, temp.result);
        results[count] = temp;
        //free(values);
        count++;
    }
    for (int i = 0; i < count; i++)
    {
        printf("%d | %s\t %d\t%d\t", i, results[i].date, results[i].mode, results[i].result);
        for (int j = results[i].moveCount - 1; j >= 0; j--)
        {
            if (j != 0)
            {
                if (results[i].moves[j] < 0) 
                {
                    printf("%d ", results[i].moves[j]);
                }
                else
                {
                    printf(" %d ", results[i].moves[j]);
                }
            }
            else
            {
                if (results[i].moves[j] < 0) 
                {
                    printf("%d", results[i].moves[j]);
                }
                else
                {
                    printf(" %d", results[i].moves[j]);
                }
            }
        }
        printf("\n");
    }
    free(results);
}

