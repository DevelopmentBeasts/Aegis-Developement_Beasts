------"AEGIS"------

Game creators: Joan Marín and Jaume Avinyó

## AEGIS

Aegis is a platformer game done by two students for a UPC-TTC (Universitat Politècnica de Catalunya - TECH TALENT CENTER) project in Barcelona, Catalunya.

The game is about the path of the fire wisp, witch has to collect the green gems from the magic caves and will face several dangerous monsters. In this adventure, his most valuable skill will be the velocity, witch will make him survive as he is defenseless.

This game it's been done  using "Tile" program and it has two levels. It is fully programmed in C++ using SDL library;


<iframe width="1280" height="720" src="https://www.youtube.com/watch?v=ktgW0TUtNUI" frameborder="0" gesture="media" allow="encrypted-media" allowfullscreen></iframe>


#### Github Repository: [Aegis](https://github.com/DevelopmentBeasts/Aegis)

## Developers

![]()

Jaume Avinyó
IA for all entities, 1/4 of the pathfinding, FPS control, debug functionalities, main character logic, near full game art logic, youtube video and webpage, camera and render logic 1/2.
* [Github](https://github.com/Jaumeavinyo) @jas3

Joan Marín
GUI, 3/4 of the Pathfinding, render logic 1/2, Tile info reading.
* [Github](https://github.com/X0KA) @X0KA


## Game Main Core
The structure has been made using a module called application witch calls the main functions of all the application modules to make them work such as Start(), Update() - pre and pos, Clear() ... App is the module that controls when do other modules work.
All data such ass animation pushbacks or internal data of the application was set up using XML files to avoid harcoded values for variables.


All regarding to game metadata (paths of assets, character and enemies characteristics...) are loaded with XML files to avoid hardcoding,
avoid confusing code and build an easier work flow.


UML HEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEpng and explanation
![]()

### UI System

ui sistem here!!!!!! png and explanation

![]()

### Innovations
*flipping of textures.
*Enemies that detect obstacles and avoids them jumping over them
*Render system witch avoids calling tiles that are not inside camera even if it is was not going to  print them, this was made to avoid checking useless info and save time.
*System witch makes the player rotate making a perfect parabola when jumping.
*load all colliders and entities from the XML generated by tiled.
*system to avoid tunelling witch has been made using sensors that detect how much space you have to make a dash

## Gameplay & Instructions

Your objective is to Pass throw levels taking the biggest amount of green gems with you:

Move and jump using WASD.

Make a dash to the right or to the left using arrows.

Your objective is to avoid enemies, they are very fast! but you are more and you have your dash! use it with brain, you could end up falling to the void!

Menu works in the start of the aplication and you can acces it throught the esc button while playing.

Debug purposes:

F1
F2
F3
F4
F5
F6
F7
F8
F9
F10


