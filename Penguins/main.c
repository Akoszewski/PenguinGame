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

enum Direction
{
    left,
    leftUp,
    leftDown,
    right,
    rightUp,
    rightDown,
};

int playersNumber;
int penguinsNumber; // number of penguins for each player
char inputboardfile[64];
char outputboardfile[64];

int cols; // number of columns
int rows; // number of rows

int turns;

struct Coordinates
{
    int x, y;
};

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

enum Direction ReadDirectionWithMessage(char* message)
{
    char input[3] = {' ', ' ', '\0'};
    printf(message);
    bool failed = false;
    do
    {
        fseek(stdin, 0, SEEK_END); // clears stdin
        fgets(input, sizeof(input), stdin);

        if(input[0] == 'L' && input[1] == '\n' || input[0] == 'l' && input[1] == '\n')
            return left;
        if(input[0] == 'L' && input[1] == 'U' || input[0] == 'l' && input[1] == 'u')
            return leftUp;
        if(input[0] == 'L' && input[1] == 'D' || input[0] == 'l' && input[1] == 'd')
            return leftDown;
        if(input[0] == 'R' && input[1] == '\n' || input[0] == 'r' && input[1] == '\n')
            return right;
        if(input[0] == 'R' && input[1] == 'U' || input[0] == 'r' && input[1] == 'u')
            return rightUp;
        if(input[0] == 'R' && input[1] == 'D' || input[0] == 'r' && input[1] == 'd')
            return rightDown;
        else
        {
            printf("Wrong input! Try again: ");
            failed = true;
        }
    } while (failed);
}

void InitBoard(int floes[cols][rows])
{
    int r,c;
    for( r = 0; r < rows; r++)
    {
        for(c = 0; c < cols; c++)
        {
            floes[c][r] = 1;
        }
    }
}

void PrintBoard(int floes[cols][rows])
{
    //system("cls");
    int c,r;
    printf("\n");

    for( r = 0; r < rows; r++)
    {
        if(r%2 == 1) printf(" ");
        for(c = 0; c < cols; c++)
        {
            printf("%d",floes[c][r]);
            printf(" ");
        }
        printf("\n");
        printf("\n");
    }
    printf("\n");
}

struct Coordinates GetNeighbouringCoord(struct Coordinates curr, enum Direction direction)
{
    struct Coordinates new_coords;
     new_coords.x = curr.x;
     new_coords.y = curr.y;

    switch(direction)
    {
    case right:
        new_coords.x = curr.x + 1;
        break;
    case left:
        new_coords.x = curr.x - 1;
        break;
    case rightUp:
        if(curr.y%2 == 0)
        {
            new_coords.y = curr.y - 1;
        }
        else
        {
            new_coords.x = curr.x + 1;
            new_coords.y = curr.y - 1;
        }
        break;
    case rightDown:
        if(curr.y%2 == 0)  // if current column is even
        {
            new_coords.y = curr.y + 1;
        }
        else
        {
            new_coords.x = curr.x + 1;
            new_coords.y = curr.y + 1;
        }
        break;
    case leftUp:
        if(curr.y%2 == 0)
        {
            new_coords.x = curr.x - 1;
            new_coords.y = curr.y - 1;
        }
        else
        {
            new_coords.y = curr.y - 1;
        }
        break;
    case leftDown:
        if(curr.y%2 == 0)
        {
            new_coords.x = curr.x - 1;
            new_coords.y = curr.y + 1;
        }
        else
        {
            new_coords.y = curr.y + 1;
        }
        break;
    }
    return new_coords;
}

bool MovePenguin()
{

}

void UpdatePenguinPosition(struct Penguin* penguin, int floes[cols][rows], int index, int new_x, int new_y) // index is index of penguin
{
    floes[penguin->x][penguin->y] = 0; // old floe disappears
    floes[new_x][new_y] = 3 + 1 + index; // place penguin on new field
    penguin->x = new_x;
    penguin->y = new_y;
}

bool IsMoveValid(struct Coordinates new_coords)
{
    // if movement is impossible return false
    if(new_coords.x >= cols || new_coords.y >= rows || new_coords.x < 0 || new_coords.y < 0)
    {
        return false;
    }
    else return true;
}

void ReadMovement(enum Direction* direction, int* jumps)
{
    *direction = ReadDirectionWithMessage("Type direction of movement: ");
    *jumps = ReadIntWithMessage("Type number of jumps: ");
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

        InitBoard(floes);

        //playersNumber = ReadIntWithMessage("Type number of players: ");
        //penguinsNumber = ReadIntWithMessage("Type number of penguins for every player: ");
        //turns = ReadIntWithMessage("Type number of turns: ");
        turns = 10;
        printf("\n");

        struct Penguin penguins[4];

        penguins[0].x = 0;
        penguins[0].y = 0;

        //printf("Your position is: %d, %d\n\n", penguins[0].x, penguins[0].y);
        while (!EndGame())
        {
            struct Coordinates new_coords = { penguins[0].x, penguins[0].y };
            enum Direction direction;
            int jumps;
            ReadMovement(&direction, &jumps);
            while(jumps > 0)
            {
                new_coords = GetNeighbouringCoord(new_coords, direction);
                if(floes[new_coords.x][new_coords.y] == 0 || floes[new_coords.x][new_coords.y] > 3)
                {
                    new_coords.x = penguins[0].x; // back to initial values
                    new_coords.y = penguins[0].y;
                    printf("You can't move there!\n"); // TODO: We have to addd something like "try again: " in loop
                    break;
                }
                jumps--;
            }
            printf("Your position is: %d, %d\n\n", new_coords.x, new_coords.y);

            if(IsMoveValid(new_coords))
            {
                UpdatePenguinPosition(&penguins[0], floes, 0, new_coords.x, new_coords.y); // penguin by reference, tab with floes, index of penguin, new coordinates
            }
            else
            {
                printf("Invalid movment\n");
            }
            PrintBoard(floes);
            //printf("Your position is: %d, %d\n\n", penguins[0].x, penguins[0].y);
            //PrintBoard(floes);
        }
    }
    printf("End of the game. Type any key to exit\n");
    getch();
    return 0;
}
