#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#define bool int
#define true 1
#define false 0

#define SIZE 100

enum Phase
{
    placementPhase,
    movementPhase
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

int numOfPlayers;
int numOfPenguins; // number of penguins for each player
char inputboardfile[64];
char outputboardfile[64];
int currentPlayerIndex;

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
    struct Coordinates coords;
};

struct Player
{
    bool lost;
    int score;
    struct Penguin* penguins;
    struct Movement* movements;
    int numOfMovements;
};
struct Player* players;


struct Movement
{
    int penguinIndex;
    struct Coordinates coords;
};

struct MovementInSteps
{
    int penguinIndex;
    enum Direction direction;
    int jumps;
};

bool ReadArgs(int argc, char* argv[])
{

    if(argc < 2)
    {
        printf("No parameters passed. The program will be opened in interactive mode.\n\n");
        return false;
    }
    currentPlayerIndex = atoi(arg[0]) - 1;
    if(strcmp(argv[1], "phase=placement") == 0) // 0 means the strings are equal
    {
        phase = placementPhase;
    }
    else if(strcmp(argv[1], "phase=movement") == 0)
    {
        phase = movementPhase;
    }
    else
    {
        printf("Wrong input (first parameter invalid)\n");
        exit(0);
    }
    if(phase == placementPhase)
    {
        if(strncmp(argv[2], "penguins=", 9) == 0) // if first 9 letters are equal to "penguins="
        {
            numOfPenguins = atoi(&argv[2][9]); // convert numbers after "=" to int and assign it to penguinsNum
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

bool IsFieldInScope(struct Coordinates coords)
{
    if(coords.x >= cols || coords.y >= rows || coords.x < 0 || coords.y < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool IsFieldValid(int board[cols][rows], struct Coordinates coords) // checks if a field is valid to move on
{
    if(IsFieldInScope(coords))
    {
        if(board[coords.x][coords.y] == 0 || board[coords.x][coords.y] > 3)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

struct Movement GetMaxFishes(int board[cols][rows], struct Movement* movements, int numOfMovements)
{
    int i;
    struct Movement bestMov = movements[0];
    for(i = 0; i < numOfMovements; i++)
    {
        if(board[movements[i].coords.x][movements[i].coords.y] == 3)
        {
            bestMov = movements[i];
            break;
        }
        else
        {
            if(board[movements[i].coords.x][movements[i].coords.y] > board[bestMov.coords.x][bestMov.coords.y])
                bestMov = movements[i];
        }
    }
    return bestMov;
}

int TakeNextPenguinIndex(struct Penguin* penguins)
{
    int penguinIndex = 0;
    while(penguins[penguinIndex].placed)
    {
        penguinIndex++;
        if(penguinIndex > numOfPenguins)
        {
            perror("Error of TakeNextPenguinIndex function, too big penguinIndex!");
            exit(0);
        }
    }
    return penguinIndex;
}

void PrintBoard(int board[cols][rows])
{
    int c,r;
    printf("\n");

    for( r = 0; r < rows; r++)
    {
        if(r%2 == 1) printf(" ");
        for(c = 0; c < cols; c++)
        {
            printf("%d",board[c][r]);
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

struct Coordinates GetNeighbouringCoords(struct Coordinates curr, enum Direction direction)
{
    struct Coordinates new_coords;
     new_coords = curr;

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


void SetPenguinPosition(int board[cols][rows], struct Penguin* penguin, int index, struct Coordinates coords) // index is index of penguin
{
    board[coords.x][coords.y] = 3 + 1 + index; // place penguin on new field
    penguin->coords = coords;
    penguin->placed = true;
}

void UpdatePenguinPosition(int board[cols][rows], struct Penguin* penguin, int index, struct Coordinates coords) // index is index of penguin
{
    board[penguin->coords.x][penguin->coords.y] = 0; // old floe disappears
    board[coords.x][coords.y] = 3 + 1 + index; // place penguin on new field
    penguin->coords = coords;
}

bool TryMovement(int board[cols][rows], struct Coordinates startingPoint, struct MovementInSteps movement, struct Coordinates* new_coords)
{
    *new_coords = startingPoint;
    while(movement.jumps > 0)
    {
        *new_coords = GetNeighbouringCoords(*new_coords, movement.direction);
        if(!IsFieldValid(board, *new_coords))
        {
            return false;
        }
        movement.jumps--;
    }
    return true;
}

struct Movement ReadPlacement(int board[cols][rows], struct Penguin* penguins)
{
    struct Movement placement;
    placement.penguinIndex = TakeNextPenguinIndex(penguins);
    bool success = false;
    while(!success)
    {
        placement.coords.x = ReadIntWithMessage("Type initial penguin x: ") - 1; // board[0][0] is 1,1 in game
        placement.coords.y = ReadIntWithMessage("Type initial penguin y: ") - 1;
        if(IsFieldInScope(placement.coords))
        {
            if(board[placement.coords.x][placement.coords.y] != 1)
            {
                puts("You can't place penguin here!!! You can place your penguin only on field with one fish. Try again!");
            }
            else
            {
                success = true;
            }
        }
        else
        {
            puts("Such field doesn't exist! Try again!");
        }
    }
    return placement;
}

struct Movement ChooseBestPlacement(int board[cols][rows], struct Penguin* penguins)
{
    struct Movement placement;
    placement.penguinIndex = TakeNextPenguinIndex(penguins);
    do
    {
        placement.coords.x = rand() % cols;
        placement.coords.y = rand() % rows;
    }
    while (board[placement.coords.x][placement.coords.y] != 1);
    return placement;
}

void FindAllMovements(int board[cols][rows], struct Player* player) // fills the player's array of possible movements
{
    player->movements = (struct Movement*) malloc(SIZE * sizeof(struct Movement));
    player->numOfMovements = 0;
    int penguinIndex = 0;
    enum Direction direction = 0;
    for(penguinIndex = 0; penguinIndex < numOfPenguins; penguinIndex++)
    {
        for(direction = 0; direction < 6; direction++) //  6 directions
        {
            struct Coordinates new_coords = player->penguins[penguinIndex].coords;
            while(true)
            {
                new_coords = GetNeighbouringCoords(new_coords, direction);
                if(IsFieldValid(board, new_coords)) // if movement is valid add it to the array
                {
                    player->movements[player->numOfMovements].penguinIndex = penguinIndex;
                    player->movements[player->numOfMovements].coords = new_coords;
                    player->numOfMovements++;

                    if((player->numOfMovements % SIZE) == 0) // allocate new memory if needed
                    {
                        player->movements = (struct Movement*) realloc(player->movements, (player->numOfMovements + SIZE) * sizeof(struct Movement));
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
}

struct MovementInSteps ReadMovement(int playerIndex)
{
    struct MovementInSteps movement;
    int minPeng, maxPeng;

    minPeng = 4 + playerIndex * numOfPenguins;
    maxPeng = minPeng + numOfPenguins - 1;

    printf("Type number of penguin (%d - %d): ", minPeng, maxPeng);
    movement.penguinIndex = ReadIntWithMessage("") - playerIndex * numOfPenguins - 4; // penguin 7 is player[1].penguin[0]
    while(movement.penguinIndex >= numOfPenguins || movement.penguinIndex < 0)
    {
        movement.penguinIndex = ReadIntWithMessage("You don't have such penguin! Try again!: ") - playerIndex*numOfPenguins - 4;
    }
    movement.direction = ReadDirectionWithMessage("Type direction of movement: ");
    movement.jumps = ReadIntWithMessage("Type number of jumps: ");
    return movement;
}

bool EndGame()
{
    if(currentturn > totalturns) return true;
    return false;
}

struct Player* CreatePlayers(int numOfPlayers, int numOfPenguins)  // creates players and gives them penguins
{
    int i, j;
    struct Player* players = (struct Player*) malloc(numOfPlayers*sizeof(struct Player));
    for(i = 0; i < numOfPlayers; i++)
    {
        players[i].penguins = (struct Penguin*) malloc(numOfPenguins*sizeof(struct Penguin));
        players[i].lost = false;
        players[i].score = 0;
        players[i].numOfMovements = 0;
        players[i].movements = NULL;
        for(j = 0; j < numOfPenguins; j++)
        {
            players[i].penguins[j].coords.x = -1;
            players[i].penguins[j].coords.y = -1;
            players[i].penguins[j].placed = false;
            players[i].penguins[j].blocked = false;
        }
    }
    return players;
}

void RemovePlayers(struct Player* players, int numOfPlayers) // free memory
{
    int i;
    for(i=0; i < numOfPlayers; i++)
    {
        free(players[i].penguins);
        free(players[i].movements);
    }
    free(players);
}

void WriteBoard(int board[cols][rows], char* fname)
{
    int i, j, k;
    int numOfFieldsToPlacePeng = 0; // Number of fields with exactly one fish
	FILE * fPointer = NULL;
	fPointer = fopen(fname, "w");
    if(fPointer == NULL)
    {
        printf("Can not open file! \n");
        exit(0);
    }

	fprintf(fPointer, "%d %d\n", numOfPlayers, numOfPenguins);
	fprintf(fPointer, "%d %d\n", cols, rows);


	for (i = 0; i < cols; i++)
	{
		for (j = 0; j < rows; j++)
        {
            fprintf(fPointer, "%d ", board[j][i]);
        }
        fprintf(fPointer, "\n");
	}


	fprintf(fPointer, "%d/%d", currentturn, totalturns);
	fprintf(fPointer, "\n");
	for(k = 0; k < numOfPlayers; k++)
    {
        fprintf(fPointer, "%d ", players[k].score);
    }
    fclose(fPointer);
}


void ReadBoard(int board[cols][rows], char* fname)
{
    int i, j, k;
    int numOfFieldsToPlacePeng = 0; // Number of fields with exactly one fish
	FILE * fPointer = NULL;
	fPointer = fopen(fname, "r");
    if(fPointer == NULL)
    {
        printf("No input file found! \n");
        getch();
        exit(0);
    }

	fscanf(fPointer, "%d %d\n", &numOfPlayers, &numOfPenguins);
	fscanf(fPointer, "%d %d\n", &cols, &rows);

    players = CreatePlayers(numOfPlayers, numOfPenguins);

	for (i = 0; i < cols; i++)
	{
		for (j = 0; j < rows; j++)
        {
            fscanf(fPointer, "%d ", &board[j][i]);
            if(board[j][i] == 1)
            {
                numOfFieldsToPlacePeng++;
            }
        }
	}

	//remove later
	srand(time(NULL));
    numOfFieldsToPlacePeng = 0;
	for (i = 0; i < cols; i++)
	{
		for (j = 0; j < rows; j++)
        {
            board[j][i] = rand() % 3 + 1;
            if(board[j][i] == 1)
            {
                numOfFieldsToPlacePeng++;
            }
        }
	}
    if(numOfFieldsToPlacePeng < numOfPlayers*numOfPenguins)
    {
        puts("The board is invalid! Not enough fields to place all penguins.");
        exit(0);
    }

	fscanf(fPointer, "%d/%d", &currentturn, &totalturns);

	for(k = 0; k < numOfPlayers; k++)
    {
        fscanf(fPointer, "%d ", &players[k].score);
    }
    fclose(fPointer);
}

void PrintScoreTable()
{
    int i;

    printf("\nSCORES\n\n");

    for (i = 0; i < numOfPlayers; i++)
    {
        printf("Player %d: %d\n", i+1, players[i].score);
    }
    printf("\n");
}

void PrintInitialGameState()
{
    printf("Number of players is: %d\n", numOfPlayers);
    printf("Number of penguins: %d\n", numOfPenguins);
}

void PrintGameState()
{

}

int main(int argc, char* argv[])
{
    int i;
    if( ReadArgs(argc, argv) )
    {
        //Batch mode

        // Very early sketch

        //ReadBoard();
        if (phase == placementPhase)
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
        if(phase == movementPhase) printf("Phase is: movement\n");
        else printf("Phase is: placement\n");
        printf("Penguins number is: %d\n", numOfPenguins);
        printf("Input file is: %s\n", inputboardfile);
        printf("Output file is: %s\n", outputboardfile);

    }
    else
    {
        // Interactive mode


        rows = 10;
        cols = 10;
        int board[cols][rows];
        ReadBoard(board, "board.txt");
        PrintInitialGameState(); // Prints num of players and penguins for each player
        PrintBoard(board);
        phase = placementPhase;

        while (!EndGame())
        {
            int playerIndex;
            printf("Turn %d: \n", currentturn);
            for(playerIndex = 0; playerIndex < numOfPlayers; playerIndex++)
            {
                printf("Player %d: \n", playerIndex + 1);
                if(phase == placementPhase)
                {
                    //struct Movement placement = ReadPlacement(board, players[playerIndex].penguins);
                    struct Movement placement = ChooseBestPlacement(board, players[playerIndex].penguins);
                    players[playerIndex].score += board[placement.coords.x][placement.coords.y];
                    SetPenguinPosition(board, &players[playerIndex].penguins[placement.penguinIndex], numOfPenguins*playerIndex + placement.penguinIndex, placement.coords);
                    PrintBoard(board);
                }
                else
                {
                    FindAllMovements(board, &players[playerIndex]);
                    if(players[playerIndex].numOfMovements == 0)
                    {
                        players[playerIndex].lost = true;
                    }

                    // remove later (it's for testing algotrithms)
                    /*if(!players[playerIndex].lost)
                    {
                        struct Movement movement;
                        if(playerIndex % 2)  // player 2
                        {
                            movement = GetMaxFishes(board, players[playerIndex].movements, players[playerIndex].numOfMovements);
                            //movement = players[playerIndex].movements[0]; // first movement from array (moving in an order)
                        }
                        else // player 1
                        {
                            int movementInd;
                            movementInd = rand() % players[playerIndex].numOfMovements; // random (chaotic) movement
                            movement = players[playerIndex].movements[movementInd];
                        }
                        players[playerIndex].score += board[movement.coords.x][movement.coords.y];
                        UpdatePenguinPosition(board, &players[playerIndex].penguins[movement.penguinIndex], numOfPenguins*playerIndex + movement.penguinIndex, movement.coords);
                    }
                    else
                    {
                        printf("Player lost!!!\n");
                    }*/

                    bool success = false;
                    while(!success)
                    {
                        struct Coordinates new_coords;
                        struct MovementInSteps movement = ReadMovement(playerIndex);

                        if(TryMovement(board, players[playerIndex].penguins[movement.penguinIndex].coords, movement, &new_coords))
                        {
                            players[playerIndex].score += board[new_coords.x][new_coords.y];
                            UpdatePenguinPosition(board, &players[playerIndex].penguins[movement.penguinIndex], numOfPenguins*playerIndex + movement.penguinIndex, new_coords); // tab with board, penguin by reference, index of penguin, new coordinates
                            success = true;
                        }
                        else
                        {
                            puts("Invalid movment! Try again.");
                        }
                    }
                    PrintScoreTable();
                    PrintBoard(board);
                    free(players[playerIndex].movements);
                }
            }
            if(currentturn >= numOfPenguins) // if all penguins are placed switch to the movement phase
            {
                phase = movementPhase;
            }
            currentturn++;
        }
    }
    printf("End of the game. Type any key to exit\n");
    RemovePlayers(players, numOfPlayers);
    getch();
    return 0;
}
