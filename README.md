# kyrolab
Game of Life Simulator

This program features multithreaded process to simulate Game of Life in 2D grid space.
Each thread 'gameofLife' represents a parallel space where a certain number of seed agents (representing ficticious life forms or particles) enter the 2D grid world and interact with other seed agents from other parallel spaces.
Interaction is defined by specific set rules.
The Grid template class and mutithreading Record class can be reused to design simple text (later on graphic based) based board game (grid board) such as go.

To Run the program, follow the following step:
1. From /src directory, run:
$ make clean
$ make
2. ./kyrolab yourfilename.txt
