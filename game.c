#include <stdio.h>

void displayBoard();

int main(void)
{
    printf("Tic Tac Toe\n");
    
    displayBoard();
    return 0;
}

void displayBoard()
{
    printf(" =======================\n");
    printf(" ||     ||     ||     ||\n");
    printf(" ||  %d  ||  %d  ||  %d  ||\n", 1, 2, 3);
    printf(" ||     ||     ||     ||\n");
    printf(" =======================\n");
    printf(" ||     ||     ||     ||\n");
    printf(" ||  %d  ||  %d  ||  %d  ||\n", 4, 5, 6);
    printf(" ||     ||     ||     ||\n");
    printf(" =======================\n");
    printf(" ||     ||     ||     ||\n");
    printf(" ||  %d  ||  %d  ||  %d  ||\n", 7, 8, 9);
    printf(" ||     ||     ||     ||\n");
    printf(" =======================\n");
}