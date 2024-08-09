#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

/* Define la altura del tablero. */
#define HEIGHT 20

/* Define la anchura del tablero. */
#define WIDTH 20

/* Multiplicador de anchura de la pantala.
Solamente afecta en la representacion en
la pantalla y no a los metadatos del programa.*/
#define WMULTI 2

/* Define la velocidad de de la serpiente. */
#define SPEED 7

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
void switch_dir(int x, int y);

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
int remove_entity(Entity **entities, int length, int pos);

/* Inicializa una nueva partida.
Hace uso de las variables externas. */
void init_game();

/* Dibuja los bordes del tablero. */
void draw_borders(WINDOW *win);

/* Permite generar numeros
naturales aleatorios. */
int randint(int min, int max);

/* Imprime un array de entidades. */
void print_entities(Entity *entities, int length);

/* Variables externas */

/* Puntuacion de la partida. */
int score;

/* Player (cabeza de la serpiente). */
Entity player;
Vec2 player_dir;

/* Cola de la serpiente. */
int tail_length;
Entity *tail;
Entity tail_chunk;

/* Berries. */
Entity berrie;

int game_over;

/* Constante de caracter. */
char game_over_text[] = "GAME OVER";
char snake_tail_init_err[] = "Snake tail inicialization error.\n";
char snake_tail_resize_err[] = "Snake tail resize error.\n";

/* Juego Snake */
int main()
{
  /* Player (cabeza de la serpiente). */
  extern Entity player;
  extern Vec2 player_dir;

  /* Cola de la serpiente. */
  extern int tail_length;
  extern Entity *tail;
  extern Entity tail_chunk;

  /* Berries. */
  extern Entity berrie;

  int i, c, key, game_over, quit;

  quit = false;

  /* Inicializar semilla de aleatoriedad. */
  srand(time(NULL));

  /* Inicializar ventana de ncurses. */
  WINDOW *win = initscr();
  keypad(win, true);
  noecho();

  /* Inicialiar el color. */
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_RED);
  init_pair(3, COLOR_BLACK, COLOR_BLUE);
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);

  while (quit == false)
  {
    /* Inicializar nueva partida. */
    init_game();
    game_over = false;

    nodelay(win, true);
    while (game_over == false)
    {
      /* Comprobar si el jugador
      se salio del mapa. */
      if (player.pos.x <= 0 || player.pos.x >= WIDTH)
        game_over = true;
      else if (player.pos.y <= 0 || player.pos.y >= HEIGHT)
        game_over = true;

      /* Comprobar si el jugador se
      choco contra su cola. */
      for (i = 0; i < tail_length; ++i)
      {
        tail_chunk = tail[i];
        if (tail_chunk.pos.x == player.pos.x &&
            tail_chunk.pos.y == player.pos.y)
        {
          game_over = true;
          break;
        }
      }

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

      /* Control de teclas de manera
      no bloqueante. */
      key = wgetch(win);

      switch (key)
      {
      case ERR:
        break;
      case 'q':
        game_over = true;
        break;

      case KEY_RIGHT:
        if (player_dir.x == 0)
          switch_dir(1, 0);
        break;

      case KEY_LEFT:
        if (player_dir.x == 0)
        {
          switch_dir(-1, 0);
        }
        break;

      case KEY_UP:
        if (player_dir.y == 0)
        {
          switch_dir(0, -1);
        }
        break;

      case KEY_DOWN:
        if (player_dir.y == 0)
        {
          switch_dir(0, 1);
        }
        break;

      default:
        break;
      }

      if (game_over)
        break;

      /* Movimiento de la cabeza de
      la serpiente. */
      player.pos.x += player_dir.x;
      player.pos.y += player_dir.y;

      /* Eliminar frame anterior */
      werase(win);

      /* Dibujar cola de la serpiente. */
      attrset(COLOR_PAIR(4));
      for (i = 0; i < tail_length; ++i)
      {
        tail_chunk = tail[i];
        wmove(win, tail_chunk.pos.y, tail_chunk.pos.x * WMULTI);
        waddch(win, tail_chunk.c);
      }
      attrset(COLOR_PAIR(1));

      /* Dibuajr berrie. */
      attrset(COLOR_PAIR(2));
      wmove(win, berrie.pos.y, berrie.pos.x * WMULTI);
      waddch(win, berrie.c);
      attrset(COLOR_PAIR(1));

      /* Dibujar cabeza de la serpiente. */
      attrset(COLOR_PAIR(4));
      wmove(win, player.pos.y, player.pos.x * WMULTI);
      waddch(win, player.c);
      attrset(COLOR_PAIR(1));

      /* Dibujar bordes. */
      draw_borders(win);

      /* Refrescar la pantalla y
      esperar al proximo frame. */
      wmove(win, 0, 0);
      wrefresh(win);
      usleep(1000000 / SPEED);
    }

    /* Liberar la memoria de la
    cola de la serpiente. */
    free(tail);
    tail = NULL;

    /* Secuencia de game-over. */
    wmove(win, HEIGHT / 2, (WIDTH * WMULTI / 2) - (sizeof(game_over_text) / 2));
    wprintw(win, "%s", game_over_text);

    wmove(win, 0, 0);
    wrefresh(win);

    nodelay(win, false);
    while ((key = wgetch(win)) != 'r' && key != 'q')
      ;

    if (key == 'q')
      quit = true;
  }

  endwin();

  return 0;
}

void switch_dir(int x, int y)
{
  /* Player (cabeza de la serpiente). */
  extern Entity player;
  extern Vec2 player_dir;

  player_dir.x = x;
  player_dir.y = y;

  if (x == 1)
    player.c = '>';
  else if (x == -1)
    player.c = '<';
  else if (y == 1)
    player.c = 'v';
  else if (y == -1)
    player.c = '^';
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

void init_game()
{
  extern int score;
  /* Player (cabeza de la serpiente). */
  extern Entity player;
  extern Vec2 player_dir;

  /* Cola de la serpiente. */
  extern int tail_length;
  extern Entity *tail;
  extern Entity tail_chunk;

  /* Berries. */
  extern Entity berrie;

  extern int game_over;

  score = 0;

  /* Inicializar jugador. */
  player.c = '>';
  player.pos.x = 5;
  player.pos.y = HEIGHT / 2;
  player_dir.x = 1;
  player_dir.y = 0;

  /* Inicializar cola de la serpiente. */
  tail_length = LENGTH;
  tail = init_tail(player, tail_length);
  if (tail == NULL)
  {
    fprintf(stderr, "%s", snake_tail_init_err);
    exit(1);
  }

  /* Inicializar berrie. */
  berrie.c = '@';
  berrie.pos.x = randint(1, WIDTH);
  berrie.pos.y = randint(1, HEIGHT);

  game_over = false;
}

void draw_borders(WINDOW *win)
{
  extern int score;
  int i, j;
  char score_text[] = " SCORE: XXX ";

  attrset(COLOR_PAIR(3));

  /* Dibujando bordes. */
  for (i = 0; i < WIDTH * WMULTI; ++i)
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

    wmove(win, i, WIDTH * WMULTI);
    waddch(win, '|');
  }

  /* Dibujando esquinas. */
  wmove(win, 0, 0);
  waddch(win, '+');

  wmove(win, HEIGHT, 0);
  waddch(win, '+');

  wmove(win, 0, WIDTH * WMULTI);
  waddch(win, '+');

  wmove(win, HEIGHT, WIDTH * WMULTI);
  waddch(win, '+');

  /* Dibujar puntuacion. */
  snprintf(score_text, sizeof(score_text), " SCORE: %3d ", score);

  wmove(win, 0, (WIDTH * WMULTI / 2) - (sizeof(score_text) / 2));
  wprintw(win, "%s", score_text);

  attrset(COLOR_PAIR(1));
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