This assignment is about loading a Pokemon while pc is roaming on the long grass. The probability of encountering pokemon is 10%. The pokemon selected from the data has an equal chance to be shown up as following uniform distribution.
The level of encountered pokemon will increase with the Manhattan distance from the origin. When distance <= 200, the level is from 1 to distance/2. Otherwise, it is from (distance-200)/2 to 100.
Encountered pokemon will know upto 2 moves. If it doesn't have at least one move in its level-up leanest, advance its level top the point where it can learn it. 
Pokemon stats are given in the csv file. And, each pokemon has an additive IV which is uniformly distributed from 1 to 15.
The gender of the pokemon has an equal probability of being male or female.
Pokemon has a 1/8192 chance to be a shiny. We could just ignoring IV and using rand()%8192==0.
When the pokemon is level up, the formula below has been applied.
HP=(((base+IV)*2)*level)/100) + level + 10
Others = (((base+IV)*2)*level)/100) + 5
When encountering a pokemon, there will be a placeholder function for the battle and capture sequence. In here, we’ll print the pokemon’s level, stats, and moves and pause for input (escape, any key, etc.).
When game starts, the pc will have a choice to choose one from three randomly generated pokemons.
All generated trainers should have at least one random pokemon. The trainer has 60% chance to get a next pokemon unto 6. The generated pokemons should follow pokemon generation rule of their map.
In trainer-battle placeholder function, the information of npc's pokemon will be printed.

I have implemented the npc's pokemon generation, however, I've just set them to have only one randomly generated pokemon.
In the battle stage, the information of the npc's pokemon is displayed.