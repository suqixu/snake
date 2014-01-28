/* by suqixu@126.com */

#include <time.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define SIZE  8
#define FPS 60

enum
{
    DOWN = 1,
    LEFT= 2,
    RIGHT = 3,
    UP = 4
};

enum
{
    DEFAULT_FRAMES = 7,
    MIN_FRAMES = 1,
    MAX_FRAMES = 10
};

typedef struct Node
{
    SDL_Rect box;
    struct Node *p;
} Node_t;

typedef struct Snake
{
    Node_t *head, *tail;
    int length;
    int dir;
    int skip_frames;
} Snake_t;

Node_t* new_node();
Node_t* new_food(Snake_t* snake);
Snake_t* new_snake ();
void wait_frame ();
int init_game();
int is_collision(SDL_Rect box1, SDL_Rect box2);
int is_in_snake(Node_t* node, Snake_t* snake);
int is_gameover (Snake_t *snake);
void change_dir (SDL_Event *event, Snake_t *snake);
void move_snake (Snake_t *snake);
void fill_rect(SDL_Rect* box, int r, int g, int b);
void draw_node (Node_t *node, int r, int g, int b);
void draw_snake (Snake_t *snake);
void erase_node (Node_t *node);

SDL_Surface* screen;
static Uint32 next_tick;
static int is_god;

Node_t* new_node()
{
    Node_t* node = (Node_t *)malloc(sizeof(Node_t));
    node->box.x = rand() % ( DISPLAY_WIDTH -  SIZE) + 1;
    node->box.y = rand() % ( DISPLAY_HEIGHT - SIZE) + 1;
    node->box.w =
    node->box.h = SIZE;

    return node;
}

int is_in_snake(Node_t* node, Snake_t* snake)
{
    int k;
    Node_t *tp = snake->head;

    for ( k = 0; k < snake->length; k++, tp = tp->p)
    {
        if (is_collision((*tp).box, (*node).box) == 1)
            return 1;
    }

    return 0;
}

Node_t* new_food(Snake_t* snake)
{
    Node_t* node = new_node();

    while(is_in_snake(node, snake) == 1)
    {
        free(node);
        node = new_node();
    }

    return node;
}

Snake_t* new_snake ()
{
    Snake_t* snake = (Snake_t * ) malloc ( sizeof ( Snake_t ) );
    Node_t *temp = new_node();
    snake->head = temp;

    temp = new_node();
    temp->box.x = snake->head->box.x + snake->head->box.w;
    temp->box.y = snake->head->box.y;
    snake->tail = temp;

    snake->tail->p = snake->head;
    snake->head->p = snake->tail;
    snake->length= 2;
    snake->skip_frames = DEFAULT_FRAMES;
    snake->dir = LEFT;

    return snake;
}

void wait_frame ()
{
    Uint32 this_tick = SDL_GetTicks();

    if ( this_tick < next_tick )
        SDL_Delay ( next_tick - this_tick );

    next_tick = this_tick + 1000/FPS;
}

void fill_rect(SDL_Rect* box, int r, int g, int b)
{
    SDL_FillRect (screen, box , SDL_MapRGB ( screen->format, r, g, b ) );
}

void draw_node (Node_t *node, int r, int g, int b)
{
    int border = 1;
    SDL_Rect rect_square;

    rect_square.x = node->box.x + border;
    rect_square.y = node->box.y + border;
    rect_square.w = node->box.w - 2 * border;
    rect_square.h = node->box.w - 2 * border;

    fill_rect(&(node->box), 0, 0xff, 0);
    fill_rect (&rect_square ,r, g, b);
}

void draw_snake (Snake_t *snake)
{
    int k,r,g,b;
    Node_t *tp = snake->head;

    for ( k = 0; k < snake->length; k++,tp = tp->p )
    {
        if (tp == snake->head)
        {
            r = 0xff;
            g = 0;
            b = 0;
        }
        else if (tp == snake->tail)
        {
            r = 0xff;
            g = 0xff;
            b = 0xff;
        }
        else
        {
            r = 0;
            g = 0;
            b = 0xff;
        }

        draw_node ( tp , r, g, b);
    }
}

void erase_node ( Node_t *node )
{
    fill_rect( &node->box, 0, 0, 0);
}

int init_game()
{
    int flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT ;

    if ( SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_TIMER ) )
        return -1;

    screen = SDL_SetVideoMode ( DISPLAY_WIDTH, DISPLAY_HEIGHT, 8, flags );

    if ( screen == NULL )
        return -1;

    int i;
    for ( i = 0; i<SDL_NUMEVENTS; ++i )
    {
		if ( (i != SDL_KEYDOWN) && (i != SDL_QUIT) )
            SDL_EventState(i, SDL_IGNORE);
	}

    return 0;
}

void change_dir (SDL_Event *event, Snake_t *snake)
{
    switch ( event->key.keysym.sym )
    {
    case SDLK_LEFT:
        if (snake->dir != RIGHT )
            snake->dir = LEFT;
        break;
    case SDLK_RIGHT:
        if (snake->dir != LEFT )
            snake->dir = RIGHT;
        break;
    case SDLK_UP:
        if (snake->dir != DOWN )
            snake->dir = UP;
        break;
    case SDLK_DOWN:
        if (snake->dir != UP )
            snake->dir = DOWN;
        break;
    case SDLK_w:
        if (snake->skip_frames > MIN_FRAMES)
            snake->skip_frames--;
        break;
    case SDLK_s:
        if (snake->skip_frames < MAX_FRAMES)
            snake->skip_frames++;
        break;
    case SDLK_g:
        is_god ^= 1;
        break;
    default:
        break;
    }
}

void move_snake (Snake_t *snake)
{
    switch ( snake->dir )
    {
    case UP:
    {
        snake->tail->box.x = snake->head->box.x;
        snake->tail->box.y = snake->head->box.y - snake->head->box.h;

        if (snake->tail->box.y < 0)
            snake->tail->box.y = DISPLAY_HEIGHT- snake->tail->box.h;
        break;
    }
    case DOWN:
    {
        snake->tail->box.x = snake->head->box.x;
        snake->tail->box.y = snake->head->box.y + snake->head->box.h;

        if (snake->tail->box.y > DISPLAY_HEIGHT - snake->tail->box.h)
            snake->tail->box.y = 0;
        break;
    }
    case LEFT:
    {
        snake->tail->box.x = snake->head->box.x - snake->head->box.w;
        snake->tail->box.y = snake->head->box.y;

        if (snake->tail->box.x < 0)
            snake->tail->box.x = DISPLAY_WIDTH - snake->tail->box.w;
        break;
    }
    case RIGHT:
    {
        snake->tail->box.x = snake->head->box.x + snake->head->box.w;
        snake->tail->box.y = snake->head->box.y;

        if (snake->tail->box.x > DISPLAY_WIDTH - snake->tail->box.w)
            snake->tail->box.x = 0;
        break;
    }
    default:
        break;
    }

    snake->head = snake->tail;
    snake->tail = snake->tail->p ;
}

void add_snake_node ( Snake_t *snake , Node_t * food)
{
    Node_t *tp = new_node();
    tp->box.x = snake->head->box.x;
    tp->box.y = snake->head->box.y;
    tp->p = snake->tail;

    snake->head->p = tp;
    snake->length++;
    
    free(food);
}

int is_collision(SDL_Rect box1, SDL_Rect box2)
{
    if (box1.x + box1.w < box2.x ||
        box1.x > box2.x + box2.w ||
        box1.y + box1.h < box2.y ||
        box1.y > box2.y + box2.h)
        return 0;

    return 1;
}

int is_gameover (Snake_t *snake)
{
    int k;
    Node_t *tp = snake->tail;

    for ( k = snake->length; k >= 5; k--, tp = tp->p )
    {
        if (snake->head->box.x == tp->box.x &&
            snake->head->box.y == tp->box.y )
            return 1;
    }

    return 0;
}

int main ( int argc, char *argv[] )
{
    int running = 1;
    int frame_count = 0;
    int socres = 0;
    int speed = 0;
    SDL_Event event;

    if ( init_game() == -1 )
        return 0;

    srand (time ( NULL ) );
    Snake_t* snake = new_snake ();
    Node_t* food = new_food(snake);

    while ( running == 1)
    {
        wait_frame();
        frame_count++;

        char szFps[ 128 ] ;
        socres = snake->length - 2;
        speed = MAX_FRAMES - MIN_FRAMES - snake->skip_frames +2;

        sprintf ( szFps, "snake socres:%d speed:%d god:%s", socres, speed, is_god == 1 ? "on" : "off" );
        SDL_WM_SetCaption ( szFps, NULL );

        fill_rect(NULL, 0, 0, 0);
        draw_snake ( snake );
        draw_node(food, 0xff, 0xff, 0);

        while ( SDL_PollEvent ( &event ) )
        {
            switch ( event.type )
            {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;
                else
                    change_dir ( &event, snake );
                break;
            default:
                break;
            }
        }

        if (frame_count > FPS)
            frame_count= 0;

        if(frame_count == snake->skip_frames)
        {
            move_snake ( snake );
            frame_count = 0;
        }

        if (is_collision(snake->head->box, food->box) == 1)
        {
            add_snake_node ( snake, food );
            food = new_food(snake);
        }

        if (is_god == 0 && is_gameover ( snake) == 1)
            running = 0;

        if ( SDL_Flip ( screen ) == -1 )
            return 0;
    }

    int k;
    Node_t *tp = snake->head;

    for ( k = 0; k < snake->length; k++, tp = tp->p)
    {
        free( tp );
    }

    free(snake);
    SDL_Quit();

    return 0;
}