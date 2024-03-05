# Tic Tac Toe Plus
The well-known Tic Tac Toe game with many additional game mechanics written in C for Windows. This project was created for a university assignment.

## Main Idea

In a game as simple as Tic Tac Toe, if you are playing conventionally, there is really not much you can do to add a little bit more fun to gameplay. Originally, we were given a task to create a replica of this game within a week; however, one thing we realized while developing it was that we finished it in just 4 hours. So we decided to add random features to it and call it Tic Tac Toe _Plus_. The first time anyone tries to play, they immediately notice a couple of key differences, such as the way grids and prompts are printed, user input validation, and great flexibility over almost all game mechanics.

## Launch

The game starts a little window greeting the player(s). The text slowly fills in and later it listens for a key press. One `Enter` is pressed, it takes the player to the main menu. There is a big sign at the top of the screen displaying the title of the game with a short animation using a "sliding window" effect. Note that we use [box-drawing characters](https://en.wikipedia.org/wiki/Box-drawing_character) extensively.

![Launch](/res/launch.gif)
