#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

#include "input.h"
#include "character.h"
#include "poke327.h"

typedef struct input_message {                          
  char msg[71];
  struct input_message *next;
} input_message_t;

static input_message_t *input_head, *input_tail;

void input_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void input_reset_terminal(void)
{
  endwin();

  while (input_head) {
    input_tail = input_head;
    input_head = input_head->next;
    free(input_tail);
  }
  input_tail = NULL;
}

static int compare_trainer_distance(const void *t1, const void *t2)
{
  const character_t *const *c1 = t1;
  const character_t *const *c2 = t2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

void input_display()
{
  int y, x;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;

        case ter_mountain:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
          break;

        case ter_tree:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;

        case ter_forest:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;

        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TALL_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;

        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, SHORT_GRASS_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;

        case ter_water:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, WATER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN));
          break;

        case ter_path:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, PATH_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;

        case ter_gate:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, GATE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
          
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMART_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;

        case ter_center:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;

        default:                                                
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC is at (%2d,%2d) on map: %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
 
  refresh();
}

static void input_scroll_trainer_list(char (*s)[40], int count)
{
  int page;
  int i;

  page = 0;

  while (1) {
    for (i = 0; i < 10; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + page]);
    }
    switch (getch()) {
    case KEY_UP:
      if (page) {
        page--;
      }
      break;
    case KEY_DOWN:
      if (page < (count - 10)) {
        page++;
      }
      break;
    
    //ESC
    case 27:
      return;
    }

  }
}

static void input_trainers_list_display(character_t **c,
                                     int count)
{
  int i;
  char (*s)[40];

  s = malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  snprintf(s[0], 40, "There are %d trainers on the map:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s : %2d %s",
             char_type_name[c[i]->npc->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "N" : "S"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "W" : "E"));
    if (count <= 10) {
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 10) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Press ESC to escape.");
    while (getch() != 27)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "To scroll up and down, use the arrow. Else press ESC to escape.");
    input_scroll_trainer_list(s, count);
  }

  free(s);
}

static void input_trainers_list()
{
  character_t **c;
  int x, y, count;

  c = malloc(world.cur_map->num_trainers * sizeof (*c));

  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  qsort(c, count, sizeof (*c), compare_trainer_distance);

  input_trainers_list_display(c, count);
  free(c);

  input_display();
}

void enter_pokemart()
{
  mvprintw(0, 0, "Welcome to the Pokemart! We got plenty of helpful tools.");
  refresh();
  getch();
}

void enter_pokecenter()
{
  mvprintw(0, 0, "Welcome to the Pokemon Center.  Let me take care of your pokemon.");
  refresh();
  getch();
}

void input_battle(character_t *aggressor, character_t *defender)
{
  character_t *npc;

  input_display();
  mvprintw(0, 0, "You Win! Take my poke coin...");
  refresh();
  getch();
  if (aggressor->pc) {
    npc = defender;
  } else {
    npc = aggressor;
  }

  npc->npc->defeated = 1;
  if (npc->npc->ctype == char_hiker || npc->npc->ctype == char_rival) {
    npc->npc->mtype = move_sentry;
  }
}

int move_pc_direction(int input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;

  case 4:
  case 5:
  case 6:
    break;

  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;

  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;

  case 2:
  case 5:
  case 8:
    break;

  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;

  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      enter_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      enter_pokecenter();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]->npc &&
        world.cur_map->cmap[dest[dim_y]][dest[dim_x]]->npc->defeated) {
      return 1;
    } else if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]->npc) {
      input_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      INT_MAX) {
    return 1;
  }

  return 0;
}

void input_user_input(pair_t dest)
{
  int turn;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
      turn = move_pc_direction(7, dest);
      break;

    case '8':
    case 'k':
      turn = move_pc_direction(8, dest);
      break;

    case '9':
    case 'u':
      turn = move_pc_direction(9, dest);
      break;
    case '6':
    case 'l':
      turn = move_pc_direction(6, dest);
      break;

    case '3':
    case 'n':
      turn = move_pc_direction(3, dest);
      break;

    case '2':
    case 'j':
      turn = move_pc_direction(2, dest);
      break;

    case '1':
    case 'b':
      turn = move_pc_direction(1, dest);
      break;

    case '4':
    case 'h':
      turn = move_pc_direction(4, dest);
      break;

    case '5':
    case ' ':
    case '.':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn = 0;
      break;

    case '>':
      turn = move_pc_direction('>', dest);
      break;

    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn = 0;
      break;
      break;

    case 't':
      input_trainers_list();
      turn = 1;
      break;
    default:
      mvprintw(0, 0, "Invalud input: %#o ", key);
      turn = 1;
    }
    refresh();
  } while (turn);
}
