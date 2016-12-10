#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#define bool int
#define true 1
#define false 0

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
int penguinNumber;
int penguinsNumber; // number of penguins for each player
char inputboardfile[64];
char outputboardfile[64];

int cols; // number of columns
int rows; // number of rows

int currentturn;
int totalturns;

struct Coordinates
{
    int x, y;
};

struct Penguin
{
    bool placed;
    bool blocked;
    int x, y;
};

struct Player
{
    bool lost;
    int score;
    struct Penguin* penguins;
};
struct Player* players;

/*struct Movement
{
    int penguinNumber;
    int new_x;
    int new_y;
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

void PrintBoard(int floes[cols][rows])
{
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


void SetPenguinPosition(int floes[cols][rows], struct Penguin* penguin, int index, int new_x, int new_y) // index is index of penguin
{
    floes[new_x][new_y] = 3 + 1 + index; // place penguin on new field
    penguin->x = new_x;
    penguin->y = new_y;
    penguin->placed = true;
}

void UpdatePenguinPosition(int floes[cols][rows], struct Penguin* penguin, int index, int new_x, int new_y) // index is index of penguin
{
    floes[penguin->x][penguin->y] = 0; // old floe disappears
    floes[new_x][new_y] = 3 + 1 + index; // place penguin on new field
    penguin->x = new_x;
    penguin->y = new_y;
}

bool TryMovePenguin(int floes[cols][rows], struct Penguin penguin, enum Direction direction, int jumps, struct Coordinates* new_coords)
{
    new_coords->x = penguin.x;
    new_coords->y = penguin.y;
    while(jumps > 0)
    {
        *new_coords = GetNeighbouringCoord(*new_coords, direction);
        if(floes[new_coords->x][new_coords->y] == 0 || floes[new_coords->x][new_coords->y] > 3 || new_coords->x >= cols || new_coords->y >= rows || new_coords->x < 0 || new_coords->y < 0)
        {
            return false;
        }
        jumps--;
    }
    return true;
}

void ReadPlacement(int floes[cols][rows], int* penguinNumber, struct Coordinates* coords)
{
    *penguinNumber = ReadIntWithMessage("Type number of penguin: ") - 1; // penguin 1 is penguin[0]
    while(*penguinNumber >= penguinsNumber || *penguinNumber < 0)
    {
        *penguinNumber = ReadIntWithMessage("You don't have such penguin! Try again: ") - 1;
    }
    coords->x = ReadIntWithMessage("Type initial penguin x: ");
    coords->y = ReadIntWithMessage("Type initial penguin y: ");
    while(coords->x >= cols || coords->y >= rows || coords->x < 0 || coords->y < 0 || floes[coords->x][coords->y] != 1)
    {
        printf("You can't place penguin here!!! Try again. \n");
        *penguinNumber = ReadIntWithMessage("Type number of penguin: ") - 1;
        while(*penguinNumber >= penguinsNumber || *penguinNumber < 0)
        {
            *penguinNumber = ReadIntWithMessage("You don't have such penguin! Try again: ") - 1;
        }
        coords->x  = ReadIntWithMessage("Type initial penguin x: ");
        coords->y = ReadIntWithMessage("Type initial penguin y: ");
    }
}

void ReadMovement(int* penguinNumber, enum Direction* direction, int* jumps)
{
    *penguinNumber = ReadIntWithMessage("Type number of penguin: ") - 1; // penguin 1 is penguin[0]
    while(*penguinNumber >= penguinsNumber)
    {
        *penguinNumber = ReadIntWithMessage("You don't have such penguin! Try again: ") - 1;
    }
    *direction = ReadDirectionWithMessage("Type direction of movement: ");
    *jumps = ReadIntWithMessage("Type number of jumps: ");
}

bool EndGame()
{
    if(currentturn > totalturns) return true;
    return false;
}

struct Player* CreatePlayers(int playersNumber, int penguinsNumber)  // create players and give them penguins
{
    int i, j;
    struct Player* players = (struct Player*)malloc(playersNumber*sizeof(struct Player));
    for(i = 0; i < playersNumber; i++)
    {
        players[i].penguins = (struct Penguin*)malloc(penguinsNumber*sizeof(struct Penguin));
        players[i].lost = false;
        players[i].score = 0;
        for(j = 0; j < penguinsNumber; j++)
        {
            players[i].penguins[j].x = -1;
            players[i].penguins[j].y = -1;
            players[i].penguins[j].placed = false;
            players[i].penguins[j].blocked = false;
        }
    }
    return players;
}

void RemovePlayers(struct Player* players, int playersNum) // ree memory
{
    int i;
    for(i=0; i < playersNum; i++)
    {
        free(players[i].penguins);
    }
    free(players);
}

void ReadBoard(int floes[cols][rows], char* fname)
{
    int i, j, k;
	FILE * fPointer = NULL;
	fPointer = fopen(fname, "r");
    if(fPointer == NULL)
    {
        printf("No input file found! \n");
        getch();
        exit(0);
    }

	fscanf(fPointer, "%d %d\n", &playersNumber, &penguinsNumber);
	fscanf(fPointer, "%d %d\n", &cols, &rows);

    players = CreatePlayers(playersNumber, penguinsNumber);

	for (i = 0; i < cols; i++)
	{
		for (j = 0; j < rows; j++)
        {
            fscanf(fPointer, "%d ", &floes[j][i]);
        }
	}

	fscanf(fPointer, "%d/%d", &currentturn, &totalturns);

	for(k = 0; k < playersNumber; k++)
    {
        fscanf(fPointer, "%d ", &players[k].score);
    }
    fclose(fPointer);
}

void PrintScoreTable()
{
    int i;

    printf("\nSCORES\n\n");

    for (i = 0; i < playersNumber; i++)
    {
        printf("Player %d: %d\n", i+1, players[i].score);
    }
    printf("\n");
}

void PrintInitialGameState()
{
    printf("Number of players is: %d\n", playersNumber);
    printf("Number of penguins: %d\n", penguinsNumber);
}

void PrintGameState()
{

}

int main(int argc, char* argv[])
{
    int i, j;
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
        ReadBoard(floes, "board.txt");
        PrintInitialGameState(); // Prints num of players and penguins for each player
        PrintBoard(floes);
        phase = placement;

        while (!EndGame())
        {
            struct Coordinates new_coords;
            enum Direction direction;
            int jumps;

            for(i = 0; i < playersNumber; i++)
            {
                printf("Player %d: \n", i+1);
                if(phase == placement)
                {
                    ReadPlacement(floes, &penguinNumber, &new_coords);
                    while(players[i].penguins[penguinNumber].placed)
                    {
                        printf("This penguin is already placed! Try again.\n");
                        ReadPlacement(floes, &penguinNumber, &new_coords);
                    }
                    SetPenguinPosition(floes, &players[i].penguins[penguinNumber], penguinsNumber*i + penguinNumber, new_coords.x, new_coords.y);
                    PrintBoard(floes);
                }
                else
                {
                    bool success = false;
                    while(!success)
                    {
                        ReadMovement(&penguinNumber, &direction, &jumps);
                        if(TryMovePenguin(floes, players[i].penguins[penguinNumber], direction, jumps, &new_coords))
                        {
                            players[i].score += floes[new_coords.x][new_coords.y];
                            UpdatePenguinPosition(floes, &players[i].penguins[penguinNumber], penguinsNumber*i + penguinNumber, new_coords.x, new_coords.y); // tab with floes, penguin by reference, index of penguin, new coordinates
                            success = true;
                        }
                        else
                        {
                            printf("Invalid movment! Try again.\n");
                        }
                    }
                    PrintScoreTable();
                    PrintBoard(floes);
                }
            }
            currentturn++;
            if(currentturn > penguinsNumber)
            {
                phase = movement;
            }
        }
    }
    printf("End of the game. Type any key to exit\n");
    RemovePlayers(players, playersNumber);
    getch();
    return 0;
}
