#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>

enum Phase
{
    placement,
    movement
} phase;

int penguinsNum = 0; // number of penguins
char inputboardfile[64];
char outputboardfile[64];

int rows; // number of rows
int cols; // number of columns

int new_x;
int new_y;

int turns;

struct Floe
{
    bool exists;
    int fishes;
};

struct Penguin
{
    int x, y;
};

bool ReadArgs(int argc, char* argv[])
{

    if(argc < 2)
    {
        printf("No parameters passed. The program will be opened in interactive mode.\n\n");
        return false;
    }
    if(strcmp(argv[1], "phase=placement") == 0) // 0 means the strings are equal
    {
        phase = placement;
    }
    else if(strcmp(argv[1], "phase=movement") == 0)
    {
        phase = movement;
    }
    else
    {
        printf("Wrong input (first parameter invalid)\n");
        exit(0);
    }
    if(phase == placement)
    {
        if(strncmp(argv[2], "penguins=", 9) == 0) // if first 9 letters are equal to "penguins="
        {
            penguinsNum = atoi(&argv[2][9]); // convert numbers after "=" to int and assign it to penguinsNum
        }
        else
        {
            printf("Wrong input (second parameter invalid)\n");
            exit(0);
        }
        strcpy(inputboardfile, argv[3]); // copy argv[3] to inputboardfile
        strcpy(outputboardfile, argv[4]);
    }
    else
    {
        strcpy(inputboardfile, argv[2]); // copy argv[3] to inputboardfile
        strcpy(outputboardfile, argv[3]);
    }
    return true;
}

void UpdatePenguinPosition(struct Penguin* penguin, int new_x, int new_y)
{
    penguin->x = new_x;
    penguin->y = new_y;
}

bool IsMoveValid()
{
    // if movement is impossible return false
    if(new_x >= rows || new_y >= cols)
    {
        return false;
    }
    else return true;
}

void ReadMovement()
{
    printf("Type new penguin x: ");
    scanf("%d", &new_x);
    printf("Type new penguin y: ");
    scanf("%d", &new_y);
}

bool EndGame()
{
    static int counter = 0;
    counter++;
    if(counter > turns) return true;
    return false;
}

int main(int argc, char* argv[])
{
    if( ReadArgs(argc, argv) )
    {
        //Normal mode

        // remove later
        if(phase == movement) printf("Phase is: movement\n");
        else printf("Phase is: placement\n");
        printf("Penguins number is: %d\n", penguinsNum);
        printf("Input file is: %s\n", inputboardfile);
        printf("Output file is: %s\n", outputboardfile);
    }
    else
    {
        // Interactive mode

        cols = 10; rows = 10;
        struct Floe floes[rows][cols];

        printf("Type number of turns: ");
        scanf("%d", &turns);
        printf("\n");

        struct Penguin penguins[4];

        printf("Your position is: %d, %d\n\n", penguins[0].x, penguins[0].y);
        while (!EndGame())
        {
            ReadMovement();
            if(IsMoveValid())
            {
                UpdatePenguinPosition(&penguins[0], new_x, new_y);
            }
            else
            {
                printf("Invalid movment\n");
            }
            printf("Your position is: %d, %d\n\n", penguins[0].x, penguins[0].y);
        }
    }
    return 0;
}

