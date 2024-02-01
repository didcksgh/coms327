#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAP_X 80
#define MAP_Y 21

#define xy(x, y) (m->map[y][x])

typedef enum coor{
  coor_x,
  coor_y
} coor_t;


typedef int pair_t[2];


typedef enum  terrain_type {

  ter_boulder,
  ter_path,
  ter_grass,
  ter_clearing,
  ter_water,
  ter_pokemart,
  ter_pokecenter,
  ter_tree
} terrain_type_t;

typedef struct map {

  terrain_type_t map[MAP_Y][MAP_X];
  int n, s, e, w;
} map_t;

int set_boulders(map_t *m){
    int x, y;
    for(y = 0; y < MAP_Y; y++){
        m->map[y][0] = ter_boulder;
        m->map[y][MAP_X-1] = ter_boulder;        

    }


    for(x = 0; x < MAP_X; x++){
        m->map[0][x] = ter_boulder;
        m->map[MAP_Y-1][x] = ter_boulder;
        

    }
    return 0;
}

int set_gate(map_t *m, int n, int s, int e, int w){
    m->n = n;
    m->s = s;
    m->e = e;
    m->w = w;

    xy(n, 0) = ter_path;
    xy(n, 1) = ter_path;

    xy(s, MAP_Y - 1) = ter_path;
    xy(s, MAP_Y - 2) = ter_path;

    xy(MAP_X - 1, e) = ter_path;
    xy(MAP_X - 2, e) = ter_path;

    xy(0, w) = ter_path;
    xy(1, w) = ter_path;
    

  return 0;
    

}

void building_location(map_t *m, pair_t p){
    int can_place_building = 0;
    int is_not_path = 0;
    int is_not_building=0;

    do {
    p[coor_x] = rand() % (MAP_X - 3) + 1;
    p[coor_y] = rand() % (MAP_Y - 3) + 1;

    if(
        xy(p[coor_x], p[coor_y]) == ter_path && xy(p[coor_x]-1, p[coor_y]+1) == ter_path ||
        xy(p[coor_x]+2, p[coor_y]) == ter_path && xy(p[coor_x]+2, p[coor_y]+1) == ter_path ||
        xy(p[coor_x], p[coor_y]-1) == ter_path && xy(p[coor_x]+1, p[coor_y]-1) == ter_path ||
        xy(p[coor_x], p[coor_y]+2) == ter_path && xy(p[coor_x]+1, p[coor_y]+2) == ter_path

    ){
        can_place_building = 1;
    }

    if(
        xy(p[coor_x], p[coor_y]) != ter_path && xy(p[coor_x]+1, p[coor_y]) != ter_path &&
        xy(p[coor_x], p[coor_y]+1) != ter_path && xy(p[coor_x]+1, p[coor_y]+1) != ter_path
    ){
        is_not_path = 1;
    }

    if(
        xy(p[coor_x], p[coor_y]) != ter_pokemart && xy(p[coor_x], p[coor_y]) != ter_pokecenter &&
        xy(p[coor_x]+1, p[coor_y]) != ter_pokemart && xy(p[coor_x]+1, p[coor_y]) != ter_pokecenter &&
        xy(p[coor_x], p[coor_y]+1) != ter_pokemart && xy(p[coor_x], p[coor_y]+1) != ter_pokecenter &&
        xy(p[coor_x]+1, p[coor_y]+1) != ter_pokemart && xy(p[coor_x]+1, p[coor_y]+1) != ter_pokecenter 
    ){
        is_not_building = 1;
    }
    
    if(can_place_building == 1 && is_not_building == 1 && is_not_path == 1){
        break;

    }

    }while(1);



}

int place_market(map_t *m){
    pair_t p;

    building_location(m, p);

    xy(p[coor_x], p[coor_y]) = ter_pokemart;

    xy(p[coor_x], p[coor_y]+1) = ter_pokemart;

    xy(p[coor_x]+1, p[coor_y]) = ter_pokemart;
    
    xy(p[coor_x]+1, p[coor_y]+1) = ter_pokemart;
    
    
    return 0;



}

int place_pokecenter(map_t *m){
    pair_t p;

    building_location(m, p);

    xy(p[coor_x], p[coor_y]) = ter_pokecenter;

    xy(p[coor_x], p[coor_y]+1) = ter_pokecenter;

    xy(p[coor_x]+1, p[coor_y]) = ter_pokecenter;
    
    xy(p[coor_x]+1, p[coor_y]+1) = ter_pokecenter;
    

    return 0;

}


int build_paths(map_t *m){
    if(m->n-m->s == 0){
        for(int y = 2; y < MAP_Y-2; y++){
            xy(m->n, y) = ter_path;

        }
    }else if(m->n-m->s >0){
        for(int y = 2; y < 12; y++){
        xy(m->n, y) = ter_path;

        }
        for(int y = 12; y < MAP_Y-2; y++){
            xy(m->s,y) = ter_path;

        }
        for(int x = (m->s)+1; x < m->n; x++){
            xy(x, 11) = ter_path;

        }
    }else{
        
        for(int y = 2; y < 12; y++){
        xy(m->n, y) = ter_path;

    
        }
         for(int y = 11; y < MAP_Y-2; y++){
            xy(m->s,y) = ter_path;

        }
        for(int x = (m->n)+1; x < m->s; x++){
            xy(x, 11) = ter_path;

        }
        
    }

   
    if(m->n-m->s == 0){
        for(int x = 2; x < MAP_Y-2; x++){
            xy(x, MAP_Y-2) = ter_path;

        }
    }else if(m->w-m->e >0){
        for(int x = 2; x < 51; x++){
        xy(x, m->w) = ter_path;

        }
        for(int x = 50; x < MAP_X-2; x++){
            xy(x,m->e) = ter_path;

        }
        for(int y = (m->e)+1; y < m->w; y++){
            xy(50, y) = ter_path;

        }
    }else{
        
        for(int x = 2; x < 51; x++){
        xy(x, m->w) = ter_path;

        }
        for(int x = 50; x < MAP_X-2; x++){
            xy(x,m->e) = ter_path;

        }
        for(int y = (m->w)+1; y < m->e; y++){
            xy(50, y) = ter_path;

        }
        
    }
    


  return 0;
}


int generate_map(map_t *m){
    set_boulders(m);
    
    terrain_type_t type;
    int x, y;

    int count = 0;
    
    for(int i = 1; i < MAP_Y-1; i++){
        for(int j  = 1; j < MAP_X-1; j++){
            
                m->map[i][j] = ter_clearing;
            

        }

    }

    set_gate(m, 1 + rand() % (MAP_X - 2), 1 + rand() % (MAP_X - 2), 1 + rand() % (MAP_Y - 2), 1 + rand() % (MAP_Y - 2));

    pair_t p1;


    do{
        int seed = rand()%4;

        switch(seed){
            case 0:
            case 3:
            type = ter_grass;
            break;

            case 1:
            type = ter_water;
            break;

            case 2:
            type = ter_tree;
            break;


        }

        x = 1+rand()%(MAP_X-1);
        y = 1+rand()%(MAP_Y-1);

        if(x<=5 && y<=5){
            for(int i = 1; i < y+6; i++){
                for(int j = 1; j < x+6; j++){
                    if(xy(j,i) != ter_path){
                        xy(j,i) = type;
                    }
                    
                }


            }

        }

        if(x>5 && y<5){
            if(x+6 < MAP_X-1){
                for(int i = 1; i < y+6; i++){
                    for(int j = x-5; j < x+6; j++){
                        if(xy(j,i) != ter_path){
                            xy(j,i) = type;
                        }
                    }
                }


            }else{
            for(int i = 1; i < y+6; i++){
                for(int j = x-5; j < MAP_X-1; j++){
                    if(xy(j,i) != ter_path){
                        xy(j,i) = type;
                        }
                    }
                }
            }

        }

        if(x<5 && y>5){
            if(y+6 < MAP_Y-1){
                for(int i = y-5; i < y+6; i++){
                    for(int j = 1; j < x+6; j++){
                        if(xy(j,i) != ter_path){
                            xy(j,i) = type;
                        }
                    }
                }


            }else{
            for(int i = y-5; i < MAP_Y-1; i++){
                for(int j = 1; j < x+6; j++){
                    if(xy(j,i) != ter_path){
                        xy(j,i) = type;
                        }
                    }
                }
            }


        }

        if(x>5 && y>5){
            if (x +6 < MAP_X-1 && y+6 < MAP_Y-1)
            {
                 for(int i = y-5; i < y+6; i++){
                    for(int j = x-5; j < x+6; j++){
                        if(xy(j,i) != ter_path){
                            xy(j,i) = type;
                        }
                    }
                }
            }else if(x+6 < MAP_X-1 && y+6 >= MAP_Y-1){
                 for(int i = y-5; i < MAP_Y-1; i++){
                    for(int j = x-5; j < x+6; j++){
                        if(xy(j,i) != ter_path){
                            xy(j,i) = type;
                        }
                    }
                }

            }else if(x + 6 >= MAP_X-1 && y + 6 < MAP_Y-1){
                 for(int i = y-5; i < y+6; i++){
                    for(int j = x-5; j < MAP_X-1; j++){
                        if(xy(j,i) != ter_path){
                            xy(j,i) = type;
                        }
                    }
                }

            }
            

        }


        count++;

    }while(count < 10);

   build_paths(m);
   place_pokecenter(m);
   place_market(m);



    return 0;

}



void print_map(map_t *m){

    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            switch (m->map[y][x]) {
                case ter_boulder:
                putchar('%');
                break;
                
                case ter_path:
                putchar('#');
                break;

                case ter_grass:
                putchar(':');
                break;

                case ter_clearing:
                putchar('.');
                break;

                case ter_water:
                putchar('~');
                break;

                case ter_pokemart:
                putchar('M');
                break;

                case ter_pokecenter:
                putchar('C');
                break;

                case ter_tree:
                putchar('^');
                break;
            
      }
    }
    putchar('\n');
  }

}

int main(int argc, char* argv[]){

    map_t nm;

    srand((unsigned int)time(NULL));

    generate_map(&nm);

    print_map(&nm);

    return 0;

}