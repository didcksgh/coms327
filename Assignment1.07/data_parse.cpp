#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <climits>
#include <fstream>
#include <iostream>


#include "data_parse.h"

pokemon_move_data pokemon_moves[528239];
pokemon_data pokemon[1093];
char* types[19];
move_data moves[845];
pokemon_species_data species[899];
experience_data experience[601];
pokemon_stats_data pokemon_stats[6553];
stats_data stats[9];
pokemon_types_data pokemon_types[1676];

static char *next_char(char *start, char d){
    int i;
    //must not change between the function calls
    static char *s;

    if(start){
        s = start;

    }

    start = s;

    for(i = 0; s[i] && s[i] != d; i++);

    s[i] = '\0';
    s = (s+i) + 1;

    return start;

}
static const char *int_to_string(int i){
    static int cur = 0;
    static char s[20][20];

    if(cur == 20){
        cur = 0;
    }

    if(i == INT_MAX){
        s[cur][0] = '\0';
    }else{
        sprintf(s[cur], "%d", i);
    }

    return s[cur++];
}

void data_parse(){
    FILE *file;
    char line[800];
    int i, j, count;
    struct stat buf;
    char *address;
    int address_len;

    char *tmp;

    i = (strlen(getenv("HOME")) +
       strlen("/.poke327/pokedex/pokedex/data/csv/") + 1);
    address = (char *) malloc(i);
    strcpy(address, getenv("HOME"));
    strcat(address, "/.poke327/pokedex/pokedex/data/csv/");

    if (stat(address, &buf)) {
     free(address);
     address = NULL;
     }

    if (!address && !stat("/share/cs327", &buf)) {
        address = strdup("/share/cs327/pokedex/pokedex/data/csv/");
    }

    address_len = strlen(address);

    address = (char *) realloc(address, address_len + strlen("pokemon.csv") + 1);

    strcpy(address + address_len, "pokemon.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 80, file);
  
    for (i = 1; i < 1093; i++) {
        fgets(line, 80, file);
        pokemon[i].id = atoi(next_char(line, ','));
        strncpy(pokemon[i].identifier, next_char(NULL, ','), 30);
        pokemon[i].species_id = atoi(next_char(NULL, ','));
        pokemon[i].height = atoi(next_char(NULL, ','));
        pokemon[i].weight = atoi(next_char(NULL, ','));
        pokemon[i].base_experience = atoi(next_char(NULL, ','));
        pokemon[i].order = atoi(next_char(NULL, ','));
        pokemon[i].is_default = atoi(next_char(NULL, ','));
    }  

    fclose(file);

    file = fopen("pokemon.csv", "w");
    for (i = 1; i < 1093; i++) {
      fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s\n",
              int_to_string(pokemon[i].id),
              pokemon[i].identifier,
              int_to_string(pokemon[i].species_id),
              int_to_string(pokemon[i].height),
              int_to_string(pokemon[i].weight),
              int_to_string(pokemon[i].base_experience),
              int_to_string(pokemon[i].order),
              int_to_string(pokemon[i].is_default));
    }
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("moves.csv") + 1);

    strcpy(address + address_len, "moves.csv");

    file = fopen(address, "r");

    

    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 845; i++) {
        fgets(line, 800, file);
        moves[i].id = atoi((tmp = next_char(line, ',')));
        strcpy(moves[i].identifier, (tmp = next_char(NULL, ',')));
        
        tmp = next_char(NULL, ',');

        if (*tmp) {
        moves[i].generation_id = atoi(tmp);
        } else {
        moves[i].generation_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].type_id = atoi(tmp);
        } else {
        moves[i].type_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].power = atoi(tmp);
        } else {
        moves[i].power = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].pp = atoi(tmp);
        } else {
        moves[i].pp = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].accuracy = atoi(tmp);
        } else {
        moves[i].accuracy = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].priority = atoi(tmp);
        } else {
        moves[i].priority = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].target_id = atoi(tmp);
        } else {
        moves[i].target_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].damage_class_id = atoi(tmp);
        } else {
        moves[i].damage_class_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].effect_id = atoi(tmp);
        } else {
        moves[i].effect_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].effect_chance = atoi(tmp);
        } else {
        moves[i].effect_chance= INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].contest_type_id = atoi(tmp);
        } else {
        moves[i].contest_type_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].contest_effect_id = atoi(tmp);
        } else {
        moves[i].contest_effect_id= INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        moves[i].super_contest_effect_id = atoi(tmp);
        } else {
        moves[i].super_contest_effect_id = INT_MAX;   
        }
  }


    fclose(file);

    file = fopen("moves.csv", "w");
    for (i = 1; i < 845; i++) {
      fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s\n",
              int_to_string(moves[i].id),
              moves[i].identifier,
              int_to_string(moves[i].generation_id),
              int_to_string(moves[i].type_id),
              int_to_string(moves[i].power),
              int_to_string(moves[i].pp),
              int_to_string(moves[i].accuracy),
              int_to_string(moves[i].priority),
              int_to_string(moves[i].target_id),
              int_to_string(moves[i].damage_class_id),
              int_to_string(moves[i].effect_id),
              int_to_string(moves[i].effect_chance),
              int_to_string(moves[i].contest_type_id),
              int_to_string(moves[i].contest_effect_id),
              int_to_string(moves[i].super_contest_effect_id));
    }
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("pokemon_moves.csv") + 1);

    strcpy(address + address_len, "pokemon_moves.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 528239; i++) {
        fgets(line, 800, file);
        tmp = next_char(line, ',');
        if (*tmp) {
        pokemon_moves[i].pokemon_id = atoi(tmp);
        } else {
        pokemon_moves[i].pokemon_id= INT_MAX;   
        }
        
        tmp = next_char(NULL, ',');
        if (*tmp) {
        pokemon_moves[i].version_group_id = atoi(tmp);
        } else {
        pokemon_moves[i].version_group_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        pokemon_moves[i].move_id = atoi(tmp);
        } else {
        pokemon_moves[i].move_id= INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        pokemon_moves[i].pokemon_move_method_id = atoi(tmp);
        } else {
        pokemon_moves[i].pokemon_move_method_id = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        pokemon_moves[i].level = atoi(tmp);
        } else {
        pokemon_moves[i].level = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        pokemon_moves[i].order = atoi(tmp);
        } else {
        pokemon_moves[i].order = INT_MAX;   
        }
     }
    fclose(file);

    file = fopen("pokemon_moves.csv", "w");
    for (i = 1; i < 528239; i++) {
      fprintf(file, "%s,%s,%s,%s,%s,%s\n",
              int_to_string(pokemon_moves[i].pokemon_id),
              int_to_string(pokemon_moves[i].version_group_id),
              int_to_string(pokemon_moves[i].move_id),
              int_to_string(pokemon_moves[i].pokemon_move_method_id),
              int_to_string(pokemon_moves[i].level),
              int_to_string(pokemon_moves[i].order));
    }
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("pokemon_species.csv") + 1);

    strcpy(address + address_len, "pokemon_species.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 899; i++) {
        fgets(line, 800, file);
        species[i].id = atoi((tmp = next_char(line, ',')));
        strcpy(species[i].identifier, (tmp = next_char(NULL, ',')));

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].generation_id= atoi(tmp);
        } else {
        species[i].generation_id= INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].evolves_from_species_id= atoi(tmp);
        } else {
        species[i].evolves_from_species_id= INT_MAX;  
        } 

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].evolution_chain_id= atoi(tmp);
        } else {
        species[i].evolution_chain_id= INT_MAX;  
        } 

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].color_id= atoi(tmp);
        } else {
        species[i].color_id= INT_MAX; 
        }  


        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].shape_id = atoi(tmp);
        } else {
        species[i].shape_id = INT_MAX;   
        }
        
        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].habitat_id= atoi(tmp);
        } else {
        species[i].habitat_id= INT_MAX;   
        }
        

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].gender_rate = atoi(tmp);
        } else {
        species[i].gender_rate = INT_MAX; 
        }  
        

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].capture_rate= atoi(tmp);
        } else {
        species[i].capture_rate= INT_MAX; 
        }  
        

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].base_happiness = atoi(tmp);
        } else {
        species[i].base_happiness= INT_MAX; 
        }  
        

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].is_baby= atoi(tmp);
        } else {
        species[i].is_baby= INT_MAX;   
        }
        

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].hatch_counter= atoi(tmp);
        } else {
        species[i].hatch_counter = INT_MAX; 
        }  
        

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].has_gender_differences= atoi(tmp);
        } else {
        species[i].has_gender_differences= INT_MAX; 
        }  

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].growth_rate_id= atoi(tmp);
        } else {
        species[i].growth_rate_id= INT_MAX;   
        }
        

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].forms_switchable= atoi(tmp);
        } else {
        species[i].forms_switchable= INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].is_legendary= atoi(tmp);
        } else {
        species[i].is_legendary = INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].is_mythical= atoi(tmp);
        } else {
        species[i].is_mythical= INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].order= atoi(tmp);
        } else {
        species[i].order= INT_MAX;   
        }

        tmp = next_char(NULL, ',');
        if (*tmp) {
        species[i].conquest_order= atoi(tmp);
        } else {
        species[i].conquest_order= INT_MAX;   
        }
  }

    fclose(file);

    file = fopen("pokemon_species.csv", "w");
    for (i = 1; i < 899; i++) {
      fprintf(file,
              "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
              int_to_string(species[i].id),
              species[i].identifier,
              int_to_string(species[i].generation_id),
              int_to_string(species[i].evolves_from_species_id),
              int_to_string(species[i].evolution_chain_id),
              int_to_string(species[i].color_id),
              int_to_string(species[i].shape_id),
              int_to_string(species[i].habitat_id),
              int_to_string(species[i].gender_rate),
              int_to_string(species[i].capture_rate),
              int_to_string(species[i].base_happiness),
              int_to_string(species[i].is_baby),
              int_to_string(species[i].hatch_counter),
              int_to_string(species[i].has_gender_differences),
              int_to_string(species[i].growth_rate_id),
              int_to_string(species[i].forms_switchable),
              int_to_string(species[i].is_legendary),
              int_to_string(species[i].is_mythical),
              int_to_string(species[i].order),
              int_to_string(species[i].conquest_order));
    }
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("experience.csv") + 1);

    strcpy(address + address_len, "experience.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 601; i++) {
    fgets(line, 800, file);
    experience[i].growth_rate_id = atoi((tmp = next_char(line, ',')));
    tmp = next_char(NULL, ',');
    if (*tmp) {
        experience[i].level= atoi(tmp);
        } else {
        experience[i].level= INT_MAX;   
    }
    tmp = next_char(NULL, ',');
    if (*tmp) {
        experience[i].experience= atoi(tmp);
        } else {
        experience[i].experience= INT_MAX;   
    }
  }

    fclose(file);

    file = fopen("experience.csv", "w");
    for (i = 1; i < 601; i++) {
      fprintf(file, "%s,%s,%s\n",
              int_to_string(experience[i].growth_rate_id),
              int_to_string(experience[i].level),
              int_to_string(experience[i].experience));
    }
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("type_names.csv") + 1);

    strcpy(address + address_len, "type_names.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 19; i++) {
    fgets(line, 800, file); 
    fgets(line, 800, file); 
    fgets(line, 800, file); 
    fgets(line, 800, file); 
    fgets(line, 800, file); 
    fgets(line, 800, file); 
    fgets(line, 800, file); 
    fgets(line, 800, file); //English
    for (j = count = 0; count < 2; j++) {
      if (line[j] == ',') {
        count++;
      }
    }
    line[strlen(line) - 1] = '\0';
    types[i] = strdup(line + j);
    fgets(line, 800, file); 
    fgets(line, 800, file); 
    }  

    fclose(file);

    file = fopen("type_names.csv", "w");
    for (i = 1; i < 19; i++) {
      fprintf(file, "%s\n", types[i]);
    }
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("pokemon_stats.csv") + 1);

    strcpy(address + address_len, "pokemon_stats.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 6553; i++) {
    fgets(line, 800, file);
    pokemon_stats[i].pokemon_id = atoi((tmp = next_char(line, ',')));
    tmp = next_char(NULL, ',');
    if (*tmp) {
        pokemon_stats[i].stat_id= atoi(tmp);
        } else {
        pokemon_stats[i].stat_id= INT_MAX;   
        }

    tmp = next_char(NULL, ',');
    if (*tmp) {
        pokemon_stats[i].base_stat= atoi(tmp);
        } else {
        pokemon_stats[i].base_stat= INT_MAX;   
        }

    tmp = next_char(NULL, ',');
    if (*tmp) {
        pokemon_stats[i].effort= atoi(tmp);
        } else {
        pokemon_stats[i].effort= INT_MAX;   
        }
  } 

    fclose(file);

    file = fopen("pokemon_stats.csv", "w");
    for (i = 1; i < 6553; i++) {
      fprintf(file, "%s,%s,%s,%s\n",
              int_to_string(pokemon_stats[i].pokemon_id),
              int_to_string(pokemon_stats[i].stat_id),
              int_to_string(pokemon_stats[i].base_stat),
              int_to_string(pokemon_stats[i].effort));
    }
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("stats.csv") + 1);

    strcpy(address + address_len, "stats.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 9; i++) {
        fgets(line, 800, file);
        stats[i].id = atoi((tmp = next_char(line, ',')));
        tmp = next_char(NULL, ',');
        if (*tmp) {
        stats[i].damage_class_id= atoi(tmp);
        } else {
        stats[i].damage_class_id= INT_MAX;   
        }
        
        strcpy(stats[i].identifier, (tmp = next_char(NULL, ',')));

        tmp = next_char(NULL, ',');
        if (*tmp) {
        stats[i].is_battle_only= atoi(tmp);
        } else {
        stats[i].is_battle_only= INT_MAX;   
        }
        
        tmp = next_char(NULL, ',');
        if (*tmp) {
        stats[i].game_index= atoi(tmp);
        } else {
        stats[i].game_index= INT_MAX;   
        }
    }  

    fclose(file);

    file = fopen("stats.csv", "w");
    for (i = 1; i < 9; i++) {
      fprintf(file, "%s,%s,%s,%s,%s\n",
              int_to_string(stats[i].id),
              int_to_string(stats[i].damage_class_id),
              stats[i].identifier,
              int_to_string(stats[i].is_battle_only),
              int_to_string(stats[i].game_index));
    }
      
    fclose(file);


    address = (char *) realloc(address, address_len + strlen("pokemon_types.csv") + 1);

    strcpy(address + address_len, "pokemon_types.csv");

    file = fopen(address, "r");



    address = (char *) realloc(address, address_len + 1);

    fgets(line, 800, file);
  
    for (i = 1; i < 1676; i++) {
        fgets(line, 800, file);
        pokemon_types[i].pokemon_id = atoi((tmp = next_char(line, ',')));
        tmp = next_char(NULL, ',');
        if (*tmp) {
        pokemon_types[i].type_id= atoi(tmp);
        } else {
        pokemon_types[i].type_id= INT_MAX;   
        }
        
        tmp = next_char(NULL, ',');
        if (*tmp) {
        pokemon_types[i].slot= atoi(tmp);
        } else {
        pokemon_types[i].slot= INT_MAX;   
        }
    }

    fclose(file);

    file = fopen("pokemon_types.csv", "w");
    for (i = 1; i < 1676; i++) {
      fprintf(file, "%s,%s,%s\n",
              int_to_string(pokemon_types[i].pokemon_id),
              int_to_string(pokemon_types[i].type_id),
              int_to_string(pokemon_types[i].slot));
    }
    fclose(file);





    
}
