![](https://s5.gifyu.com/images/SRvtL.gif)

# Simulation tool for Life-like cellular automata, including Game of Life
## A bit on the technical side
This program is written in C++17, utilising OOP and polymorphic principles.  
The GUI was created using [SFML](https://www.sfml-dev.org), a graphics and multimedia library.
It only supports Windows currently.

## Introduction to cellular automata
A [cellular automaton](https://www.techtarget.com/searchenterprisedesktop/definition/cellular-automaton) is "Life-like" if it meets the following criteria:
1. The automaton's array of cells has two dimensions.
2. Each cell of the automaton has two states (conventionally referred to as "alive" and "dead").
3. The neighborhood of each cell is the Moore neighborhood - the eight adjacent cells, and possibly the cell itself.
4. In each time step of the automaton, the new state of a cell can be expressed as a function of the number of adjacent cells
   that are "alive" and of the cell's own state. This function is called the automaton **rule**.

This class is named after **'Game of Life'** (also 'Life'), the most famous cellular automaton, devised by John Horton Conway.

The **rule** of every Life-like cellular automaton can be broken down into:
- Amount of neighbors for a _dead_ cell in order for it to be "_born_".
- Amount of neighbors for a _live_ cell in order for it to "_survive_".

And this is all the information we need to generate the next gen.  
We describe this born/survive logic with a **rulestring**: `B<digits>S<digits>`.  
So, for example, Game of Life is `B3S23`; because a cell is born if it has 3 neighbors;  
and a cell survives if it has 2 or 3 neighbors.

## Available automata
1. [Game of Life (Life)](https://conwaylife.com/wiki/Conway%27s_Game_of_Life) - Initial patterns tend to evolve into structures that interact in complex and interesting ways.
2. Custom rulestring - User-defined life-like automaton.  
   You'll be prompted to enter the 'born' and 'survive' digits from the rulestring.  
   *When prompted, if you wish to input zero digits, simply press Enter.
3. [Coral](https://conwaylife.com/wiki/OCA:Coral) - Exploding rule in which patterns grow slowly and form coral-like textures.
4. [Day and Night](https://conwaylife.com/wiki/OCA:Day_%26_Night) - Symmetric under live-dead reversal. Has patterns with highly complex behavior.
5. [DotLife](https://conwaylife.com/wiki/OCA:DotLife) - Exploding chaotic rule closely related to Life.
6. [Flock](https://conwaylife.com/wiki/OCA:Flock) - Patterns tend to quickly settle into small still lives or oscillators. It differs strongly from Life.
7. [Grounded Life](https://conwaylife.com/wiki/OCA:Grounded_Life) - A more stable version of Life.
8. [H-trees](https://conwaylife.com/wiki/OCA:H-trees) - Exploding chaotic rule where H-shaped branches grow from line patterns.
9. [HighLife](https://conwaylife.com/wiki/OCA:HighLife) - Identical to Life on a lot of simple patterns, but the two differ on more complicated ones.
10. [Iceballs](https://conwaylife.com/wiki/OCA:Iceballs) - Small masses of solid living cells flicker in and out of existence.
11. [Life without Death](https://conwaylife.com/wiki/OCA:Life_without_death) - Similar to Life, but live cells never die.  
    It combines chaotic growth with more structured ladder-like patterns.
12. [Live Free or Die](https://conwaylife.com/wiki/OCA:Live_Free_or_Die) - Exploding chaotic rule in which only cells with no neighbors survive.
13. [Maze](https://conwaylife.com/wiki/OCA:Maze) - Expanding rule that tends to form maze-like designs.
14. [Morley](https://conwaylife.com/wiki/OCA:Move) - Random patterns tend to stabilize extremely quickly.
15. [Pseudo Life](https://conwaylife.com/wiki/OCA:Pseudo_Life) - Chaotic rule with evolution that resembles Life, but few patterns from Life work in this rule.
16. [Replicator](https://conwaylife.com/wiki/OCA:Replicator) - Every pattern self-replicates, and will eventually produce an arbitrary number of copies of itself.
17. [Seeds](https://conwaylife.com/wiki/OCA:Seeds) - Every live cell immediately dies, and many patterns lead to explosive chaotic growth.
18. [Serviettes](https://conwaylife.com/wiki/OCA:Serviettes) - Produces beautiful fabric-like patterns, and similar to Seeds, every live cell immediately dies.

## Instructions for use
Download the release, and run the `game of life.exe`.  
Choose the automaton to simulate, or create a custom one with your own rulestring.  
Then, you can choose one of 100+ pre-defined patterns; or choose "**custom pattern**",  
to input your own by clicking on the GUI (and submit by pressing `Enter`).  
You'll then be prompted to save your creation in .rle format.  
Now, at any time, you can press `Enter` to reset the board and input your own pattern again.  
Also, at any time you can press `Esc` to return to previous menu and change automaton/pattern.

## Functionalities
- The board is infinite, resizable and draggable by mouse clicks and `WASD` keys.
- You can speed up the simulation by pressing `X`, or speed down by pressing `Z`.
- You can zoom-in and zoom-out on the grid by scrolling the mouse wheel.
- A `patterns` directory is included in the release, with all the pre-defined patterns, in [.rle format](https://conwaylife.com/wiki/Run_Length_Encoded),  
  divided to sub-directories by [type](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life#Examples_of_patterns). You can add a pre-defined pattern in 2 ways:
  - Create one with the GUI:  
    In pattern menu, choose "custom pattern", and you'll be prompted to save your creation in .rle format,  
  in `custom` sub-directory.
  - Take an .rle file that contains the pattern, and put it in one of the sub-directories (`custom` is preferable).

  Now, next time you'll run the .exe, it will recognize the new pattern.