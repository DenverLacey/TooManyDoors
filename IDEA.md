# Game Idea

The idea for the game is that you have been abducted by aliens and are trying
to escape from their ship. As you're navigating the ship you come across many
locked doors that block your path to freedom. You hack the doors by learning
how to use their weird alien technology and open them one-by-one until you can
escape the spaceship.

-------------------------------------------------------------------------------

# Hacking the Doors (The gameplay)

The gameplay of the game is about hacking these alien doors open. The idea of
the puzzle is that you write a short sentence of strange alien runic symbols
to command a tiny robot inside the circuitry of the door to alter the circuitry
so that when the robot lands on the power-on square, the electricity flows into
the door.

The sentences will be like vim commands where a rune represents a specific
command such as: move, change direction, rotate, interact, etc.  
They would also be number runes that could alter commands, for example:
if the move command used a rune that looked like 'M', then the sentence
'4M' would mean move four spaces which is shorter than just writing 'MMMM'.

A repeat command that can be used to repeat the last *n* commands. It would
only do this once to prevent having to add branching to prevent infinite loops.

The circuit board will have different types of a squares the robot and/or power
in different ways such as: a reflector that rotates the robot and power 90°, a
redirector that changes the direction of the robot and power, a portal,
barrier, a splitter that either clones the robot or splits the power in two,
etc.

-------------------------------------------------------------------------------

# Communication

I want to design the levels and their sequence such that a tutorial isn't
necessary. That instead, the player is slowly introduced to new runes and
circuitry and expected to experiment to figure out what things do similar to
The Witness.
