#include <stdio.h>

const char * options[] = {
        "Player vs Player",
        "Player vs Computer (N/A)",
        "Change Board Size (N/A)",
        "Leaderboards (N/A)",
        "Previous Matches (N/A)",
        "Export Results (CSV) (N/A)",
        "Exit",
};

#define OPTION_COUNT (sizeof (options) / sizeof (const char *))

void displayOptions();
void interact();

int main(void)
{
    interact();
    return 0;
}

void interact()
{
    char userInput[5];
    int option = -1;
    displayOptions();
    do
    {
        fgets(userInput, 100, stdin);
        option = atoi(userInput);
        switch (option)
        {
            case 1:
                displayOptions();
                printf("Selected 1\n");
                break;
            case 2:
                displayOptions();
                printf("Selected 2\n");
                break;
            case 3:
                displayOptions();
                printf("Selected 3\n");
                break;
            case 4:
                displayOptions();
                printf("Selected 4\n");
                break;
            case 5:
                displayOptions();
                printf("Selected 5\n");
                break;
            case 6:
                displayOptions();
                printf("Selected 6\n");
                break;
            case OPTION_COUNT:
                printf("Selected Exit\n");
                break;
            default:
                displayOptions();
                printf("[Error] Invalid option selected. Please enter a value between 1-%d:\n", OPTION_COUNT);
                break;
        }
    } while (option < 1 || option > OPTION_COUNT);
}

void displayOptions()
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