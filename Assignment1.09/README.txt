In this project, I've implmented the Pokemon battle. The battle would be occured
when PC encounter the wild Pokemon or the other trainer. 
Battle with wild Pokemon end either the wild Pokemon or all of the trainer's
Pokemon getting knocked out, or the trainer flees. 
Trainer battles end when all of one trainer's Pokemon are knocked out.
Battle between trainers cannot be flee and the Pokemon cannot be captured.
The knocked out Pokemon is unavailable until it is revived.
Pokemon can be revived by using revives or by healing from the Poke center.
Each party takes turns in a Pokemon battle. If the action chosen by PC
is anything other than Pokemon moves, it recieves a maximum priority. 
If both moves in a turn are Pokemon moves, then the turn can be set by following the rules:
1st: Comparing moves.priority.
If those are equal, then
2nd: highest Speed(level adjusted) goes first
If those are equal, then
3rd: choose one at random.

If Pokemon is knocked out or captured before it moves, its turn is forfeit.

The accuracy of the move can be decided if ranc()%100 < moves.accuracy, the move hits.

And, the damage can be calculated by using the given formula.

Durng the battle, the PC has the following options:

* Fight
        AI choose a move radomly. rand()%(the number of pokemon's moves).

* Bag(Inventory): revives(revives it and heal half HP), potion(heal 20 HP, never exceding Max HP), and Pokeballs
    Pokeballs only can be used in battle with wild Pokemon.
    If PC has 6 Pokemon, the attempt to capture the Pokemon would be failed and
    the Pokemon would flee, otherwise, every attempt will be suceed.
    Every items are restored everytimes when PC visit Pokemart.
    If user press B, the items in bag are displayed.

* Run/flee
        Only available in battle with wild Pokemon.

* Pokemon: switching a different active Pokemon. May have upto 6 Pokemon.
        The first Pokemon will start the battle always.
        After the battle, Pokemons are not revived or healed automatically.
        PC has to visit Pokecenter or use items to heal or revives them.


I have implmented the battle with the wild pokemon, but it seems not working properly.
Also, I couldn't fix the problem with my dmg_cal() so I just return 5 dmg. 