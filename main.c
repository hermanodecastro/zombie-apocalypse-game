// Hermano de Castro Pinheiro
// 25/05/2021

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BROWN "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define LIGHT_GREY "\033[0;37m"
#define BLACK_GREY "\033[1;30m"
#define LIGHT_RED "\033[1;31m"
#define LIGHT_GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define LIGHT_BLUE "\033[1;34m"
#define LIGHT_PURPLE "\033[1;35m"
#define LIGHT_CYAN "\033[1;36m"
#define WHITE "\033[1;37m"
#define RESET_COLOR "\033[0m"

uint8_t GLOBAL_ZOMBIES_LEFT = 20;
uint8_t GLOBAL_BULLETS_AMOUNT = 0;
uint8_t GLOBAL_DEVORED_BY_ZOMBIE = 0;
uint8_t GLOBAL_MANAGED_TO_ESCAPE = 0; 
uint8_t GLOBAL_MINIMUM_DISTANCE_TO_CHASE_RICK = 3; 

enum createWorldConfig
{
    FILLED, EMPTY
};

enum keyBoardArrow
{
    UP = 'W', 
    DOWN = 'S', 
    LEFT = 'A', 
    RIGHT = 'D'
};

enum objects
{
    BULLET = 'B',
    CAR = 'C',
    EMPTY_SPACE = '.',
    EXIT = 'E',
    RICK = 'R',
    STONE = 'S',
    TREE = 'T',
    ZOMBIE = 'Z',
    NEW_ZOMBIE_POSITION = 'N'
};

char **createWorld(uint8_t lines, uint8_t columns, enum createWorldConfig config);
uint8_t *calculatesTheShortestPathToRick(char **world, uint8_t lines, uint8_t columns, uint8_t zombieLine, uint8_t zombieColumn, uint8_t rickLine, uint8_t rickColumn);
uint8_t distanceToTarget(uint8_t selfLine, uint8_t selfColumn, uint8_t targetLine, uint8_t targetColumn);
uint8_t drawPosition(uint8_t range);
uint8_t exitIsBlocked(char **world, uint8_t lines, uint8_t columns);
uint8_t *findRickPosition(char **world, uint8_t lines, uint8_t columns); 
uint8_t isNotOffTheEdge(int8_t lines, int8_t columns, int8_t position, enum keyBoardArrow key);
uint8_t zombieIsCloseToRick(uint8_t rickLine, uint8_t rickColumn, uint8_t zombieLine, uint8_t zombieColumn);
void clear();
void drawBulletPosition(char **world, uint8_t range);
void drawCarPosition(char **world, uint8_t range);
void drawExitPosition(char **world, uint8_t range);
void drawRickPosition(char** world, uint8_t range);
void drawStonePosition(char **world, uint8_t range);
void drawTreePosition(char **world, uint8_t range);
void drawZombiePosition(char **world, uint8_t range);
void freeWorld(char **world, uint8_t lines);
void makeMoviment(char **world, uint8_t lines, uint8_t columns, enum keyBoardArrow key); 
void runAllTests();
void setupWorld(char** world, uint8_t range);
void showWorld(char **world, uint8_t lines, uint8_t columns);
void statistics();
void testCalculatesTheShortestPathToRick();
void testDistanceToTarget();
void testExitIsBlockedShouldReturnFalse();
void testExitIsBlockedShouldReturnTrue();
void testZombieIsCloseToRickShouldReturnTrue();
void testZombieIsCloseToRickShouldReturnFalse();
void zombieMoviment(char **world, uint8_t lines, uint8_t columns);


int main() 
{   
    // LINUX
    // compiling: gcc main.c -o main -lm 
    // ((lm) -> The Math library must be linked during compilation)
    
    // WINDOWS
    // gcc main.c -o main  

    srand(time(NULL));

    runAllTests();
    
    uint8_t lines = 30;
    uint8_t columns = 30;
    uint8_t range = 30;

    char **world = createWorld(lines, columns, FILLED);

    setupWorld(world, range);

    uint8_t finished = 0;

    char key;
    
    while(!finished)
    {  
        clear();
        statistics();
        showWorld(world, lines, columns);
        printf(RESET_COLOR);

        scanf(" %c", &key);
        
        switch (toupper(key))
        {
            case UP:
                makeMoviment(world, lines, columns, UP);
                break;

            case DOWN:
                makeMoviment(world, lines, columns, DOWN);
                break;

            case LEFT:
                makeMoviment(world, lines, columns, LEFT);
                break;

            case RIGHT:
                makeMoviment(world, lines, columns, RIGHT);
                break;
            
            default:
                break;
        }

        if(GLOBAL_DEVORED_BY_ZOMBIE)
        {
            clear();
            statistics();
            showWorld(world, lines, columns);
            printf(RED);
            printf("You was devored by the zombies\n");
            finished = 1;
        }
        else if(GLOBAL_MANAGED_TO_ESCAPE)
        {
            clear();
            statistics();
            showWorld(world, lines, columns);
            printf(WHITE);
            printf("Wow!! You managed to escape\n");
            finished = 1;
        }
    }

    freeWorld(world, lines);

    return 0;
}

char **createWorld(uint8_t lines, uint8_t columns, enum createWorldConfig config)
{
    char **world = (char**)malloc(lines * sizeof(char*));

    for(uint8_t i = 0; i < lines; i++)
    {
        world[i] = (char*)malloc(columns * sizeof(char));
    }

    if(config == FILLED)
    {
        for(uint8_t i = 0; i < lines; i++)
        {
            for(uint8_t j = 0; j < columns; j++)
            {
                world[i][j] = EMPTY_SPACE;
            }
        }
        return world;
    }
    
    return world;
}

uint8_t *calculatesTheShortestPathToRick(char **world, uint8_t lines, uint8_t columns, uint8_t zombieLine, uint8_t zombieColumn, uint8_t rickLine, uint8_t rickColumn)
{
    //   .
    // . Z .      R
    //   .
    //

    // (1, 2) shortest path

    //   .
    // . . Z      R
    //   .
    //

    int16_t adjacencyList[4] = {-1, -1, -1, -1};

    // the verification isNotOffTheEdge(int8_t lines, int8_t columns, int8_t position, enum keyBoardArrow key) is necessary to avoid segmentation fault errors.
    // #Example:
    // zombieLine = 0;
    // zombieColumn = 1;
    // world[zombieLine - 1][zombieColumn] is equivalent to world[-1][0] and the compiler will throw an error.

    if(isNotOffTheEdge(lines, columns, (zombieLine - 1), UP))
    {   
        if(world[zombieLine - 1][zombieColumn] == EMPTY_SPACE || world[zombieLine - 1][zombieColumn] == RICK)
        {
            uint16_t up = sqrt(pow(((zombieLine - 1) - rickLine), 2) + pow((zombieColumn - rickColumn), 2));
            adjacencyList[0] = up;
        }
    }
    if(isNotOffTheEdge(lines, columns, (zombieLine + 1), DOWN))
    {
        if(world[zombieLine + 1][zombieColumn] == EMPTY_SPACE || world[zombieLine + 1][zombieColumn] == RICK)
        {
            uint16_t down = sqrt(pow(((zombieLine + 1) - rickLine), 2) + pow((zombieColumn - rickColumn), 2));
            adjacencyList[1] = down;
        }
    }
    if(isNotOffTheEdge(lines, columns, (zombieColumn - 1), LEFT))
    {
        if(world[zombieLine][zombieColumn - 1] == EMPTY_SPACE || world[zombieLine][zombieColumn - 1] == RICK)
        {
            uint16_t left = sqrt(pow((zombieLine - rickLine), 2) + pow(((zombieColumn - 1) - rickColumn), 2));
            adjacencyList[2] = left;
        }
    }
    if(isNotOffTheEdge(lines, columns, (zombieColumn + 1), RIGHT))
    {
        if(world[zombieLine][zombieColumn + 1] == EMPTY_SPACE || world[zombieLine][zombieColumn + 1] == RICK)
        {
            uint16_t right = sqrt(pow((zombieLine - rickLine), 2) + pow(((zombieColumn + 1) - rickColumn), 2));
            adjacencyList[3] = right;
        }
    }

    uint8_t zombieIsBlocked = 1;
    uint8_t firstNonNegativeIndex;
    uint8_t shortestIndex;
    int16_t shortestElement;


    for(uint8_t i = 0; i < 4; i++)
    {   
        // if it doesn't enter the "if" at any time, it means that the zombie is blocked.
        if(adjacencyList[i] != -1)
        {   
            // firstNonNegative receives the first non negative element;
            firstNonNegativeIndex = i;
            shortestElement = adjacencyList[i];
            zombieIsBlocked = 0;
            break;
        }
    }

    uint8_t *position = (uint8_t*) malloc(2 * sizeof(uint8_t));

    // the 4 adjacent positions with a value of -1, means that the zombie is blocked, 
    // he has nowhere to move, so he will maintain his current position.
    if(zombieIsBlocked)
    {
        position[0] = zombieLine;
        position[1] = zombieColumn;

        return position;
    }

    for(uint8_t i = firstNonNegativeIndex; i < 4; i++)
    {
       if(adjacencyList[i] <= shortestElement && adjacencyList[i] != -1)
       {
           shortestElement = adjacencyList[i];
           shortestIndex = i;
       }
    }

    // 0: up
    // 1: down
    // 2: left
    // 3: right
    switch (shortestIndex)
    {
        case 0:
            position[0] = zombieLine - 1;
            position[1] = zombieColumn;
            return position;
            break;

        case 1:
            position[0] = zombieLine + 1;
            position[1] = zombieColumn;
            return position;
            break;

        case 2:
            position[0] = zombieLine;
            position[1] = zombieColumn - 1;
            return position;
            break;

        case 3:
            position[0] = zombieLine;
            position[1] = zombieColumn + 1;
            return position;
            break;
    }
}

uint8_t distanceToTarget(uint8_t selfLine, uint8_t selfColumn, uint8_t targetLine, uint8_t targetColumn)
{
    return sqrt(pow((selfLine - targetLine), 2) + pow((selfColumn - targetColumn), 2));
}

uint8_t drawPosition(uint8_t range)
{
    return rand() % range;
}

uint8_t exitIsBlocked(char **world, uint8_t lines, uint8_t columns)
{
    if
    (
        world[lines + 1][columns] != EMPTY_SPACE &&
        world[lines + 1][columns + 1] != EMPTY_SPACE &&
        world[lines + 1][columns - 1] != EMPTY_SPACE &&
        world[lines - 1][columns] != EMPTY_SPACE &&
        world[lines - 1][columns + 1] != EMPTY_SPACE &&
        world[lines - 1][columns - 1] != EMPTY_SPACE &&
        world[lines][columns + 1] != EMPTY_SPACE &&
        world[lines][columns - 1] != EMPTY_SPACE
    )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t *findRickPosition(char **world, uint8_t lines, uint8_t columns)
{
    uint8_t *rickPosition = (uint8_t*) malloc(2 * sizeof(uint8_t));

    for(uint8_t i = 0; i < lines; i++)
    {
        for(uint8_t j = 0; j < columns; j++)
        {
            if(world[i][j] == RICK)
            {
                rickPosition[0] = i;
                rickPosition[1] = j;

                return rickPosition;
            }
        }
    }
}

uint8_t isNotOffTheEdge(int8_t lines, int8_t columns, int8_t position, enum keyBoardArrow key)
{
    if(key == UP && position >= 0)
    {
        return 1;
    }
    else if(key == DOWN && position < lines)
    {
        return 1;
    }
    else if(key == LEFT && position >= 0)
    {
        return 1;
    }
    else if(key == RIGHT && position < columns)
    {
        return 1;
    }
    else 
    {   
        return 0;
    }
}

uint8_t zombieIsCloseToRick(uint8_t rickLine, uint8_t rickColumn, uint8_t zombieLine, uint8_t zombieColumn)
{
    return distanceToTarget(rickLine, rickColumn, zombieLine, zombieColumn) <= GLOBAL_MINIMUM_DISTANCE_TO_CHASE_RICK;
}

void clear()
{
    #ifdef __linux__
        system("clear");
    #else
        system("cls");
    #endif
}

void drawBulletPosition(char **world, uint8_t range)
{
    uint8_t count = 0;

    while(count < 4)
    {
        uint8_t line = drawPosition(range);
        uint8_t column = drawPosition(range);

        if(world[line][column] == EMPTY_SPACE)
        {
            world[line][column] = BULLET;
            count++;
        }
    }
}

void drawCarPosition(char **world, uint8_t range)
{
    uint8_t count = 0;

    while(count < 4)
    {
        uint8_t line = drawPosition(range);
        uint8_t column = drawPosition(range);

        if(world[line][column] == EMPTY_SPACE)
        {
            world[line][column] = CAR;
            count++;
        }
    }
}

void drawExitPosition(char **world, uint8_t range)
{
    uint8_t count = 0;

    while(count < 1)
    {
        uint8_t line = drawPosition(range);
        uint8_t column = drawPosition(range);

        if(world[line][column] == EMPTY_SPACE)
        {
            if(exitIsBlocked(world, line, column)) 
            {
                printf(RED);
                printf("The exit is blocked, you won't escape");
                exit(1);
            }
            else
            {
                world[line][column] = EXIT;
                count++;
            } 
        }
    }
}

void drawRickPosition(char** world, uint8_t range)
{
    uint8_t count = 0;

    while(count < 1)
    {
        uint8_t line = drawPosition(range);
        uint8_t column = drawPosition(range);

        if(world[line][column] == EMPTY_SPACE)
        {
            world[line][column] = RICK;
            count++;
        }
    }
}

void drawStonePosition(char **world, uint8_t range)
{
    uint8_t count = 0;

    while(count < 4)
    {
        uint8_t line = drawPosition(range);
        uint8_t column = drawPosition(range);

        if(world[line][column] == EMPTY_SPACE)
        {
            world[line][column] = STONE;
            count++;
        }
    }
}

void drawTreePosition(char **world, uint8_t range)
{
    uint8_t count = 0;

    while(count < 7)
    {
        uint8_t line = drawPosition(range);
        uint8_t column = drawPosition(range);

        if(world[line][column] == EMPTY_SPACE)
        {
            world[line][column] = TREE;
            count++;
        }
    }
}

void drawZombiePosition(char **world, uint8_t range)
{
    uint8_t count = 0;

    while(count < 15)
    {
        uint8_t line = drawPosition(range);
        uint8_t column = drawPosition(range);

        if(world[line][column] == EMPTY_SPACE)
        {
            world[line][column] = ZOMBIE;
            count++;
        }
    }
}

void freeWorld(char **world, uint8_t lines)
{
    for(uint8_t i = 0; i < lines; i++)
    {
        free(world[i]);
    }
    free(world);
}

void makeMoviment(char **world, uint8_t lines, uint8_t columns, enum keyBoardArrow key)
{
    uint8_t *rickPosition = findRickPosition(world, lines, columns);

    switch (key)
    {
        case UP:
            if(isNotOffTheEdge(lines, columns, (rickPosition[0] - 1), UP))
            {
                if(world[rickPosition[0] - 1][rickPosition[1]] == EMPTY_SPACE)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0] - 1][rickPosition[1]] = RICK;
                }
                else if(world[rickPosition[0] - 1][rickPosition[1]] == BULLET)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0] - 1][rickPosition[1]] = RICK;
                    GLOBAL_BULLETS_AMOUNT++;
                }
                else if(world[rickPosition[0] - 1][rickPosition[1]] == ZOMBIE)
                {
                    if(GLOBAL_BULLETS_AMOUNT > 0)
                    {
                        world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                        world[rickPosition[0] - 1][rickPosition[1]] = RICK;
                        GLOBAL_BULLETS_AMOUNT--;
                        GLOBAL_ZOMBIES_LEFT--;
                    } 
                    else
                    {
                        world[rickPosition[0]][rickPosition[1]] = ZOMBIE;
                        world[rickPosition[0] - 1][rickPosition[1]] = EMPTY_SPACE;
                        GLOBAL_DEVORED_BY_ZOMBIE = 1;
                    }
                }
                else if(world[rickPosition[0] - 1][rickPosition[1]] == EXIT)
                {   
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0] - 1][rickPosition[1]] = RICK;
                    GLOBAL_MANAGED_TO_ESCAPE = 1;
                }

                if(!GLOBAL_DEVORED_BY_ZOMBIE && !GLOBAL_MANAGED_TO_ESCAPE)
                {
                    clear();
                    statistics();
                    showWorld(world, lines, columns);
                    printf(RESET_COLOR);
                    zombieMoviment(world, lines, columns);
                }
            }

            break;

        case DOWN:
            if(isNotOffTheEdge(lines, columns, (rickPosition[0] + 1), DOWN))
            {
                 if(world[rickPosition[0] + 1][rickPosition[1]] == EMPTY_SPACE)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0] + 1][rickPosition[1]] = RICK;
                }
                else if(world[rickPosition[0] + 1][rickPosition[1]] == BULLET)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0] + 1][rickPosition[1]] = RICK;
                    GLOBAL_BULLETS_AMOUNT++;
                }
                else if(world[rickPosition[0] + 1][rickPosition[1]] == ZOMBIE)
                {
                    if(GLOBAL_BULLETS_AMOUNT > 0)
                    {
                        world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                        world[rickPosition[0] + 1][rickPosition[1]] = RICK;
                        GLOBAL_BULLETS_AMOUNT--;
                        GLOBAL_ZOMBIES_LEFT--;
                    } 
                    else
                    {
                        world[rickPosition[0]][rickPosition[1]] = ZOMBIE;
                        world[rickPosition[0] + 1][rickPosition[1]] = EMPTY_SPACE;
                        GLOBAL_DEVORED_BY_ZOMBIE = 1;
                    }
                }
                else if(world[rickPosition[0] + 1][rickPosition[1]] == EXIT)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0] + 1][rickPosition[1]] = RICK;;
                    GLOBAL_MANAGED_TO_ESCAPE = 1;
                }
                
                if(!GLOBAL_DEVORED_BY_ZOMBIE && !GLOBAL_MANAGED_TO_ESCAPE)
                {
                    clear();
                    statistics();
                    showWorld(world, lines, columns);
                    printf(RESET_COLOR);
                    zombieMoviment(world, lines, columns);
                }
            }

            break;

        case LEFT:
            if(isNotOffTheEdge(lines, columns, (rickPosition[1] - 1), LEFT))
            {
                 if(world[rickPosition[0]][rickPosition[1] - 1] == EMPTY_SPACE)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0]][rickPosition[1] - 1] = RICK;
                }
                else if(world[rickPosition[0]][rickPosition[1] - 1] == BULLET)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0]][rickPosition[1] - 1] = RICK;
                    GLOBAL_BULLETS_AMOUNT++;
                }
                else if(world[rickPosition[0]][rickPosition[1] - 1] == ZOMBIE)
                {
                    if(GLOBAL_BULLETS_AMOUNT > 0)
                    {
                        world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                        world[rickPosition[0]][rickPosition[1] - 1] = RICK;
                        GLOBAL_BULLETS_AMOUNT--;
                        GLOBAL_ZOMBIES_LEFT--;
                    } 
                    else
                    {
                        world[rickPosition[0]][rickPosition[1]] = ZOMBIE;
                        world[rickPosition[0]][rickPosition[1] - 1] = EMPTY_SPACE;
                        GLOBAL_DEVORED_BY_ZOMBIE = 1;
                    }
                }
                else if(world[rickPosition[0]][rickPosition[1] - 1] == EXIT)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0]][rickPosition[1] - 1] = RICK;;
                    GLOBAL_MANAGED_TO_ESCAPE = 1;
                }
                
                if(!GLOBAL_DEVORED_BY_ZOMBIE && !GLOBAL_MANAGED_TO_ESCAPE)
                {
                    clear();
                    statistics();
                    showWorld(world, lines, columns);
                    printf(RESET_COLOR);
                    zombieMoviment(world, lines, columns);
                }
            }
           
            break;

        case RIGHT:
            if(isNotOffTheEdge(lines, columns, (rickPosition[1] + 1), RIGHT))
            {
                if(world[rickPosition[0]][rickPosition[1] + 1] == EMPTY_SPACE)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0]][rickPosition[1] + 1] = RICK;
                }
                else if(world[rickPosition[0]][rickPosition[1] + 1] == BULLET)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0]][rickPosition[1] + 1] = RICK;
                    GLOBAL_BULLETS_AMOUNT++;
                }
                else if(world[rickPosition[0]][rickPosition[1] + 1] == ZOMBIE)
                {
                    if(GLOBAL_BULLETS_AMOUNT > 0)
                    {
                        world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                        world[rickPosition[0]][rickPosition[1] + 1] = RICK;
                        GLOBAL_BULLETS_AMOUNT--;
                        GLOBAL_ZOMBIES_LEFT--;
                    } 
                    else
                    {
                        world[rickPosition[0]][rickPosition[1]] = ZOMBIE;
                        world[rickPosition[0]][rickPosition[1] + 1] = EMPTY_SPACE;
                        GLOBAL_DEVORED_BY_ZOMBIE = 1;
                    }
                }
                else if(world[rickPosition[0]][rickPosition[1] + 1] == EXIT)
                {
                    world[rickPosition[0]][rickPosition[1]] = EMPTY_SPACE;
                    world[rickPosition[0]][rickPosition[1] + 1] = RICK;;
                    GLOBAL_MANAGED_TO_ESCAPE = 1;
                }

                if(!GLOBAL_DEVORED_BY_ZOMBIE && !GLOBAL_MANAGED_TO_ESCAPE)
                {
                    clear();
                    statistics();
                    showWorld(world, lines, columns);
                    printf(RESET_COLOR);
                    zombieMoviment(world, lines, columns);
                }
            }
            
            break;
        
        default:
            break;
    }

    free(rickPosition);
}

void setupWorld(char **world, uint8_t range)
{
    drawRickPosition(world, range);
    drawZombiePosition(world, range);
    drawCarPosition(world, range);
    drawTreePosition(world, range);
    drawStonePosition(world, range);
    drawExitPosition(world, range);
    drawBulletPosition(world, range);
}

void showWorld(char **world, uint8_t lines, uint8_t columns)
{
    for(uint8_t i = 0; i < lines; i++)
    {
        for(uint8_t j = 0; j < columns; j++)
        {
            if(world[i][j] == RICK)
            {
                printf(WHITE);
                printf("%2c", world[i][j]);
            }
            else if(world[i][j] == CAR || world[i][j] == TREE|| world[i][j] == STONE)
            {
                printf(YELLOW);
                printf("%2c", world[i][j]);
            }  
            else if(world[i][j] == ZOMBIE)
            {
                printf(RED);
                printf("%2c", world[i][j]);
            }
            else if(world[i][j] == EXIT)
            {
                printf(GREEN);
                printf("%2c", world[i][j]);
            }
            else if(world[i][j] == BULLET)
            {
                printf(LIGHT_CYAN);
                printf("%2c", world[i][j]);
            }
            else if(world[i][j] == EMPTY_SPACE)
            {
                printf(BLACK);
                printf("%2c", world[i][j]);
            }
            else if(world[i][j] == NEW_ZOMBIE_POSITION)
            {
                printf(BROWN);
                printf("%2c", world[i][j]);
            }
        }
        printf("\n");
    }
}

void statistics()
{
    printf("Zombies left: %u\n", GLOBAL_ZOMBIES_LEFT);
    printf("Bullet amount: %u\n\n", GLOBAL_BULLETS_AMOUNT);
}

void zombieMoviment(char **world, uint8_t lines, uint8_t columns)
{
    uint8_t *rickPosition = findRickPosition(world, lines, columns);
    char **zombieMotionMatrix = createWorld(lines, columns, FILLED);

    for(uint8_t i = 0; i < lines; i++)
    {
        for(uint8_t j = 0; j < columns; j++)
        {   
            if(world[i][j] == ZOMBIE)
            {
                if(zombieIsCloseToRick(rickPosition[0], rickPosition[1], i, j))
                {
                    // . . . . . . . .
                    // . . . . . . . .
                    // . . T . . . . .
                    // . . . . Z . . .
                    // . . . . . . . .
                    // . . R . . C . .
                    // . . . . . . . .

                    uint8_t *position = calculatesTheShortestPathToRick(world, lines, columns, i, j, rickPosition[0], rickPosition[1]);

                    // clear current zombie position
                    world[i][j] = EMPTY_SPACE;

                    if(world[position[0]][position[1]] == RICK)
                    {   
                        if(GLOBAL_BULLETS_AMOUNT > 0)
                        {
                            GLOBAL_BULLETS_AMOUNT--;
                            GLOBAL_ZOMBIES_LEFT--;
                            world[position[0]][position[1]] = RICK;    
                        }
                        else
                        {
                            GLOBAL_DEVORED_BY_ZOMBIE = 1;
                            world[position[0]][position[1]] = ZOMBIE; 
                        }
                    }
                    else
                    {
                        // saves all modified zombie positions in the matrix
                        zombieMotionMatrix[position[0]][position[1]] = NEW_ZOMBIE_POSITION;

                        // old position (1, 1)

                        // . . .
                        // . Z .
                        // . . .
                        
                        // current position (2, 1)

                        // . . .
                        // . . .
                        // . N .
                    }

                    free(position);
                }
            }
        }
    }

    for(uint8_t i = 0; i < lines; i++)
    {
        for(uint8_t j = 0; j < columns; j++)
        {   
            if(zombieMotionMatrix[i][j] == NEW_ZOMBIE_POSITION)
            {
                world[i][j] =  ZOMBIE;
            }
        }
    }

    freeWorld(zombieMotionMatrix, lines);
    free(rickPosition); 
}

void runAllTests()
{
    testCalculatesTheShortestPathToRick();
    testDistanceToTarget();
    testExitIsBlockedShouldReturnTrue();
    testExitIsBlockedShouldReturnFalse();
    testZombieIsCloseToRickShouldReturnTrue();
    testZombieIsCloseToRickShouldReturnFalse();
}

void testCalculatesTheShortestPathToRick()
{
    uint8_t lines = 6;
    uint8_t columns = 6;
    uint8_t rickLine = 1;
    uint8_t rickColumn = 0;
    uint8_t zombieLine = 1;
    uint8_t zombieColumn = 5;

    char **world = createWorld(lines, columns, EMPTY);

    // . . T . . .
    // R . . . Z .
    // . . . . . .
    // . . . . . .
    // . . . . . .
    // . . . . . .
    
    world[0][0] = EMPTY_SPACE;
    world[0][1] = EMPTY_SPACE;
    world[0][2] = TREE;
    world[0][3] = EMPTY_SPACE;
    world[0][4] = EMPTY_SPACE;
    world[0][5] = EMPTY_SPACE;
    world[1][0] = RICK;
    world[1][1] = EMPTY_SPACE;
    world[1][2] = EMPTY_SPACE;
    world[1][3] = EMPTY_SPACE;
    world[1][4] = EMPTY_SPACE;
    world[1][5] = ZOMBIE;
    world[2][0] = EMPTY_SPACE;
    world[2][1] = EMPTY_SPACE;
    world[2][2] = EMPTY_SPACE;
    world[2][3] = EMPTY_SPACE;
    world[2][4] = EMPTY_SPACE;
    world[2][5] = EMPTY_SPACE;
    world[3][0] = EMPTY_SPACE;
    world[3][1] = EMPTY_SPACE;
    world[3][2] = EMPTY_SPACE;
    world[3][3] = EMPTY_SPACE;
    world[3][4] = EMPTY_SPACE;
    world[3][5] = EMPTY_SPACE;
    world[4][0] = EMPTY_SPACE;
    world[4][1] = EMPTY_SPACE;
    world[4][2] = EMPTY_SPACE;
    world[4][3] = EMPTY_SPACE;
    world[4][4] = EMPTY_SPACE;
    world[4][5] = EMPTY_SPACE;
    world[5][0] = EMPTY_SPACE;
    world[5][1] = EMPTY_SPACE;
    world[5][2] = EMPTY_SPACE;
    world[5][3] = EMPTY_SPACE;
    world[5][4] = EMPTY_SPACE;
    world[5][5] = EMPTY_SPACE;

    uint8_t *position = calculatesTheShortestPathToRick(world, lines, columns, zombieLine, zombieColumn, rickLine, rickColumn);

    // o = target position (1, 4)

    // . . T . . .
    // R . . o Z .
    // . . . . . .
    // . . . . . .
    // . . . . . .
    // . . . . . .

    printf("%u %u\n", position[0], position[1]);

    if(position[0] == 1 && position[1] == 4)
    {
        assert(1);
    }
    else
    {
        assert(0);
    }

    free(position);
    freeWorld(world, lines);
}

void testDistanceToTarget()
{
 uint8_t lines = 6;
    uint8_t columns = 6;
    uint8_t rickLine = 1;
    uint8_t rickColumn = 0;
    uint8_t zombieLine = 1;
    uint8_t zombieColumn = 4;

    char **world = createWorld(lines, columns, EMPTY);

    // . . T . . .
    // R . . . Z .
    // . . . . . .
    // . . . . . .
    // . . . . . .
    // . . . . . .
    
    world[0][0] = EMPTY_SPACE;
    world[0][1] = EMPTY_SPACE;
    world[0][2] = TREE;
    world[0][3] = EMPTY_SPACE;
    world[0][4] = EMPTY_SPACE;
    world[0][5] = EMPTY_SPACE;
    world[1][0] = RICK;
    world[1][1] = EMPTY_SPACE;
    world[1][2] = EMPTY_SPACE;
    world[1][3] = EMPTY_SPACE;
    world[1][4] = ZOMBIE;
    world[1][5] = EMPTY_SPACE;
    world[2][0] = EMPTY_SPACE;
    world[2][1] = EMPTY_SPACE;
    world[2][2] = EMPTY_SPACE;
    world[2][3] = EMPTY_SPACE;
    world[2][4] = EMPTY_SPACE;
    world[2][5] = EMPTY_SPACE;
    world[3][0] = EMPTY_SPACE;
    world[3][1] = EMPTY_SPACE;
    world[3][2] = EMPTY_SPACE;
    world[3][3] = EMPTY_SPACE;
    world[3][4] = EMPTY_SPACE;
    world[3][5] = EMPTY_SPACE;
    world[4][0] = EMPTY_SPACE;
    world[4][1] = EMPTY_SPACE;
    world[4][2] = EMPTY_SPACE;
    world[4][3] = EMPTY_SPACE;
    world[4][4] = EMPTY_SPACE;
    world[4][5] = EMPTY_SPACE;
    world[5][0] = EMPTY_SPACE;
    world[5][1] = EMPTY_SPACE;
    world[5][2] = EMPTY_SPACE;
    world[5][3] = EMPTY_SPACE;
    world[5][4] = EMPTY_SPACE;
    world[5][5] = EMPTY_SPACE;

    if(distanceToTarget(zombieLine, zombieColumn, rickLine, rickColumn) == 4)
    {
        assert(1);
    }
    else
    {
        assert(0);
    }

    freeWorld(world, lines);   
}

void testExitIsBlockedShouldReturnFalse()
{
    uint8_t lines = 3;
    uint8_t columns = 3;

    char **world = createWorld(lines, columns, EMPTY);

    world[0][0] = EMPTY_SPACE;
    world[0][1] = TREE;
    world[0][2] = TREE;
    world[1][0] = TREE;
    world[1][1] = EXIT;
    world[1][2] = TREE;
    world[2][0] = TREE;
    world[2][1] = TREE;
    world[2][2] = TREE;

    if(exitIsBlocked(world, 1, 1))
    {
        assert(0);
    }
    else
    {
        assert(1);
    }


    freeWorld(world, lines);
}

void testExitIsBlockedShouldReturnTrue()
{
    uint8_t lines = 3;
    uint8_t columns = 3;

    char **world = createWorld(lines, columns, EMPTY);

    world[0][0] = TREE;
    world[0][1] = TREE;
    world[0][2] = TREE;
    world[1][0] = TREE;
    world[1][1] = EXIT;
    world[1][2] = TREE;
    world[2][0] = TREE;
    world[2][1] = TREE;
    world[2][2] = TREE;

    if(exitIsBlocked(world, 1, 1))
    {
        assert(1);
    }
    else
    {
        assert(0);
    }

    freeWorld(world, lines);
}

void testZombieIsCloseToRickShouldReturnTrue()
{
    uint8_t lines = 5;
    uint8_t columns = 5;
    uint8_t rickLine = 2;
    uint8_t rickColumn = 2;
    uint8_t zombieLine = 0;
    uint8_t zombieColumn = 0;

    char **world = createWorld(lines, columns, EMPTY);

    // Z . . . .
    // . . . . .
    // . . R . .
    // . . . . .
    // . . . . .
    
    world[0][0] = ZOMBIE;
    world[0][1] = EMPTY_SPACE;
    world[0][2] = EMPTY_SPACE;
    world[0][3] = EMPTY_SPACE;
    world[0][4] = EMPTY_SPACE;
    world[1][0] = EMPTY_SPACE;
    world[1][1] = EMPTY_SPACE;
    world[1][2] = EMPTY_SPACE;
    world[1][3] = EMPTY_SPACE;
    world[1][4] = EMPTY_SPACE;
    world[2][0] = EMPTY_SPACE;
    world[2][1] = EMPTY_SPACE;
    world[2][2] = RICK;
    world[2][3] = EMPTY_SPACE;
    world[2][4] = EMPTY_SPACE;
    world[3][0] = EMPTY_SPACE;
    world[3][1] = EMPTY_SPACE;
    world[3][2] = EMPTY_SPACE;
    world[3][3] = EMPTY_SPACE;
    world[3][4] = EMPTY_SPACE;
    world[4][0] = EMPTY_SPACE;
    world[4][1] = EMPTY_SPACE;
    world[4][2] = EMPTY_SPACE;
    world[4][3] = EMPTY_SPACE;
    world[4][4] = EMPTY_SPACE;

    if(zombieIsCloseToRick(rickLine, rickColumn, zombieLine, zombieColumn))
    {
        assert(1);
    }
    else
    {
        assert(0);
    }

    freeWorld(world, lines);
}

void testZombieIsCloseToRickShouldReturnFalse()
{
    uint8_t lines = 6;
    uint8_t columns = 6;
    uint8_t rickLine = 5;
    uint8_t rickColumn = 5;
    uint8_t zombieLine = 0;
    uint8_t zombieColumn = 0;

    char **world = createWorld(lines, columns, EMPTY);

    // Z . . . . .
    // . . . . . .
    // . . . . . .
    // . . . . . .
    // . . . . . .
    // . . . . . R
    
    world[0][0] = ZOMBIE;
    world[0][1] = EMPTY_SPACE;
    world[0][2] = EMPTY_SPACE;
    world[0][3] = EMPTY_SPACE;
    world[0][4] = EMPTY_SPACE;
    world[0][5] = EMPTY_SPACE;
    world[1][0] = EMPTY_SPACE;
    world[1][1] = EMPTY_SPACE;
    world[1][2] = EMPTY_SPACE;
    world[1][3] = EMPTY_SPACE;
    world[1][4] = EMPTY_SPACE;
    world[1][5] = EMPTY_SPACE;
    world[2][0] = EMPTY_SPACE;
    world[2][1] = EMPTY_SPACE;
    world[2][2] = EMPTY_SPACE;
    world[2][3] = EMPTY_SPACE;
    world[2][4] = EMPTY_SPACE;
    world[2][5] = EMPTY_SPACE;
    world[3][0] = EMPTY_SPACE;
    world[3][1] = EMPTY_SPACE;
    world[3][2] = EMPTY_SPACE;
    world[3][3] = EMPTY_SPACE;
    world[3][4] = EMPTY_SPACE;
    world[3][5] = EMPTY_SPACE;
    world[4][0] = EMPTY_SPACE;
    world[4][1] = EMPTY_SPACE;
    world[4][2] = EMPTY_SPACE;
    world[4][3] = EMPTY_SPACE;
    world[4][4] = EMPTY_SPACE;
    world[4][5] = EMPTY_SPACE;
    world[5][0] = EMPTY_SPACE;
    world[5][1] = EMPTY_SPACE;
    world[5][2] = EMPTY_SPACE;
    world[5][3] = EMPTY_SPACE;
    world[5][4] = EMPTY_SPACE;
    world[5][5] = RICK;

    if(zombieIsCloseToRick(rickLine, rickColumn, zombieLine, zombieColumn))
    {
        assert(0);
    }
    else
    {
        assert(1);
    }

    freeWorld(world, lines);
}


