#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>

#define ESC 27

enum Phase
{
    placement,
    movement
} phase;

int penguins = 0;
char inputboardfile[64];
char outputboardfile[64];

int rows; // number of rows
int cols; // number of columns

int new_x;
int new_y;

struct Floe
{
    bool exists;
    int fishes;
};

struct Penguin
{
    int x, y;
};

struct Penguin penguin;

bool ReadArgs(int argc, char* argv[])
{
    if(argc < 5)  // argc is number of parameters but there is one default parameter (arg[0]) that's why it's 5 not 4
    {
        if(argc < 2)
        {
            printf("No parameters passed. The program will be opened in interactive mode.\n\n");
            return false;
        }
        printf("Wrong number of parameters (required is 4)\n\n");
        exit(0);
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
    if(strncmp(argv[2], "penguins=", 9) == 0) // if first 9 letters are equal to "penguins="
    {
        penguins = atoi(&argv[2][9]); // convert numbers after "=" to int and assign it to penguins
    }
    else
    {
        printf("Wrong input (second parameter invalid)\n");
        exit(0);
    }
    strcpy(inputboardfile, argv[3]); // copy argv[3] to inputboardfile
    strcpy(outputboardfile, argv[4]);
    return true;
}

void UpdatePenguinPosition(struct Penguin* penguin, int new_x, int new_y)
{
    penguin->x = new_x;
    penguin->y = new_y;
}

bool IsMoveValid()
{
    // TODO: if move is impossible return false
    return true;
}

void ReadMovement()
{
    printf("Type new penguin x: ");
    scanf("%d", &new_x);
    printf("Type new penguin y: ");
    scanf("%d", &new_y);
}

/*bool Exit()
{
    printf("Press any key to continue (or ESC to close)");
    int key = getch();
    printf("\n\n");
    if(key == (int)'q' || key == ESC) return true;
    else return false;
}*/

int main(int argc, char* argv[])
{
    if( ReadArgs(argc, argv) )
    {
        //Normal mode

        // remove later
        if(phase == movement) printf("Phase is: movement\n");
        else printf("Phase is: placement\n");
        printf("Penguins number is: %d\n", penguins);
        printf("Input file is: %s\n", inputboardfile);
        printf("Output file is: %s\n", outputboardfile);
    }
    else
    {
        // Interactive mode

        /*printf("type board size (rows): ");
        scanf("%d", &rows);
        printf("type board size (cols): ");
        scanf("%d", &cols);*/

        rows = 10; cols = 10;
        struct Floe floes[rows][cols];

        struct Penguin penguin;
        penguin.x = 0;
        penguin.y = 0;

        printf("Your position is: %d, %d\n\n", penguin.x, penguin.y);
        while (true)
        {
            ReadMovement();
            if(IsMoveValid())
            {
                UpdatePenguinPosition(&penguin, new_x, new_y);
            }
            printf("Your position is: %d, %d\n\n", penguin.x, penguin.y);
        }
    }
    return 0;
}

