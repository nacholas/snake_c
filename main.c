#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdbool.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))
#define FIELD_EMPTY 0
#define FIELD_SNAKE_HEAD 1
#define FIELD_SMALL_ITEM 2001
#define FIELD_BIG_ITEM   2002

#define ORIENTATION_RIGHT 0
#define ORIENTATION_DOWN 1
#define ORIENTATION_LEFT 2
#define ORIENTATION_UP 3

//                (y,  x)
int playing_field[8][30];
int orientation = 0; //0=right,1=down,2=left,3=up
bool game_running = false;
int game_speed = 200;
int snake_length = 3;
double small_item_chance = 0.15;
double big_item_chance = 0.05;

//HELPER FUNCTIONS

int getch_noblock(int delay) {
    Sleep(delay);
    if (_kbhit())
        return _getch();
    else
        return -1;
}

// ACTUAL GAME

void print_playing_field()
{
    //clear screen
    system("cls");
    //print upper border
    for(int i = 0; i < NELEMS(playing_field[0])+2; i++)
    {
        printf("#");
    }

    printf("\n");
    for(int i = 0; i < NELEMS(playing_field); i++)
    {
        printf("#"); //print left limiting border
        for(int j = 0; j < (NELEMS(playing_field[i])); j++)
        {
            //print field character
            switch(playing_field[i][j])
            {
                case FIELD_EMPTY:
                    //empty space
                    printf(" ");
                    break;
                case FIELD_SNAKE_HEAD:
                    //snake head (depending on orientation)
                    if(orientation == 0)
                        printf(">");
                    else if(orientation == 1)
                        printf("v");
                    else if(orientation == 2)
                        printf("<");
                    else
                        printf("^");
                    break;
                case FIELD_SMALL_ITEM:
                    printf("o");
                    break;
                case FIELD_BIG_ITEM:
                    printf("O");
                    break;
                default:
                    //snake body
                    printf("=");
                    //DEBUG: printf("%i", playing_field[i][j]);
                    break;
            }
        }
        printf("#\n"); //print right limiting border
    }

    //print lower border
    for(int i = 0; i < NELEMS(playing_field[0])+2; i++)
    {
        printf("#");
    }
    printf("\n");
}
void insert_snake()
{
    int midy = NELEMS(playing_field) / 2;
    int midx = NELEMS(playing_field[0]) / 2;

    playing_field[midy][midx+1] = FIELD_SNAKE_HEAD;
    playing_field[midy][midx] =   2;
    playing_field[midy][midx-1] = 3;

    snake_length = 3;
}

void insert_items()
{
    int rand_value = rand();
    int item = -1;
    if(rand_value <= big_item_chance * RAND_MAX)
        item = FIELD_BIG_ITEM;
    else if(rand_value <= small_item_chance * RAND_MAX)
        item = FIELD_SMALL_ITEM;
    else
        return; //no item spawning

    // pick random field
    bool field_empty = false;
    int rand_y;
    int rand_x;
    while(!field_empty)
    {
        rand_y = rand() % NELEMS(playing_field[0])-1;
        rand_x = rand() % NELEMS(playing_field[1])-1;

        if(playing_field[rand_y][rand_x] == FIELD_EMPTY)
            field_empty = true;
    }
    //put item into field
    playing_field[rand_y][rand_x] = item;
}

bool check_move(int y, int x)
{
    //check if the snake bumped into a wall
    if(NELEMS(playing_field) <= y || NELEMS(playing_field[0]) <= x || x < 0 || y < 0)
    {
        return false;
    }

    //process items
    if(playing_field[y][x] == FIELD_SMALL_ITEM)
    {
        snake_length += 1;
        playing_field[y][x] = FIELD_EMPTY;
    }
    else if (playing_field[y][x] == FIELD_BIG_ITEM)
    {
        snake_length += 2;
        playing_field[y][x] = FIELD_EMPTY;
    }

    //check if the snake bumped into itself
    if(playing_field[y][x] != FIELD_EMPTY)
    {
        return false;
    }

    return true;
}
void update_snake()
{
    int head_x = -1;
    int head_y = -1;
    bool head_updated = false;
    for(int i = 0; i < NELEMS(playing_field); i++)
    {
        for(int j = 0; j < NELEMS(playing_field[0]); j++)
        {
            //Update all fields
            if(playing_field[i][j] != FIELD_EMPTY)
            {
                //printf("Found field type %i at field %i,%i", playing_field[i][j], i, j);
                //update head position based on orientation
                if(!head_updated && playing_field[i][j] == FIELD_SNAKE_HEAD)
                {
                    int move_y = 0;
                    int move_x = 0;
                    switch(orientation)
                    {
                        case 0:
                            move_x = 1;
                            break;
                        case 1:
                            move_y = 1;
                            break;
                        case 2:
                            move_x = -1;
                            break;
                        case 3:
                            move_y = -1;
                            break;
                        default:
                            printf("ERROR: Unknown snake direction, can't continue!");
                            exit(1);
                    }

                    if(check_move(i+move_y, j+move_x))
                    {
                        //playing_field[i][j] = 2; //snake head is now snake body
                        playing_field[i+move_y][j+move_x] = FIELD_SNAKE_HEAD; //put new snake head
                        head_x = j + move_x;
                        head_y = i + move_y;
                        head_updated = true;
                    }
                    else
                    {
                        game_running = false;
                    }
                }
                //update field only if its not the old snake head or an item
                if(!(i == head_y && j == head_x) && !(playing_field[i][j] == FIELD_SMALL_ITEM
                                                     || playing_field[i][j] == FIELD_BIG_ITEM ))
                {
                    playing_field[i][j] += 1;
                    //if its the old tail, make the field empty
                    if(playing_field[i][j] > snake_length)
                        playing_field[i][j] = FIELD_EMPTY;
                }
                //printf(" > set to: %i\n", playing_field[i][j]);
            }
        }
    }
}

void start_game()
{
    game_running = true;
    while(game_running)
    {
        insert_items();
        print_playing_field();
        char key = getch_noblock(game_speed);
        switch(key)
        {
            case 'W':
            case 'w':
                if(orientation != ORIENTATION_DOWN) //make sure player does not crash into himself.
                    orientation = ORIENTATION_UP;
                break;
            case 'A':
            case 'a':
                if(orientation != ORIENTATION_RIGHT)
                    orientation = ORIENTATION_LEFT;
                break;
            case 's':
            case 'S':
                if(orientation != ORIENTATION_UP)
                    orientation = ORIENTATION_DOWN;
                break;
            case 'D':
            case 'd':
                if(orientation != ORIENTATION_LEFT)
                    orientation = ORIENTATION_RIGHT;
                break;
        }
        update_snake();
    }
    printf("Game over!");
}
void init_game() {
    //Make sure we don't accidentally break the game by making the playing field too small
    if(NELEMS(playing_field) < 5 || NELEMS(playing_field[0]) < 5)
    {
        printf("\n\nERROR: Can't work with a field that small!\n");
        exit(1);
    }
    insert_snake();
    print_playing_field();
    srand(time(NULL)); //seed randomness
    printf("3..");
    Sleep(333);
    printf("2..");
    Sleep(333);
    printf("1..");
    Sleep(333);
}


int main() {
    setbuf(stdout, 0);
    printf("Press any key to start Snake!\n");
    //getch();

    init_game();
    start_game();
    return 0;
}