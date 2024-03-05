# Tic Tac Toe Plus +
The well-known Tic Tac Toe game with many additional game mechanics written in C for Windows. This project was created for a university assignment.

## Main Idea

In a game as simple as Tic Tac Toe, if you are playing conventionally, there is really not much you can do to add a little bit more fun to gameplay. Originally, we were given a task to create a replica of this game within a week; however, one thing we realized while developing it was that we finished it in just 4 hours. So we decided to add random features to it and call it Tic Tac Toe *Plus*. The first time anyone tries to play, they immediately notice a couple of key differences, such as the way grids and prompts are printed, user input validation, and the great flexibility in almost all game mechanics.

## Launch

The game starts with a little window greeting the player(s). The text slowly fills in and later it asks for a key press. Pressing `Enter` takes the player to the main menu. There is a big sign at the top of the screen displaying the title of the game with a short animation using a "sliding window" effect. Note that we use [box-drawing characters](https://en.wikipedia.org/wiki/Box-drawing_character) extensively.

![Launch](/res/launch.gif)

## Menus & Prompts & Input Validation

All menus generally list all the options and actions you can perform with their brief descriptions. One important nuance here is that the player needs to enter options as plain texts, while some might expect it to listen for key presses. For consistency reasons, we have grouped texts into four categories in terms of their purpose:
 * `#` - Preceeds a comment or a block of info.
 * `>` - Preceeds a prompt and then user input.
 * `!` - Preceeds an error message.
 * `~` - Preceeds a warning message.
This may seem a little confusing *at first*, but overall, this way we can provide more readability.

The program will try to validate every possible user input and print an error message in case of a failure; users can re-enter their input after an error. Inputs are parsed in a case-insensitive manner, i.e. it does not matter whether `a` or `A` is entered. Below you can see a list of all options in the main menu (`#`), a prompt for the user (`>`), and a series of error messages for different reasons (`!`):

![Menu](/res/categories-of-texts.png)
