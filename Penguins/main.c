#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <math.h>

#define bool int
#define true 1
#define false 0

#define SIZE 100 // 100*sizeof(struct Movement) is amount of allocated memory every time when allocation is needed

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
int currPlayerIndex;

int cols; // number of columns
int rows; // number of rows

int currturn;
int totalturns;

struct Coordinates
{
    int x, y;
};

struct Penguin
{
    bool placed;
    struct Coordinates coords;
};

struct Player
{
    bool blocked;
    int score;
    struct Penguin *penguins;
    struct Movement *movements;
    int numOfMovements;
    bool isHuman;
};
struct Player *players;
int **board;

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

void ReadArgs(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("No parameters passed! This version of program was compiled for batch mode and requires command line parameters.\n\n");
        exit(0);
    }
    currPlayerIndex = atoi(argv[0]);
    if (strcmp(argv[1], "phase=placement") == 0) // 0 means the strings are equal
    {
        phase = placementPhase;
    }
    else if (strcmp(argv[1], "phase=movement") == 0)
    {
        phase = movementPhase;
    }
    else
    {
        printf("Wrong input (first parameter invalid)\n");
        exit(0);
    }
    if (phase == placementPhase)
    {
        if (strncmp(argv[2], "penguins=", 9) == 0) // if first 9 letters are equal to "penguins="
        {
            numOfPenguins = atoi(&argv[2][9]); // convert numbers after "=" to int and assign it to numOfPenguins
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
}

int ReadIntWithMessage(char *message)
{
    int number;
    printf("%s", message);

    fseek(stdin, 0, SEEK_END);    // clears stdin
    while (!scanf("%d", &number)) // while scanf returned 0 (errror) execute loop
    {
        printf("Invalid input! Type an integer: ");
        fseek(stdin, 0, SEEK_END);
    }
    return number;
}

enum Direction ReadDirectionWithMessage(char *message)
{
    char input[3] = {' ', ' ', '\0'};
    printf(message);
    bool failed = false;
    do
    {
        fseek(stdin, 0, SEEK_END); // clears stdin
        fgets(input, sizeof(input), stdin);

        if (input[0] == 'L' && input[1] == '\n' || input[0] == 'l' && input[1] == '\n')
            return left;
        if (input[0] == 'L' && input[1] == 'U' || input[0] == 'l' && input[1] == 'u')
            return leftUp;
        if (input[0] == 'L' && input[1] == 'D' || input[0] == 'l' && input[1] == 'd')
            return leftDown;
        if (input[0] == 'R' && input[1] == '\n' || input[0] == 'r' && input[1] == '\n')
            return right;
        if (input[0] == 'R' && input[1] == 'U' || input[0] == 'r' && input[1] == 'u')
            return rightUp;
        if (input[0] == 'R' && input[1] == 'D' || input[0] == 'r' && input[1] == 'd')
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
    if (coords.x >= cols || coords.y >= rows || coords.x < 0 || coords.y < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool IsFieldValid(struct Coordinates coords) // checks if a field is valid to move on
{
    if (IsFieldInScope(coords))
    {
        if (board[coords.x][coords.y] == 0 || board[coords.x][coords.y] > 3)
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

bool IsPossibleToPlacePenguin()
{
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            if (board[j][i] == 1)
            {
                return true;
            }
        }
    }
    return false;
}

int TakeNextPenguinIndex(struct Penguin *penguins)
{
    int penguinIndex = 0;
    while (penguins[penguinIndex].placed)
    {
        penguinIndex++;
        if (penguinIndex > numOfPenguins)
        {
            puts("Error of TakeNextPenguinIndex function, there are no more penguins left! Maybe phase should be 'movement'?");
            exit(0);
        }
    }
    return penguinIndex;
}

void PrintBoard()
{
    int c, r;
    printf("\n");

    for (r = 0; r < rows; r++)
    {
        if (r % 2 == 1)
            printf(" ");
        for (c = 0; c < cols; c++)
        {
            printf("%d", board[c][r]);
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

    switch (direction)
    {
    case right:
        new_coords.x = curr.x + 1;
        break;
    case left:
        new_coords.x = curr.x - 1;
        break;
    case rightUp:
        if (curr.y % 2 == 0)
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
        if (curr.y % 2 == 0) // if current column is even
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
        if (curr.y % 2 == 0)
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
        if (curr.y % 2 == 0)
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

void SetPenguinPosition(struct Penguin *penguin, int number, struct Coordinates coords) // number is (index of penguin) + (number of penguins for every player) * (index of current player)
{
    board[coords.x][coords.y] = 3 + 1 + number; // place penguin on new field
    penguin->coords = coords;
    penguin->placed = true;
}

void UpdatePenguinPosition(struct Penguin *penguin, int number, struct Coordinates coords) 
{
    board[penguin->coords.x][penguin->coords.y] = 0; // old floe disappears
    board[coords.x][coords.y] = 3 + 1 + number;      // place penguin on new field
    penguin->coords = coords;
}

bool TryMovement(struct Coordinates startingPoint, struct MovementInSteps movement, struct Coordinates *new_coords)
{
    *new_coords = startingPoint;
    while (movement.jumps > 0)
    {
        *new_coords = GetNeighbouringCoords(*new_coords, movement.direction);
        if (!IsFieldValid(*new_coords))
        {
            return false;
        }
        movement.jumps--;
    }
    return true;
}

struct Movement ReadPlacement(struct Penguin *penguins)
{
    struct Movement placement;
    placement.penguinIndex = TakeNextPenguinIndex(penguins);
    bool success = false;
    while (!success)
    {
        placement.coords.x = ReadIntWithMessage("Type initial penguin x: ") - 1; // board[0][0] is 1,1 in game
        placement.coords.y = ReadIntWithMessage("Type initial penguin y: ") - 1;
        if (IsFieldInScope(placement.coords))
        {
            if (board[placement.coords.x][placement.coords.y] != 1)
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

int CountMovementsFromLocation(struct Coordinates initPos)
{
    int num = 0;
    enum Direction direction;

    for (direction = 0; direction < 6; direction++) //  6 directions
    {
        struct Coordinates new_coords = initPos;
        while (true)
        {
            new_coords = GetNeighbouringCoords(new_coords, direction);
            if (IsFieldValid(new_coords))
            {
                num++;
            }
            else
            {
                break;
            }
        }
    }
    return num;
}

int CountMovementsOfPlayer(struct Penguin *penguins)
{
    int i;
    int num = 0;
    for (i = 0; i < numOfPenguins; i++)
    {
        if (penguins[i].placed)
        {
            num += CountMovementsFromLocation(penguins[i].coords);
        }
    }
    return num;
}

void FindAllMovementsOfPlayer(struct Player *player) // fills the player's array of possible movements
{
    int counter = 0;
    if (player->movements != NULL)
    {
        free(player->movements);
    }
    player->movements = (struct Movement *)malloc(SIZE * sizeof(struct Movement));
    int penguinIndex = 0;
    enum Direction direction = 0;
    for (penguinIndex = 0; penguinIndex < numOfPenguins; penguinIndex++)
    {
        for (direction = 0; direction < 6; direction++) //  6 directions
        {
            struct Coordinates new_coords = player->penguins[penguinIndex].coords;
            while (true)
            {
                new_coords = GetNeighbouringCoords(new_coords, direction);
                if (IsFieldValid(new_coords)) // if movement is valid add it to the array
                {
                    player->movements[counter].penguinIndex = penguinIndex;
                    player->movements[counter].coords = new_coords;
                    counter++;

                    if ((counter % SIZE) == 0) // allocate new memory if needed
                    {
                        player->movements = (struct Movement *)realloc(player->movements, (counter + SIZE) * sizeof(struct Movement));
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

struct Movement ChooseBestPlacement(struct Penguin *penguins) // Chooses location with maximum movements from it (including movements of other penguins to avoid blocking player's' own ones)
{
    int i, j, k;
    struct Movement placement;
    struct Coordinates bestloc = {0, 0};
    int bestlocscore = 0;
    int currscore = 0;

    if (!IsPossibleToPlacePenguin())
    {
        printf("There is not enough valid floes to place all penguins!\n");
        exit(0);
    }

    placement.penguinIndex = TakeNextPenguinIndex(penguins);

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            if (board[j][i] == 1)
            {
                placement.coords.x = j;
                placement.coords.y = i;

                board[j][i] = 0; // blocks field temporarly

                currscore = sqrt(CountMovementsFromLocation(placement.coords));
                /*for(k = 0; k < numOfPenguins; k++)
                {
                     currscore += sqrt(CountMovementsOfPlayer(penguins)); // Counts also other penguins' movements
                }*/
                //currscore = CountMovementsFromLocation(placement.coords);
                board[j][i] = 1;

                if (currscore > bestlocscore)
                {
                    bestlocscore = currscore;
                    bestloc = placement.coords;
                }
                if (currscore == bestlocscore)
                {
                    if (rand() % 2) //  Element of randomness in case there will be many fields with the same number of possile movements from it
                    {
                        bestlocscore = currscore;
                        bestloc = placement.coords;
                    }
                }
            }
        }
    }
    placement.coords = bestloc;
    return placement;
}

struct Movement ChooseBestMovement(struct Player *player)
{
    int i, k;
    struct Movement bestMov = player->movements[0];
    int bestscore = 0;
    int currscore = 0;
    for (i = 0; i < player->numOfMovements; i++)
    {
        if (board[player->movements[i].coords.x][player->movements[i].coords.y] > board[bestMov.coords.x][bestMov.coords.y])
        {
            bestMov = player->movements[i];
        }
        if (board[player->movements[i].coords.x][player->movements[i].coords.y] == board[bestMov.coords.x][bestMov.coords.y])
        {
            int var = board[player->movements[i].coords.x][player->movements[i].coords.y];
            board[player->movements[i].coords.x][player->movements[i].coords.y] = 0;
            //currscore = sqrt(CountMovementsFromLocation(player->movements[i].coords)) + sqrt(CountMovementsOfPlayer(player->penguins));
            //currscore = CountMovementsFromLocation(player->movements[i].coords) + CountMovementsOfPlayer(player->penguins);
            currscore = CountMovementsFromLocation(player->movements[i].coords);
            board[player->movements[i].coords.x][player->movements[i].coords.y] = var;

            if (currscore > bestscore)
            {
                bestMov = player->movements[i];
                bestscore = currscore;
            }
        }
    }
    return bestMov;
}

/*struct Movement ChooseBestMovement(struct Movement* movements, int numOfMovements)
{
    int i;
    struct Movement bestMov = movements[0];
    for(i = 0; i < numOfMovements; i++)
    {
        if(board[movements[i].coords.x][movements[i].coords.y] > board[bestMov.coords.x][bestMov.coords.y])
        {
            bestMov = movements[i];
        }
        if(board[movements[i].coords.x][movements[i].coords.y] == board[bestMov.coords.x][bestMov.coords.y]) // similarly like in ChooseBestPlacement
        {
            if(rand() % 2) // 50% chance
            {
                bestMov = movements[i];
            }
        }
    }
    return bestMov;
}*/

struct MovementInSteps ReadMovement(int currPlayerIndex)
{
    struct MovementInSteps movement;
    int minPeng, maxPeng;

    minPeng = 4 + currPlayerIndex * numOfPenguins;
    maxPeng = minPeng + numOfPenguins - 1;

    printf("Type number of penguin (%d - %d): ", minPeng, maxPeng);
    movement.penguinIndex = ReadIntWithMessage("") - currPlayerIndex * numOfPenguins - 4; // penguin 7 is player[1].penguin[0]
    while (movement.penguinIndex >= numOfPenguins || movement.penguinIndex < 0)
    {
        movement.penguinIndex = ReadIntWithMessage("You don't have such penguin! Try again!: ") - currPlayerIndex * numOfPenguins - 4;
    }
    movement.direction = ReadDirectionWithMessage("Type direction of movement: ");
    movement.jumps = ReadIntWithMessage("Type number of jumps: ");
    return movement;
}

bool EndGame()
{
    if (currturn > totalturns)
        return true;
    return false;
}

struct Player *CreatePlayers(int numOfPlayers, int numOfPenguins) // creates players and gives them penguins
{
    int i, j;
    struct Player *players = (struct Player *)malloc(numOfPlayers * sizeof(struct Player));
    for (i = 0; i < numOfPlayers; i++)
    {
        players[i].penguins = (struct Penguin *)malloc(numOfPenguins * sizeof(struct Penguin));
        players[i].blocked = false;
        players[i].score = 0;
        players[i].numOfMovements = 0;
        players[i].movements = NULL;
        players[i].isHuman = false;
        for (j = 0; j < numOfPenguins; j++)
        {
            players[i].penguins[j].coords.x = -1;
            players[i].penguins[j].coords.y = -1;
            players[i].penguins[j].placed = false;
        }
    }
    return players;
}

int **CreateBoard(int boardSizeX, int boardSizeY)
{
    int x;
    int **board = NULL;
    board = (int **)malloc(boardSizeX * sizeof(int *));
    for (x = 0; x < boardSizeX; x++)
    {
        board[x] = (int *)malloc(boardSizeY * sizeof(int));
    }
    return board;
}

void RemovePlayers() // free memory
{
    int i;
    for (i = 0; i < numOfPlayers; i++)
    {
        free(players[i].penguins);
        free(players[i].movements);
    }
    free(players);
}

void RemoveBoard()
{
    int y;
    for (y = 0; y < rows; ++y)
    {
        free(board[y]);
    }
    free(board);
}

void SaveGame(char *fname)
{
    int i, j;
    FILE *fPointer = NULL;
    fPointer = fopen(fname, "w");
    if (fPointer == NULL)
    {
        printf("Cannot open file! \n");
        exit(0);
    }

    fprintf(fPointer, "%d %d\n", numOfPlayers, numOfPenguins);
    fprintf(fPointer, "%d %d\n", cols, rows);

    fprintf(fPointer, "\n");
    for (i = 0; i < rows; i++)
    {
        if (i % 2 == 1)
            fprintf(fPointer, " ");
        for (j = 0; j < cols; j++)
        {
            fprintf(fPointer, "%d ", board[j][i]);
        }
        fprintf(fPointer, "\n");
    }
    fprintf(fPointer, "\n");

    fprintf(fPointer, "%d/%d", currturn, totalturns);
    fprintf(fPointer, "\n");
    for (i = 0; i < numOfPlayers; i++)
    {
        fprintf(fPointer, "%d ", players[i].score);
    }
    fclose(fPointer);
}

void InitGame(char *fname)
{
    int i, j;
    FILE *fPointer = NULL;
    fPointer = fopen(fname, "r");
    if (fPointer == NULL)
    {
        printf("No input file found! \n");
        getch();
        exit(0);
    }

    fscanf(fPointer, "%d %d\n", &numOfPlayers, &numOfPenguins);
    fscanf(fPointer, "%d %d\n", &cols, &rows);

    players = CreatePlayers(numOfPlayers, numOfPenguins);
    board = CreateBoard(cols, rows);

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            fscanf(fPointer, "%d ", &board[j][i]);
            if (board[j][i] > 3)
            {
                int index = board[j][i] - currPlayerIndex * numOfPenguins - 4;
                players[currPlayerIndex].penguins[index].placed = true;
                players[currPlayerIndex].penguins[index].coords.x = j;
                players[currPlayerIndex].penguins[index].coords.y = i;
            }
        }
    }
    fscanf(fPointer, "%d/%d", &currturn, &totalturns);

    for (i = 0; i < numOfPlayers; i++)
    {
        fscanf(fPointer, "%d ", &players[i].score);
    }
    fclose(fPointer);
}

void PrintScoreTable()
{
    int i;

    printf("\nSCORES\n\n");

    for (i = 0; i < numOfPlayers; i++)
    {
        printf("Player %d: %d\n", i + 1, players[i].score);
    }
    printf("\n");
}

void PrintInitialGameState()
{
    printf("Number of players is: %d\n", numOfPlayers);
    printf("Number of penguins: %d\n", numOfPenguins);
}

int main(int argc, char *argv[])
{
//Batch mode
#ifndef INTERACTIVE
    ReadArgs(argc, argv);
    InitGame(inputboardfile);
    //PrintBoard();
    if (phase == placementPhase)
    {
        struct Movement placement = ChooseBestPlacement(players[currPlayerIndex].penguins);
        players[currPlayerIndex].score += board[placement.coords.x][placement.coords.y];
        SetPenguinPosition(&players[currPlayerIndex].penguins[placement.penguinIndex], numOfPenguins * currPlayerIndex + placement.penguinIndex, placement.coords);
    }
    else
    {
        players[currPlayerIndex].numOfMovements = CountMovementsOfPlayer(players[currPlayerIndex].penguins);

        if (players[currPlayerIndex].numOfMovements > 0) // if player is not blocked
        {
            FindAllMovementsOfPlayer(&players[currPlayerIndex]);
            struct Movement movement = ChooseBestMovement(&players[currPlayerIndex]);
            players[currPlayerIndex].score += board[movement.coords.x][movement.coords.y];
            UpdatePenguinPosition(&players[currPlayerIndex].penguins[movement.penguinIndex], numOfPenguins * currPlayerIndex + movement.penguinIndex, movement.coords); // tab with penguin by reference, index of penguin, new coordinates
            //PrintBoard();
        }
        if (currPlayerIndex == numOfPlayers - 1)
        {
            currturn++;
        }
    }
    SaveGame(outputboardfile);
    if (currturn > totalturns)
    {
        printf("\nAll turns passed.\n");
        PrintScoreTable();
        printf("End of the game.\n");
        return 0;
    }
#else
    // Interactive mode
    int i;
    int counter = 0;
    InitGame("board1.txt");
    phase = placementPhase;

    for (i = 0; i < numOfPlayers; i++)
    {
        printf("Do you want player %d to be played by computer? ", i + 1);
        if (ReadIntWithMessage("(type 0 for no or any other number for yes): "))
        {
            players[i].isHuman = false;
        }
        else
        {
            players[i].isHuman = true;
        }
    }
    printf("\n");

    PrintInitialGameState(); // Prints num of players and penguins for each player
    PrintBoard();

    while (!EndGame())
    {
        int currPlayerIndex;
        for (currPlayerIndex = 0; currPlayerIndex < numOfPlayers; currPlayerIndex++)
        {
            if (phase == placementPhase)
            {
                struct Movement placement;
                printf("Player %d: \n", currPlayerIndex + 1);

                if (players[currPlayerIndex].isHuman)
                {
                    placement = ReadPlacement(players[currPlayerIndex].penguins);
                }
                else
                {
                    placement = ChooseBestPlacement(players[currPlayerIndex].penguins);
                }

                players[currPlayerIndex].score += board[placement.coords.x][placement.coords.y];
                SetPenguinPosition(&players[currPlayerIndex].penguins[placement.penguinIndex], numOfPenguins * currPlayerIndex + placement.penguinIndex, placement.coords);
                PrintScoreTable();
                PrintBoard();
            }
            else
            {
                if (!players[currPlayerIndex].blocked)
                {
                    printf("Turn %d: \n\n", currturn);
                    printf("Player %d: \n", currPlayerIndex + 1);
                    players[currPlayerIndex].numOfMovements = CountMovementsOfPlayer(players[currPlayerIndex].penguins);
                    if (players[currPlayerIndex].numOfMovements == 0)
                    {
                        printf("Player %d is blocked!\n\n", currPlayerIndex + 1);
                        players[currPlayerIndex].blocked = true;
                        break;
                    }
                    if (!players[currPlayerIndex].isHuman)
                    {
                        struct Movement movement;
                        FindAllMovementsOfPlayer(&players[currPlayerIndex]);
                        movement = ChooseBestMovement(&players[currPlayerIndex]);
                        players[currPlayerIndex].score += board[movement.coords.x][movement.coords.y];
                        UpdatePenguinPosition(&players[currPlayerIndex].penguins[movement.penguinIndex], numOfPenguins * currPlayerIndex + movement.penguinIndex, movement.coords);
                        free(players[currPlayerIndex].movements);
                    }
                    else
                    {
                        bool success = false;
                        while (!success)
                        {
                            struct Coordinates new_coords;
                            struct MovementInSteps movement = ReadMovement(currPlayerIndex);

                            if (TryMovement(players[currPlayerIndex].penguins[movement.penguinIndex].coords, movement, &new_coords))
                            {
                                players[currPlayerIndex].score += board[new_coords.x][new_coords.y];
                                UpdatePenguinPosition(&players[currPlayerIndex].penguins[movement.penguinIndex], numOfPenguins * currPlayerIndex + movement.penguinIndex, new_coords); // tab with penguin by reference, index of penguin, new coordinates
                                success = true;
                            }
                            else
                            {
                                puts("Invalid movment! Try again.");
                            }
                        }
                    }
                    PrintScoreTable();
                    PrintBoard();
                }
            }
        }
        if (phase == movementPhase)
        {
            currturn++;
        }
        if (counter >= numOfPenguins - 1) // if all penguins are placed switch to the movement phase
        {
            phase = movementPhase;
        }
        counter++;
    }
    printf("\nEnd of the game.\n");
    PrintScoreTable();
    printf("Type any key to exit.\n");
    getch();
#endif
    //RemovePlayers(); // free memory
    //RemoveBoard();
    return 0;
}
