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

void displayMenuOptions();
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
                printf("Selected 2\n");
                break;
            case 3:
                displayMenuOptions();
                printf("Selected 3\n");
                break;
            case 4:
                displayMenuOptions();
                printf("Selected 4\n");
                break;
            case 5:
                displayMenuOptions();
                printf("Selected 5\n");
                break;
            case 6:
                displayMenuOptions();
                printf("Selected 6\n");
                break;
            case OPTION_COUNT:
                displayMenuOptions();
                printf("Selected Exit\n");
                break;
            default:
                displayMenuOptions();
                printf("[Error] Invalid option selected. Please enter a value between 1-%d:\n", OPTION_COUNT);
                break;
        }
    } while (option != 7);
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