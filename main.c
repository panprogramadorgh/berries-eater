#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

/* Define la altura del tablero. */
#define HEIGHT 30

/* Define la anchura del tablero. */
#define WIDTH 40

/* Define la velocidad de de la serpiente. */
#define SPEED 10

/* Define la longitud inicial de la serpiente. */
#define LENGTH 3

/* Estructura util para
representar direccciones y
posiciones en el mapa. */
typedef struct Vec2
{
  int x;
  int y;
} Vec2;

/* Estructura para representar
cualquier entidad en el mapa.
(cabeza de jugador, cuerpo de serpiente
y berries). */
typedef struct Entity
{
  char c;
  Vec2 pos;
} Entity;

/* Permite redireccionar el jugador. */
void switch_dir(Entity *player, Vec2 *dir, int x, int y);

/* Permite inicializar la cola de la serpiente.
La funcion retorna un puntero a la primera
entidad y NULL en caso de fallo alocando la memoria. */
Entity *init_tail(Entity player, int initial_tl);

/* Permite insertar una entidad en la posicion `pos`
de un array de entidades de longitud dinamica.
`length` representa la cantidad actual de entidades
dentro del array. El parametro `e` contiene los datos
de la nueva entidad a insertar. La funcion retorna
la nueva longitud para el array de longitud dinamica.
Si la funcion retorna -1, quiere decir que hubo un error
alocando la memoria. */
int insert_entity(Entity **entities, int length, Entity e, int pos);

/* Permite eliminar el elemento del array
de entidades `entities` ubicado en `pos`. */
int remove_entity(Entity **entities, int lengt, int pos);

/* Dibuja los bordes del tablero. */
void draw_borders(WINDOW *win);

/* Permite generar numeros
naturales aleatorios. */
int randint(int min, int max);

/* Imprime un array de entidades. */
void print_entities(Entity *entities, int length);

/* Variables externas */
int score = 0;
char game_over_text[] = "GAME OVER";
char snake_tail_init_err[] = "Snake tail inicialization error.\n";
char snake_tail_resize_err[] = "Snake tail resize error.\n";

/* Juego Snake */
int main()
{
  /* Player (cabeza de la serpiente). */
  Entity player;
  Vec2 player_dir;

  /* Cola de la serpiente. */
  int tail_length;
  Entity *tail;
  Entity tail_chunk;

  /* Berries. */
  Entity berrie;

  int i, c, key, quit;

  /* Inicializar jugador. */
  player.c = '>';
  player.pos.x = 5;
  player.pos.y = 5;
  player_dir.x = 1;
  player_dir.y = 0;

  /* Inicializar cola de la serpiente. */
  tail_length = LENGTH;
  tail = init_tail(player, tail_length);
  if (tail == NULL)
  {
    fprintf(stderr, "%s", snake_tail_init_err);
    free(tail);
    tail = NULL;
    return 1;
  }

  /* Inicializar semilla de aleatoriedad. */
  srand(time(NULL));
  berrie.c = '@';
  berrie.pos.x = randint(1, WIDTH);
  berrie.pos.y = randint(1, HEIGHT);

  key = ERR;
  quit = 0;

  /* Inicializar ventana de ncurses. */
  WINDOW *win = initscr();
  keypad(win, true);
  nodelay(win, true);
  noecho();

  while (1)
  {
    /* Comprobar si el jugador
    se salio del mapa. */
    if (player.pos.x <= 0 || player.pos.x >= WIDTH)
      break;
    else if (player.pos.y <= 0 || player.pos.y >= HEIGHT)
      break;

    /* Comprobar si el jugador se
    choco contra su cola. */
    for (i = 0; i < tail_length; ++i)
    {
      tail_chunk = tail[i];
      if (tail_chunk.pos.x == player.pos.x &&
          tail_chunk.pos.y == player.pos.y)
      {
        quit = 1;
        break;
      }
    }

    if (quit == 1)
      break;

    /* Control de teclas de manera
    no bloqueante. */
    key = wgetch(win);

    switch (key)
    {
    case ERR:
      break;
    case 'q':
      quit = 1;
      break;

    case KEY_RIGHT:
      if (player_dir.x == 0)
        switch_dir(&player, &player_dir, 1, 0);
      break;

    case KEY_LEFT:
      if (player_dir.x == 0)
      {
        switch_dir(&player, &player_dir, -1, 0);
      }
      break;

    case KEY_UP:
      if (player_dir.y == 0)
      {
        switch_dir(&player, &player_dir, 0, -1);
      }
      break;

    case KEY_DOWN:
      if (player_dir.y == 0)
      {
        switch_dir(&player, &player_dir, 0, 1);
      }
      break;

    default:
      break;
    }

    if (quit == 1)
      break;

    /* Insercion de nueva primer entidad
    en el array `tail`. */
    tail_chunk.pos.x = player.pos.x;
    tail_chunk.pos.y = player.pos.y;

    if (player_dir.x == 0)
      tail_chunk.c = 'H';
    else if (player_dir.y == 0)
      tail_chunk.c = '=';

    tail_length = insert_entity(&tail, tail_length, tail_chunk, 0);
    if (tail_length == -1)
    {
      endwin();
      fprintf(stderr, "%s", snake_tail_resize_err);
      free(tail);
      tail = NULL;
      exit(1);
    }

    /* Comprobar si el jugador se
    comio una berrie, en cuyo caso
    no se debe eliminar la ultima
    entidad del array `tail`. */
    if (berrie.pos.x != player.pos.x || berrie.pos.y != player.pos.y)
    {
      tail_length = remove_entity(&tail, tail_length, tail_length - 1);
      if (tail_length == -1)
      {
        endwin();
        fprintf(stderr, "%s", snake_tail_resize_err);
        free(tail);
        tail = NULL;
        exit(1);
      }
    }
    else
    {
      ++score;
      berrie.pos.x = randint(1, WIDTH);
      berrie.pos.y = randint(1, HEIGHT);
    }

    /* Movimiento de la cabeza de
    la serpiente. */
    player.pos.x += player_dir.x;
    player.pos.y += player_dir.y;

    /* Eliminar frame anterior */
    werase(win);

    /* Dibuajr berrie. */
    wmove(win, berrie.pos.y, berrie.pos.x * 2);
    waddch(win, berrie.c);

    /* Dibujar cabeza de la serpiente. */
    wmove(win, player.pos.y, player.pos.x * 2);
    waddch(win, player.c);

    /* Dibujar cola de la serpiente. */
    for (i = 0; i < tail_length; ++i)
    {
      tail_chunk = tail[i];
      wmove(win, tail_chunk.pos.y, tail_chunk.pos.x * 2);
      waddch(win, tail_chunk.c);
    }

    /* Dibujar bordes. */
    draw_borders(win);

    /* Refrescar la pantalla y
    esperar al proximo frame. */
    wmove(win, 0, 0);
    wrefresh(win);
    usleep(1000000 / SPEED);
  }

  /* Secuencia de game-over. */
  nodelay(win, false);
  wmove(win, HEIGHT / 2, WIDTH - (sizeof(game_over_text) / 2));

  wprintw(win, "%s", game_over_text);
  wgetch(win);

  endwin();

  /* Liberar la memoria de la
  cola de la serpiente. */
  free(tail);
  tail = NULL;

  return 0;
}

void switch_dir(Entity *player, Vec2 *dir, int x, int y)
{
  dir->x = x;
  dir->y = y;

  if (x == 1)
    player->c = '>';
  else if (x == -1)
    player->c = '<';
  else if (y == 1)
    player->c = 'v';
  else if (y == -1)
    player->c = '^';
}

Entity *init_tail(Entity player, int initial_tl)
{
  int i, j;
  Entity e;

  e.c = '=';
  e.pos.y = player.pos.y;
  Entity *tail = (Entity *)malloc(sizeof(Entity) * initial_tl);

  if (tail == NULL)
    return NULL;

  for (i = 0; i < initial_tl; ++i)
  {
    e.pos.x = player.pos.x - (i + 1);
    tail[i] = e;
  }

  return tail;
}

int insert_entity(Entity **entities, int length, Entity e, int pos)
{
  Entity *new_entities;
  int i, new_length;

  /*  */
  if (pos > length || pos < 0)
    return -1;

  /* Incrementar la longitud. */
  new_length = length + 1;

  /* Realocar en un bloque de memoria
  con longitud + 1. */
  new_entities = realloc(*entities, sizeof(Entity) * new_length);
  if (new_entities == NULL)
    return -1;

  *entities = new_entities;

  if (pos < length)
  {
    for (i = length - 1; i >= pos; --i)
      (*entities)[i + 1] = (*entities)[i];
  }

  (*entities)[pos] = e;

  return new_length;
}

int remove_entity(Entity **entities, int length, int pos)
{
  Entity *new_entities;
  int i, new_length;

  new_length = length - 1;

  for (i = pos; i < length - 1; ++i)
    (*entities)[i] = (*entities)[i + 1];

  new_entities = (Entity *)realloc(*entities, sizeof(Entity) * new_length);

  if (new_entities == NULL)
    return -1;

  *entities = new_entities;

  return new_length;
}

void draw_borders(WINDOW *win)
{
  extern int score;
  int i, j;
  char score_text[] = " SCORE: XXX ";

  /* Dibujando bordes. */
  for (i = 0; i < WIDTH * 2; ++i)
  {
    wmove(win, 0, i);
    waddch(win, '-');

    wmove(win, HEIGHT, i);
    waddch(win, '-');
  }
  for (i = 0; i < HEIGHT; ++i)
  {
    wmove(win, i, 0);
    waddch(win, '|');

    wmove(win, i, WIDTH * 2);
    waddch(win, '|');
  }

  /* Dibujando esquinas. */
  wmove(win, 0, 0);
  waddch(win, '+');

  wmove(win, HEIGHT, 0);
  waddch(win, '+');

  wmove(win, 0, WIDTH * 2);
  waddch(win, '+');

  wmove(win, HEIGHT, WIDTH * 2);
  waddch(win, '+');

  /* Dibujar puntuacion. */
  snprintf(score_text, sizeof(score_text), " SCORE: %3d ", score);

  wmove(win, 0, WIDTH - (sizeof(score_text) / 2));
  wprintw(win, "%s", score_text);
}

int randint(int min, int max)
{
  return (rand() % (max - min)) + min;
}

void print_entities(Entity entities[], int length)
{
  Entity e;
  int i;

  printf("Char\t  X\t  Y\n");
  for (i = 0; i < length; ++i)
  {
    e = entities[i];
    printf("%c\t%3d\t%3d\n", e.c, e.pos.x, e.pos.y);
  }
}