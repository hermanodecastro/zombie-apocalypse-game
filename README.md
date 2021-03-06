# Zombie Apocalypse

This game was inspired by The Walking Dead

![image](https://user-images.githubusercontent.com/45005417/119849759-46d2b600-bee3-11eb-9de3-d0e6e125d88d.png)

## Quickstart
1. Clone this repository to your local.
> `https://github.com/hermanodecastro/zombie-apocalypse-game.git`

2. On Linux `gcc main.c -o main -lm` to compile the application.

3. On Windows `gcc main.c -o main` to compile the application.


## The game

1 - Rick wakes up stunned in some random place on the scene, with a gun without bullets. (Color White)

2 - There are 15 zombies randomly scattered around the scenery. (Color Red)

3 - Obstacles are everywhere. Altogether there are 4 cars, 7 trees and 8 stones randomly distributed. (Color Yellow)

4 - In the scenario there is a single exit, also randomly defined. If she is blocked by obstacles, Rick has no
exit and will die. Otherwise, if Rick reaches the exit, the game is over.

5 - Rick has 4 possible movements: up, down, left and right. Although Rick's life is complicated, there are 4
bullets scattered on the scenerio. If Rick moves and has a bullet in that position, Rick will load the weapon immediately.

6 - If Rick tries to move to an area where there is an obstacle, he remains where he is and does not move. If he
move to a region where there is a zombie, there are two possibilities: if Rick has his gun unloaded, he’s
attacked and dies; otherwise, Rick uses the bullet on the zombie.

7 - Zombies have 4 possible movements: up, down, left and right. The zombies that are 3 square meters away
Rick will chase him. Those who are farthest away will stand still.

## Directional keys

- UP:     w
- DOWN:   s
- LEFT:   a
- RIGHT:  d 

Press enter after pressing a directional key.
