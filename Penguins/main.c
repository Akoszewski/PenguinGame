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

int playersNumber;
int penguinsNumber; // number of penguins for each player
char inputboardfile[64];
char outputboardfile[64];

int cols; // number of columns
int rows; // number of rows

int new_x;
int new_y;

int turns;

struct Penguin
{
    bool blocked;
    int x, y;
};

/*struct Movement
{
    int penguinNumber;
    int new_x;
    int new_y;
};
*/
/*struct Player
{
    bool lost;
    struct Penguin* penguins;
};
*/

/*int GetNeighbouringCoord(enum Direction direction)
{
    return coords;
}*/

int ReadIntWithMessage(char* message)
{
    int number;
    printf("%s", message);

    fseek(stdin, 0, SEEK_END); // clears stdin
    while(!scanf("%d", &number)) // while scanf returned 0 (errror) execute loop
    {
        printf("Invalid input! Type an integer: ");
        fseek(stdin, 0, SEEK_END);
    }
    return number;
}

void PrintBoard(int floes[cols][rows])
{
    // Print board to screen
}

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
            penguinsNumber = atoi(&argv[2][9]); // convert numbers after "=" to int and assign it to penguinsNum
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
        strcpy(inputboardfile, argv[2]); // copy argv[2] to inputboardfile
        strcpy(outputboardfile, argv[3]);
    }
    return true;
}

void UpdatePenguinPosition(struct Penguin* penguin, int floes[cols][rows], int index, int new_x, int new_y) // index is index of penguin
{
    floes[penguin->x][penguin->y] = 0; // old floe disappears
    floes[new_x][new_y] = 3 + 1 + index; // place penguin on new field
    penguin->x = new_x;
    penguin->y = new_y;
}

bool IsMoveValid()
{
    // if movement is impossible return false
    if(new_x >= cols || new_y >= rows)
    {
        return false;
    }
    else return true;
}

void ReadMovement()
{
    new_x = ReadIntWithMessage("Type new penguin x: ");
    new_y = ReadIntWithMessage("Type new penguin y: ");
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
        //Batch mode

        // Very early sketch

        //ReadBoard();
        if (phase == placement)
        {
            //ChooseBestPlacement();   // number of fishes on floe must be equal to 1 !!!
            //UpdatePenguinPosition();
            //UpdateBoard();
        }
        else
        {
            //ChooseBestMovement();  // the more fishes the better
            //UpdatePenguinPosition();
            //UpdateBoard();
        }

        // remove later
        if(phase == movement) printf("Phase is: movement\n");
        else printf("Phase is: placement\n");
        printf("Penguins number is: %d\n", penguinsNumber);
        printf("Input file is: %s\n", inputboardfile);
        printf("Output file is: %s\n", outputboardfile);

    }
    else
    {
        // Interactive mode


        rows = 10;
        cols = 10;
        int floes[cols][rows];

        playersNumber = ReadIntWithMessage("Type number of players: ");
        penguinsNumber = ReadIntWithMessage("Type number of penguins for every player: ");
        turns = ReadIntWithMessage("Type number of turns: ");
        printf("\n");

        struct Penguin penguins[4];

        penguins[0].x = 0;
        penguins[0].y = 0;

        printf("Your position is: %d, %d\n\n", penguins[0].x, penguins[0].y);
        while (!EndGame())
        {
            ReadMovement();
            if(IsMoveValid())
            {
                UpdatePenguinPosition(&penguins[0], floes, 0, new_x, new_y); // penguin by reference, tab with floes, index of penguin, new coordinates
            }
            else
            {
                printf("Invalid movment\n");
            }
            printf("Your position is: %d, %d\n\n", penguins[0].x, penguins[0].y);
            PrintBoard(floes);
        }
    }
    return 0;
}

