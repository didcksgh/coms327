#ifndef INPUT_H
# define INPUT_H

typedef struct character character_t;
typedef int16_t pair_t[2];

void input_init_terminal(void);
void input_reset_terminal(void);
void input_display(void);
void input_user_input(pair_t dest);
void input_queue_message(const char *format, ...);
void input_battle(character_t *aggressor, character_t *defender);

#endif
