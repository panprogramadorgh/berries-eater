#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

/* Define la altura del tablero. */
#define HEIGHT 30

/* Define la anchura del tablero. */
#define WIDTH 40

/* Define la velocidad de de la serpiente. */
#define SPEED 10

/* Define la longitud maxima de la serpiente. */
#define MAX 50

typedef struct Vec2
{
  int x;
  int y;
} Vec2;

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

/* Permite clonar un array de entidades.
Se debe incluir la longitud maxima para
el array `from` y `to`. */
void clone_entities(Entity from[], int max_from, Entity to[], int max_to);

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

/* Imprime un array de entidades. */
void print_entities(Entity *entities, int length);

/* FIXME: Convertir array de punteros
de entidad a array de entidade. */

/* Juego Snake */
int main()
{
  Entity player = {'>', {4, 4}};
  int tail_length;
  Entity *tail;

  tail_length = 3;
  tail = init_tail(player, tail_length);
  if (tail == NULL)
  {
    fprintf(stderr, "snake tail inicialization error\n");
    return 1;
  }

  Entity e = {'=', {100, 100}};

  tail_length = insert_entity(&tail, tail_length, e, tail_length);

  if (tail_length == -1)
  {
    fprintf(stderr, "cannot insert\n");
    return 1;
  }

  tail_length = remove_entity(&tail, tail_length, 0);

  print_entities(tail, tail_length);

  free(tail);
  tail = NULL;

  // Vec2 player_dir = {1, 0};
  // int c, key, game_over;
  // /* Variable de bucles. */
  // int x, y;

  // game_over = 0;
  // key = ERR;

  // WINDOW *win = initscr();
  // keypad(win, 1);
  // nodelay(win, 1);
  // noecho();

  // while (game_over == 0)
  // {
  //   key = wgetch(win);

  //   if (key == ERR)
  //     ;

  //   switch (key)
  //   {
  //   case 'q':
  //     game_over = 1;
  //     break;

  //   case KEY_RIGHT:
  //     if (player_dir.x == 0)
  //       switch_dir(&player, &player_dir, 1, 0);
  //     break;

  //   case KEY_LEFT:
  //     if (player_dir.x == 0)
  //     {
  //       switch_dir(&player, &player_dir, -1, 0);
  //     }
  //     break;

  //   case KEY_UP:
  //     if (player_dir.y == 0)
  //     {
  //       switch_dir(&player, &player_dir, 0, -1);
  //     }
  //     break;

  //   case KEY_DOWN:
  //     if (player_dir.y == 0)
  //     {
  //       switch_dir(&player, &player_dir, 0, 1);
  //     }
  //     break;

  //   default:
  //     break;
  //   }

  //   player.pos.x += player_dir.x;
  //   player.pos.y += player_dir.y;

  //   werase(win);

  //   /* Dibujar borde */
  //   for (y = 0; y < HEIGHT; ++y)
  //   {
  //     for (x = 0; x < WIDTH; ++x)
  //     {
  //       if (
  //           (x == 0 && y == 0) ||
  //           (x == 0 && y == HEIGHT - 1) ||
  //           (x == WIDTH - 1 && y == 0) ||
  //           (x == WIDTH - 1 && y == HEIGHT - 1))
  //       {
  //         wmove(win, y, x * 2);
  //         waddch(win, '+');
  //       }
  //       else if (x == 0 || x == WIDTH - 1)
  //       {
  //         wmove(win, y, x * 2);
  //         waddch(win, '|');
  //       }
  //       else if (y == 0 || y == HEIGHT - 1)
  //       {
  //         wmove(win, y, x * 2);
  //         waddch(win, '-');
  //       }
  //     }
  //   }

  //   wmove(win, player.pos.y, player.pos.x * 2);
  //   waddch(win, player.c);

  //   wrefresh(win);

  //   usleep(1000000 / SPEED);
  // }

  // endwin();
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

void clone_entities(Entity from[], int max_from, Entity to[], int max_to)
{
  int i;

  for (i = 0; i < max_from && i < max_to; ++i)
    to[i] = from[i];
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