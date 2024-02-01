#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <cstring>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "pokemon.h"
#include "db_parse.h"


typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
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

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character * const *) v1;
  const character *const *c2 = (const character * const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static character *io_nearest_visible_trainer()
{
  character **c, *n;
  uint32_t x, y, count;

  c = (character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, BOULDER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, MOUNTAIN_SYMBOL);
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, TREE_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, FOREST_SYMBOL);
          attroff(COLOR_PAIR(COLOR_GREEN));
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
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, POKEMON_CENTER_SYMBOL);
          attroff(COLOR_PAIR(COLOR_RED));
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
        default:
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, ERROR_SYMBOL);
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at vector %d%cx%d%c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] == INT_MAX ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static void io_list_trainers_display(npc **c,
                                     uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = (char (*)[40]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", *s);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  npc **c;
  uint32_t x, y, count;

  c = (npc **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = dynamic_cast<npc *> (world.cur_map->cmap[y][x]);
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display(c, count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  mvprintw(0, 0, "Welcome to the Pokemart.  Could I interest you in some Pokeballs?");
  refresh();
  getch();
}

void io_pokemon_center()
{
  mvprintw(0, 0, "Welcome to the Pokemon Center.  How can Nurse Joy assist you?");
  refresh();
  getch();
}

int choose_move(pokemon *p){
  clear();
  int i = 0;

  mvprintw(0,0, "Choose a move: ");

  while(p->move_index[i]){
    mvprintw(i+1, 0, "%d. %s", i+1, moves[p->move_index[i]].identifier);
    i++;
  }

  refresh();

  int v = 0;
  int input;

  while(!v){
    input = getch();
    switch(input){
      case '1':
        return p->move_index[0];

      case '2':
        return p->move_index[1];

      default:
        mvprintw(5,0,"Invalid input!");
    }

  }



  return 1;

}

int get_trainers_move(pokemon *p){
  int count =0;
  while(p->move_index[count]){
    count++;

  }

  return p->move_index[rand()%count];

}

int get_move_priority(int pc_index, int npc_index, pokemon *p1, pokemon *p2){
  int pc_priority = moves[pc_index].priority;
  int npc_priority = moves[npc_index].priority;

  if(pc_priority == npc_priority){
    if(p1->get_speed() == p2->get_speed()){
      return (rand()%1);

    }else if(p1->get_speed() > p2->get_speed()){
      return 1;
    }else{
      return 0;
    }

  }else if(pc_priority > npc_priority){
      
      return 1;

  }else{
    return 0;
  }

}

bool move_hit(int i){
  if(rand()%100<moves[i].accuracy){
    return true;

  }else{
  return false;
  }
 
}

int get_dmg(pokemon *p_attack, pokemon *p_defense, int i){
  /**int dmg;
  if(move_hit(i)){
    dmg = ((p_attack->get_level()*2)/5)+2;
    dmg = (dmg * moves[i].power * (p_attack->get_atk())/p_defense->get_def());
    dmg = (dmg / 50) + 2;
   
   double critical;
   if(rand()% 256 < (p_attack->s->base_stat[stat_speed]/2)){
    critical = 1.5;

   }else{
    critical = 1;
   }

   double type = 1;

   double stab;

   if(p_attack->pt->type_id == moves[i].type_id){
    stab = 1.5;

   }else{
    stab =1;
   }

   int r = (rand() % (100 + 1 - 85) + 85)/100;

   dmg = (int)(double)dmg*critical*r*stab*type;

   if(dmg < 0){
    return 1;
     
   }else{
    return dmg;
   }

  }else{
    return 0;
  }**/
  return 5;
}

int dmg_cal(pokemon *p, int dmg){
  int left_hp = p->effective_stat[stat_hp] - dmg;

  if(left_hp >= 0){

     p->effective_stat[stat_hp] = left_hp;
 
    
    return left_hp;

  }

  p->effective_stat[stat_hp] = 0;
 
 

  return 0;
}

int show_bag(){
  clear();
  mvprintw(0,0, "Choose the item to use: ");
  mvprintw(1,0, "1. Revive x %d", world.pc.num_rev);
  mvprintw(2,0, "2. Potion x %d", world.pc.num_potion);
  mvprintw(3,0, "3. Pokeball x %d", world.pc.num_pokeball);
  refresh();

  int v = 0;
  int input;

  while(!v){
    input = getch();

    switch (input)
    {
    case '1':
    case '2':
    case '3':

    return input;
    
    default:
      mvprintw(5,0, "Invalid input.");
    }
  }
  return 0;
}

void normal_fight(int priority, int pc_damage, int npc_damage, int pc_move, int npc_move, pokemon *pc_pokemon, pokemon *npc_pokemon){
  clear();
  pc_damage = get_dmg(pc_pokemon, npc_pokemon, pc_move);
  npc_damage = get_dmg(npc_pokemon, pc_pokemon, npc_move);

  //pc first
  if(priority){
    if(pc_damage > 0){
      mvprintw(0,0, "%s used %s. It does %d damage!", pc_pokemon->get_species(), moves[pc_move].identifier, pc_damage);
      mvprintw(1,0, "Press any key...");
      refresh();
      getch();
      dmg_cal(npc_pokemon, pc_damage);

    }else{
      clear();
      mvprintw(0,0, "%s used %s. It missed!", pc_pokemon->get_species(), moves[pc_move].identifier);
      mvprintw(1,0, "Press any key...");
      refresh();
      getch();
      dmg_cal(npc_pokemon, pc_damage);
    }
    if(npc_pokemon->get_hp() > 0){
      
      if(npc_damage>0){
        clear();
        mvprintw(0,0, "%s used %s. It does %d damage!", npc_pokemon->get_species(), moves[npc_move].identifier, npc_damage);
        mvprintw(1,0, "Press any key...");
        refresh();
        getch();
        dmg_cal(pc_pokemon, npc_damage);

      }else{
        clear();
        mvprintw(0,0, "%s used %s. It missed!", npc_pokemon->get_species(), moves[npc_move].identifier);
        mvprintw(1,0, "Press any key...");
        refresh();
        getch();
        dmg_cal(pc_pokemon, pc_damage);
      }

    }


  }
  else{
    //npc first
    if(npc_damage>0){
      mvprintw(0,0, "%s used %s. It does %d damage!", npc_pokemon->get_species(), moves[npc_move].identifier, npc_damage);
      mvprintw(1,0, "Press any key...");
      refresh();
      getch();
      dmg_cal(pc_pokemon, npc_damage);

    }else{
      clear();
      mvprintw(0,0, "%s used %s. It missed!", npc_pokemon->get_species(), moves[npc_move].identifier);
      mvprintw(1,0, "Press any key...");
      refresh();
      getch();
      dmg_cal(pc_pokemon, npc_damage);
    }
    if(pc_pokemon->get_hp() > 0){
      if(pc_damage > 0){
        mvprintw(0,0, "%s used %s. It does %d damage!", pc_pokemon->get_species(), moves[pc_move].identifier, pc_damage);
        mvprintw(1,0, "Press any key...");
        refresh();
        getch();
        dmg_cal(npc_pokemon, pc_damage);

      }else{
        clear();
        mvprintw(0,0, "%s used %s. It missed!", pc_pokemon->get_species(), moves[pc_move].identifier);
        mvprintw(1,0, "Press any key...");
        refresh();
        getch();
        dmg_cal(npc_pokemon, pc_damage);
      }

    }

  }

}

void item_used_fight(int npc_damage, int npc_move, pokemon *pc_pokemon, pokemon *npc_pokemon){
  clear();

  if(npc_damage > 0){
    mvprintw(0,0, "%s used %s. It does %d damage!", npc_pokemon->get_species(), moves[npc_move].identifier, npc_damage);
    mvprintw(1,0, "Press any key...");
    refresh();
    getch();
    dmg_cal(pc_pokemon, npc_damage);

  }else{
    clear();
    mvprintw(0,0, "%s used %s. It missed!", npc_pokemon->get_species(), moves[npc_move].identifier);
    mvprintw(1,0, "Press any key...");
    refresh();
    getch();
    dmg_cal(pc_pokemon, npc_damage);
  }

}

void use_revive(pokemon *p){
  clear();
  int ihp = p->get_hp();
  int rhp = p->initial_hp/2;

  p->effective_stat[stat_hp] = rhp;

  mvprintw(0,0, "You used revive. %s restore its hp from %d to %d.", p->get_species(), ihp, p->effective_stat[stat_hp]);
  mvprintw(1,0, "Press any key...");
  refresh();
  getch();

  world.pc.num_rev--;



}

void choose_revive(){
  clear();
  mvprintw(0,0, "Choose the pokemon you want to revive!");
  for(int i = 0; i < 6 && world.pc.buddy[i]; i++){
    mvprintw(i+1, 0, "%d. %s | HP: %d", i+1, world.pc.buddy[i]->get_species(), world.pc.buddy[i]->get_hp());

  }

  refresh();
  int input = getchar();
  int v = 0;
  int index = -1;

  while(!v){
    refresh();
    switch (input)
    {
    case '1':
      if(world.pc.buddy[0]){
        if(world.pc.buddy[0]->get_hp() == 0){
        index = 0;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[0]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;
    case '2':
    if(world.pc.buddy[1]){
       if(world.pc.buddy[1]->get_hp() == 0){
        index = 1;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[1]->get_species());
          refresh();
          getchar();
          v = 1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;
    
    case '3':
    if(world.pc.buddy[2]){
        if(world.pc.buddy[2]->get_hp() == 0){
        index = 2;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[2]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;
    
    case '4':
      if(world.pc.buddy[3]){
        if(world.pc.buddy[3]->get_hp() == 0){
        index = 3;
        v = 1;
        }else{
         clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[3]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;
    
    case '5':
      if(world.pc.buddy[4]){
        if(world.pc.buddy[4]->get_hp() == 0){
        index = 4;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[4]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;

    case '6':
    if(world.pc.buddy[5]){
       if(world.pc.buddy[5]->get_hp() == 0){
        index = 5;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[5]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;

    }

  }
  if(index != -1){
    use_revive(world.pc.buddy[index]);

  }
  

}

void use_potion(pokemon *p){
  clear();
  int ihp = p->get_hp();
  int rhp;
  if(ihp + 20 > p->initial_hp){
    rhp = p->initial_hp;

  }else{
    rhp = ihp+20;
  }

  p->effective_stat[stat_hp] = rhp;

  mvprintw(0,0, "You used potion. %s restores its hp from %d to %d.", p->get_species(), ihp, p->effective_stat[stat_hp]);
  mvprintw(1,0, "Press any key...");
  refresh();
  getch();

  world.pc.num_potion--;
}

void choose_potion(){
  clear();
  mvprintw(0,0, "Choose the pokemon you want to revive!");
  for(int i = 0; i < 6 && world.pc.buddy[i]; i++){
    mvprintw(i+1, 0, "%d. %s | HP: %d", i+1, world.pc.buddy[i]->get_species(), world.pc.buddy[i]->get_hp());

  }

  refresh();
  int input = getchar();
  int v = 0;
  int index = -1;

  while(!v){
    refresh();
    switch (input)
    {
    case '1':
      if(world.pc.buddy[0]){
        if(world.pc.buddy[0]->get_hp() < world.pc.buddy[0]->initial_hp){
        index = 0;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[0]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;
    case '2':
    if(world.pc.buddy[1]){
       if(world.pc.buddy[1]->get_hp() < world.pc.buddy[1]->initial_hp){
        index = 1;
        v = 1;
        }else{
          mvprintw(8,0, "%s is healthy!", world.pc.buddy[1]->get_species());
          refresh();
          getchar();
        }

      }else{
       clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[1]->get_species());
          refresh();
          getchar();
          v=1;
      }
      break;
    
    case '3':
    if(world.pc.buddy[2]){
        if(world.pc.buddy[2]->get_hp() < world.pc.buddy[2]->initial_hp){
        index = 2;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[2]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;
    
    case '4':
      if(world.pc.buddy[3]){
        if(world.pc.buddy[3]->get_hp() < world.pc.buddy[3]->initial_hp){
        index = 3;
        v = 1;
        }else{
         clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[3]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;
    
    case '5':
      if(world.pc.buddy[4]){
        if(world.pc.buddy[4]->get_hp() < world.pc.buddy[4]->initial_hp){
        index = 4;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[4]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;

    case '6':
    if(world.pc.buddy[5]){
       if(world.pc.buddy[5]->get_hp() < world.pc.buddy[5]->initial_hp){
        index = 5;
        v = 1;
        }else{
          clear();
          mvprintw(0,0, "%s is healthy!", world.pc.buddy[5]->get_species());
          refresh();
          getchar();
          v=1;
        }

      }else{
        mvprintw(8,0, "Invalid input!");
        refresh();
        input = getchar();
      }
      break;

    }

  }
  if(index != -1){
    use_potion(world.pc.buddy[index]);

  }
  
  
}

pokemon * switch_pokemon(){
  clear();
  int i =0;
  mvprintw(0,0, "Choose a pokemon to use!");
  for(i = 0; i < 6 && world.pc.buddy[i]; i++){
    mvprintw(i+1, 0, "%d. %s | HP: %d", i+1, world.pc.buddy[i]->get_species(), world.pc.buddy[i]->get_hp());

  }
  refresh();
  int input = getch();
  int v = 0;

  while(!v){
    refresh();
    switch(input){
      case '1':
        if(i >=0){
          if(world.pc.buddy[0]->get_hp() > 0){
            v=1;

          }else{
            mvprintw(8,0,"%s has been fainted. Please, choose other.", world.pc.buddy[0]->get_species());
            refresh();
            input = getch();
          }

        }else{
          mvprintw(8,0,"Invalid input! Please, type again.");
          refresh();
          input = getch();
        }
        break;

      case '2':
        if(i >= 2){
          if(world.pc.buddy[1]->get_hp() > 0){
            v=1;

          }else{
            mvprintw(8,0,"%s has been fainted. Please, choose other.", world.pc.buddy[1]->get_species());
            refresh();
            input = getch();
          }
        

        }else{
          mvprintw(8,0,"Invalid input! Please, type again.");
          refresh();
          input = getch();
          
        }
        break;

      case '3':
        if(i >= 3){
          if(world.pc.buddy[2]->get_hp() > 0){
            v=1;

          }else{
            mvprintw(8,0,"%s has been fainted. Please, choose other.", world.pc.buddy[2]->get_species());
            refresh();
            input = getch();
          }
        }else{
          mvprintw(8,0,"Invalid input! Please, type again.");
          refresh();
          input = getch();
        }
        break;

      case '4':
        if(i >= 4){
          if(world.pc.buddy[3]->get_hp() > 0){
            v=1;

          }else{
            mvprintw(8,0,"%s has been fainted. Please, choose other.", world.pc.buddy[3]->get_species());
            refresh();
            input = getch();
          }
        }else{
          mvprintw(8,0,"Invalid input! Please, type again.");
          refresh();
          input = getch();
        }
        break;

      case '5':
        if(i >= 5){
          if(world.pc.buddy[4]->get_hp() > 0){
            v=1;

          }else{
            mvprintw(8,0,"%s has been fainted. Please, choose other.", world.pc.buddy[4]->get_species());
            refresh();
            input = getch();
          }
        }else{
          mvprintw(8,0,"Invalid input! Please, type again.");
          refresh();
          input = getch();
        }
        break;

      case '6':
        if(i >= 6){
          if(world.pc.buddy[5]->get_hp() > 0){
            v=1;

          }else{
            mvprintw(8,0,"%s has been fainted. Please, choose other.", world.pc.buddy[5]->get_species());
            refresh();
            input = getch();
          }
        }else{
          mvprintw(8,0,"Invalid input! Please, type again.");
          refresh();
          input = getch();
        }
        break;
    }

  }

  return world.pc.buddy[input];
}



void io_battle(character *aggressor, character *defender)
{
  npc *n;
  if(!(n = dynamic_cast<npc *>(aggressor))){
    n = dynamic_cast<npc *>(defender);

  }
  //int i;

  pokemon *pc_p = world.pc.buddy[0];
  pokemon *npc_np = n->buddy[0];

  clear();

  mvprintw(0,0, "You are challeneged by %c", n->symbol);
  mvprintw(1,0, "Press any key to continue...");
  refresh();
  getch();

  clear();

    do{
    if(npc_np->get_hp() == 0){
      char ex_poke[40];
      strcpy(ex_poke, npc_np->get_species());
      npc_np = n->get_next();
      clear();
      mvprintw(0,0,"You knocked out %s, %s enters the battle!", ex_poke, npc_np->get_species());
      mvprintw(1,0, "Press any key to continue...");
      refresh();
      getch();
    }
    

    if(pc_p->get_hp() == 0){
      clear();
      mvprintw(0,0,"%s has been fainted!", pc_p->get_species());
      mvprintw(1,0, "Press any key to continue...");
      refresh();
      getch();


    
      pc_p = switch_pokemon();
    }
    
    clear();
    mvprintw(0,0, "Opponent's Pokemon:");
    mvprintw(1,0,"%s LV:%d", npc_np->get_species(), npc_np->get_level());
    mvprintw(2,0, "HP: %d", npc_np->get_hp());

    mvprintw(4,0, "Your Pokemon:");
    mvprintw(5,0,"%s LV:%d", pc_p->get_species(), pc_p->get_level());
    mvprintw(6,0, "HP: %d", pc_p->get_hp());

    mvprintw(8,0, "Choose an option:");
    
    mvprintw(10,0, "1 - Fight | 2 - Bag | 3 - Run | 4 - Pokemon");
    refresh();

    int input;
    input = getch();
    int pc_move;
    int npc_move;
    int mp;
    int pc_damage;
    int npc_damage;
    switch (input){
      case '1':
        pc_move = choose_move(pc_p);
        npc_move =get_trainers_move(npc_np);
        mp = get_move_priority(pc_move, npc_move, pc_p, npc_np);
        pc_damage = dmg_cal(pc_p, pc_move);
        npc_damage = dmg_cal(npc_np, npc_move);
        normal_fight(mp, pc_damage, npc_damage, pc_move, npc_move, pc_p, npc_np);
        break;
      case '2':
        int item;
        item = show_bag();
        npc_move = get_trainers_move(npc_np);
        npc_damage = dmg_cal(npc_np, npc_move);
        switch (item){
          case '1':
              if(world.pc.num_rev >0){
                if(world.pc.buddy[0]){
                  choose_revive();
                  item_used_fight(npc_damage, npc_move, npc_np, pc_p);
                }

              else{
                clear();
                mvprintw(0, 0, "None of your pokemon have fainted!");
                mvprintw(1, 0, "Press any key to continue...");
                refresh();
                getch();
              }
              
            }
            else{
              clear();
              mvprintw(0, 0, "You don't have a revives!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            
            break;
          case '2':
            if(world.pc.num_potion >0){
              if(world.pc.buddy[0]){
                choose_potion();
                item_used_fight(npc_damage, npc_move, npc_np, pc_p);

              }
             
              
            }
            else{
              clear();
              mvprintw(0, 0, "You don't have a potions!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            break;
          case '3':
          clear();
            mvprintw(0, 0, "You can't capture other trainer's Pokemon!");
            mvprintw(1, 0, "Press any key to continue...");
            refresh();
            getch();
            
   
            break;
        }
        break;
      case '3':
        clear();
        mvprintw(0, 0, "You can't run from a trainer battle!");
        mvprintw(1, 0, "Press any key to continue...");
        refresh();
        getch();
        break;
      case '4':
  
        clear();
        mvprintw(0,0,"You brought %s back!", pc_p->get_species());
        mvprintw(1,0, "Press any key to continue...");
        refresh();
        getch();

        for(int i = 1; i < 6 && world.pc.buddy[i]; i++){
          if(world.pc.buddy[i]->get_hp()>0){
              pc_p = switch_pokemon();
              npc_move = get_trainers_move(npc_np);
              npc_damage = dmg_cal(npc_np, npc_move);
              item_used_fight(npc_damage, npc_move, npc_np, pc_p);
          }
            mvprintw(3,0, "There's no left active pokemon.");
            mvprintw(4,0, "Press any key to continue...");
            refresh();
            getch();
        }
        
        
       
        break;
      
      default:
        mvprintw(13, 0, "%c is not a valid input!", input);
        refresh();
        break;
    }


  }while( (!n->is_battle_end()) && (!world.pc.is_battle_end()));

  io_display();
  if(!world.pc.is_battle_end()){
    mvprintw(0, 0, "Aww, how'd you get so strong?  You and your pokemon must share a special bond!");
  }
  else {
    mvprintw(0, 0, "Never give up! You got something special!");
  }
  
  refresh();
  getch();
  
  
  n->defeated = 1;
  if (n->ctype == char_hiker || n->ctype == char_rival) {
    n->mtype = move_wander;
  }



/**
  clear();
  mvprintw(0, 0, "My pokemon are: ");
  printw("%s", n->buddy[0]->get_species());
  for (i = 1; i < 6 && n->buddy[i]; i++) {
    printw(", %s", n->buddy[i]->get_species());
  }
  refresh();
  getch();

  n->defeated = 1;
  if (n->ctype == char_hiker || n->ctype == char_rival) {
    n->mtype = move_wander;
  }
  **/

}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
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
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<npc *> (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if ((dynamic_cast<npc *>
                (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]))) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
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

void io_teleport_world(pair_t dest)
{
  /* mvscanw documentation is unclear about return values.  I believe *
   * that the return value works the same way as scanf, but instead   *
   * of counting on that, we'll initialize x and y to out of bounds   *
   * values and accept their updates only if in range.                */
  int x = INT_MAX, y = INT_MAX;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  echo();
  curs_set(1);
  do {
    mvprintw(0, 0, "Enter x [-200, 200]:           ");
    refresh();
    mvscanw(0, 21, "%d", &x);
  } while (x < -200 || x > 200);
  do {
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, "%d", &y);
  } while (y < -200 || y > 200);

  refresh();
  noecho();
  curs_set(0);

  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'p':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'f':
      /* Fly to any map in the world.                                */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;    

    case 'B':
      int item;
      item = show_bag();
      switch (item)
      {
      case '1':
      if(world.pc.num_rev > 0){
        if(world.pc.buddy[0]){
          choose_revive();

        }

        }else{
          clear();
          mvprintw(0,0, "You don't have a revive!");
          mvprintw(1,0, "Press any key...");
          refresh();
          getch();
         
        }
        
        break;

      case '2':
        if(world.pc.num_potion > 0){
          clear();
          if(world.pc.buddy[0]){
            choose_potion();

          }
          
        }else{
          clear();
          mvprintw(0,0, "You don't have a potion!");
          mvprintw(1,0, "Press any key...");
          refresh();
          getch();
          
        }
        break;

        case '3':
          clear();
          mvprintw(0,0, "There's no pokemon to catch!");
          mvprintw(1,0, "Press any key...");
          refresh();
          getch();
          break;

      }


    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}

void io_encounter_pokemon()
{
  pokemon *p;
  pokemon *pc_p = world.pc.buddy[0];

  p = new pokemon();
 io_queue_message("%s%s%s: HP:%d ATK:%d DEF:%d SPATK:%d SPDEF:%d SPEED:%d %s",
                   p->is_shiny() ? "*" : "", p->get_species(),
                   p->is_shiny() ? "*" : "", p->get_hp(), p->get_atk(),
                   p->get_def(), p->get_spatk(), p->get_spdef(),
                   p->get_speed(), p->get_gender_string());
  io_queue_message("%s's moves: %s %s", p->get_species(),
                   p->get_move(0), p->get_move(1));

  // Later on, don't delete if captured
  do{
    if(p->get_hp() == 0){
      clear();
      mvprintw(0,0,"You knocked out %s!", p->get_species());
      mvprintw(1,0, "Press any key to continue...");
      refresh();
      getch();

      delete p;
    }
    

    if(pc_p->get_hp() == 0){
      clear();
      mvprintw(0,0,"%s has been fainted!", pc_p->get_species());
      mvprintw(1,0, "Press any key to continue...");
      refresh();
      getch();


    
      pc_p = switch_pokemon();
    }
    
    clear();
    mvprintw(0,0, "wild Pokemon:");
    mvprintw(1,0,"%s LV:%d", p->get_species(), p->get_level());
    mvprintw(2,0, "HP: %d", p->get_hp());

    mvprintw(4,0, "Your Pokemon:");
    mvprintw(5,0,"%s LV:%d", pc_p->get_species(), pc_p->get_level());
    mvprintw(6,0, "HP: %d", pc_p->get_hp());

    mvprintw(8,0, "Choose an option:");
    
    mvprintw(10,0, "1 - Fight | 2 - Bag | 3 - Run | 4 - Pokemon");
    refresh();

    int input;
    input = getch();
    int pc_move;
    int npc_move;
    int mp;
    int pc_damage;
    int npc_damage;
    switch (input){
      case '1':
        pc_move = choose_move(pc_p);
        npc_move =get_trainers_move(p);
        mp = get_move_priority(pc_move, npc_move, pc_p, p);
        pc_damage = dmg_cal(pc_p, pc_move);
        npc_damage = dmg_cal(p, npc_move);
        normal_fight(mp, pc_damage, npc_damage, pc_move, npc_move, pc_p, p);
        break;
      case '2':
        int item;
        item = show_bag();
        npc_move = get_trainers_move(p);
        npc_damage = dmg_cal(p, npc_move);
        switch (item){
          case '1':
              if(world.pc.num_rev >0){
                if(world.pc.buddy[0]){
                  choose_revive();
                  item_used_fight(npc_damage, npc_move, p, pc_p);
                }

              else{
                clear();
                mvprintw(0, 0, "None of your pokemon have fainted!");
                mvprintw(1, 0, "Press any key to continue...");
                refresh();
                getch();
              }
              
            }
            else{
              clear();
              mvprintw(0, 0, "You don't have a revives!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            
            break;
          case '2':
            if(world.pc.num_potion >0){
              if(world.pc.buddy[0]){
                choose_potion();
                item_used_fight(npc_damage, npc_move, p, pc_p);

              }
             
              
            }
            else{
              clear();
              mvprintw(0, 0, "You don't have a potions!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            break;
          case '3':
          clear();
            for (int a = 0; a < 6; a++)
            {
              if(world.pc.buddy[a] == NULL){
                world.pc.buddy[a] = p;
                mvprintw(0, 0, "You captured %s!", p->get_species());
                break;

              }
            }
            clear();
            mvprintw(0,0, "You already have 6 pokemon in your pocket!");
            mvprintw(1, 0, "Press any key to continue...");
          
            refresh();
            getch();
            io_display();
            break;
        }
        break;
      case '3':
        clear();
        mvprintw(0, 0, "You have run from %s!", p->get_species());
        mvprintw(1, 0, "Press any key to continue...");
        refresh();
        getch();
        delete p;
        io_display();
        break;

      case '4':
  
        clear();
        mvprintw(0,0,"You brought %s back!", pc_p->get_species());
        mvprintw(1,0, "Press any key to continue...");
        refresh();
        getch();

        for(int i = 1; i < 6 && world.pc.buddy[i]; i++){
          if(world.pc.buddy[i]->get_hp()>0){
              pc_p = switch_pokemon();
              npc_move = get_trainers_move(p);
              npc_damage = dmg_cal(p, npc_move);
              item_used_fight(npc_damage, npc_move, p, pc_p);
          }
            mvprintw(3,0, "There's no left active pokemon.");
            mvprintw(4,0, "Press any key to continue...");
            refresh();
            getch();
        }
        
        
       
        break;
      
      default:
        mvprintw(13, 0, "%c is not a valid input!", input);
        refresh();
        break;
    }


  }while(!world.pc.is_battle_end());

  io_display();

}

void io_choose_starter()
{
  class pokemon *choice[3];
  int i;
  bool again = true;

  for(int j = 0; j < 6; j++){
    world.pc.buddy[j] = NULL;

  }
  
  choice[0] = new class pokemon();
  choice[1] = new class pokemon();
  choice[2] = new class pokemon();

  echo();
  curs_set(1);
  do {
    mvprintw( 4, 20, "Before you are three Pokemon, each of");
    mvprintw( 5, 20, "which wants absolutely nothing more");
    mvprintw( 6, 20, "than to be your best buddy forever.");
    mvprintw( 8, 20, "Unfortunately for them, you may only");
    mvprintw( 9, 20, "pick one.  Choose wisely.");
    mvprintw(11, 20, "   1) %s", choice[0]->get_species());
    mvprintw(12, 20, "   2) %s", choice[1]->get_species());
    mvprintw(13, 20, "   3) %s", choice[2]->get_species());
    mvprintw(15, 20, "Enter 1, 2, or 3: ");

    refresh();
    i = getch();
    
    if (i == '1' || i == '2' || i == '3') {
      world.pc.buddy[0] = choice[(i - '0') - 1];
      delete choice[(i - '0') % 3];
      delete choice[((i - '0') + 1) % 3];
      again = false;
    }
  } while (again);
  noecho();
  curs_set(0);
}

