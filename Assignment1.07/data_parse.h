#ifndef DATA_PARSE_H
# define DATA_PARSE_H

class pokemon_data {
    public:
    int id;
     char identifier[30];
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
};

class move_data {
    public:
    int id;
     char identifier[50];
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
     int damage_class_id;
     int effect_id;
     int effect_chance;
     int contest_type_id;
     int contest_effect_id;
     int super_contest_effect_id;
};

class pokemon_move_data {
    public:
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
};

class pokemon_species_data {
    public:
    int id;
    char identifier[30];
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;
};

class experience_data {
    public:
    int growth_rate_id;
    int level;
    int experience;
};

class pokemon_stats_data {
    public:
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
};

class stats_data {
    public:
    int id;
    int damage_class_id;
    char identifier[30];
    int is_battle_only;
    int game_index;
};

class pokemon_types_data {
    public:
    int pokemon_id;
    int type_id;
    int slot;
};

extern pokemon_move_data pokemon_moves[528239];
extern pokemon_data pokemon[1093];
extern char* types[19];
extern move_data moves[845];
extern pokemon_species_data species[899];
extern experience_data experience[601];
extern pokemon_stats_data pokemon_stats[6553];
extern stats_data stats[9];
extern pokemon_types_data pokemon_types[1676];

void data_parse();

#endif
