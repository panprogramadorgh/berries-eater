# Berries Eater

## Descripcion

Se trata de un video juego de terminal, desarrollado en C con la biblioteca de formato y entrada/salida de texto desde la terminal `ncurses`.

El videojuego es un prototipo del clasico snake game, es decir, una serpiente en un mapa donde tiene que ir comiendo berries (en este caso) para obtener puntos, y que a medida que obtiene puntos la longitud de la serpiente se incrementa en 1.

## Implementacion

### Estructuras del programa

El programa hace uso de dos estructuras: la estructura `Vec2` para representar posiciones y direcciones, asi como la estructura `Entity`, la cual es utilizada para representar cualquier entidad en el mapa (como la serpiente o las manzanas).

```c
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
```

\* Cada uno de los trozos de cola de serpiente **tambien son entidades**.

### La cola de la serpiente

La cola de la serpiente consta de un array de entidades, todas y cada una con su simbolo y posicion determinada en el mapa. Al constar de un array de entidades, es por eso que a lo largo de todo el programa, hay varias funciones para manipulacion de esta clase de estructuras de datos. Si bien estas utilidades podrian a ver sido desarrolladsa desde una perspectiva especifica para la cola de la serpiente, mas bien he decidido desarrollarmas medianamente genericas por si en algun futuro se hacen nuevas mejoras en el programa que impliquen la manipulacion de arrays de entidades. Absolutamente todas estas utilidades de manipulacion de arryas de entidades, trabajan con arrays de memoria dinamica: lo que significa que hay que tener un exhaustivo control con su manejo para evitar fugas, especialmente en situaciones donde ocurren errores.

Ejemplificando lo mencionado con anterioridad, mientras que `insert_entity`, `remove_entity` y `print_entities` son funciones de relativo uso general:

```c
int insert_entity(Entity **entities, int length, Entity e, int pos);

int remove_entity(Entity **entities, int length, int pos);

void print_entities(Entity *entities, int length);
```

La funcion `init_tail` se encarga especificamente de inicializar la cola de la serpiente en el programa, dandole una longitud inicial en la partida.

```c
Entity *init_tail(Entity player, int initial_tl);
```

### Principales variables externas

Otra importante funcion del programa es `init_game`, por su declaracion de prototipo:

```c
void init_game();
```

Esta funcion tiene un **desempño crucial** puesto que es la encargada de inicializar las variables externas (o globales) del programa (de acuerdo con la partida) cada vez que el programa da comienzo, o bien cada vez que el jugador ha perdido y presiona la tecla `r` (por la palabra retry en ingles) para jugar una nueva partida.

- Estas son algunas variables externas del programa;
- todas ellas almacenas metadatos de la partida;
- son totalmente inprescindibles para que el programa funcione correctamente:

```c
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

/*  Estado de la partida. */
int game_over;
```

### Entrada del usuario

Para concluir con los principales rasgos de esta implementacion del snake game, cubriremos como el programa maneja la entrada del usuario, entrada de usuario no bloqueante.

. . .
