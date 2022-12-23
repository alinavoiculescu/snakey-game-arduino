# :snake: Snakey Game :snake:

## Introduction
Snakey is an interactive game designed to provide an engaging and stimulating experience for the player. It uses a matrix 8x8, an LCD 16x2, a joystick, and a buzzer to present a variety of gameplay elements and sound effects. The game is inspired by the classic Snake game and includes several unique features, such as adjustable difficulty levels and customizable sound settings. It employs the LedControl, LiquidCrystal, and EEPROM libraries to facilitate the display of visual and auditory elements, as well as the storage of player data.

## Hardware Components
Snakey uses the following components:
* **Matrix 8x8**: used to display the game and several images when scrolling through the main menu
* **Joystick**: employed to allow the player to navigate the menu and play the game
* **Buzzer**: utilized to produce sound effects, such as when the player successfully eats the blinking food or when the game is over
* **LCD 16x2**: used to display the main menu and other relevant information during gameplay, such as the player's score and level
* **Potentiometer**: utilized to control the contrast of the LCD
* **Electrolytic capacitor**: used for minimize power-supply ripple due to peak digit driver currents
* **Ceramic capacitor**: used for minimize power-supply ripple due to peak digit driver currents
* **Wires** (per logic)
* **Resistors** (per logic)

## Library Dependencies
Snakey relies on the following libraries to function properly:
* **LedControl**: responsible for controlling the matrix display
* **LiquidCrystal**: utilized for controlling the LCD screen
* **EEPROM**: utilized for storing high scores and other settings data, allowing the player to retain their progress and settings even when the game is turned off

## Constants and Variables
Snakey utilizes several constants and variables to store data such as menu items, matrix images, and pin numbers. Some notable examples include:
* **matrixSize**: the number of rows and columns in the matrix display
* **matrixImages**: an array of arrays storing images to be displayed on the matrix display
* **menuItems**: an array of strings storing the names of main menu items
* **settingsMenuItems**: an array of strings storing the names of items in the settings menu
* **difficultyIdxEEPROM**: the EEPROM index where the difficulty level is stored
* **soundIdxEEPROM**: the EEPROM index where the sound setting is stored
* **noHighscores**: the number of high scores that can be stored

## Menu
Snakey features a main menu, displayed on the LCD, which includes the following options:
* **Start Game**: initiates gameplay
* **Highscores**: displays stored high scores
* **Settings**: opens the settings menu, allowing the player to reset high scores and to adjust difficulty and sound settings
* **About**: displays information about the game
* **How to Play**: displays instructions for gameplay

## Gameplay
Snakey Game is a reflex-based challenge in which the player controls a snake, using the joystick to navigate through an arena and collect food. The snake grows in length with each food collected and must avoid hitting walls or biting its tail, as these actions will result in the game being over. The game has three levels, each with its own unique set of walls that the player must navigate around.

The player's score increases based on the difficulty level selected in the settings menu. If the difficulty is set to easy, the score increases by 1 when the snake eats the blinking food. If the difficulty is set to medium, the score increases by 2. If the difficulty is set to hard, the score increases by 3.

To initiate gameplay, the player must select "Start Game" from the main menu and use the joystick to navigate the snake towards the food. If the snake successfully reaches the food, the player's score will increase and the snake will grow in length. If the snake hits a wall or bites its tail, the game will be over.

If the player achieves a high score, they will be prompted to enter their name for inclusion in the high scores table. If the player wins the game, a congratulations message will be displayed on the LCD screen.

The player can exit the game at any time by pressing the joystick button for two seconds. This will make the user lose the game (the "Game over!" message will be displayed) and then will return the player to the main menu.

## Settings
Snakey features a settings menu, which can be accessed by selecting "Settings" from the main menu. The settings menu includes the following options:
* **Difficulty**: allows the player to adjust the difficulty level of the game. The difficulty level determines how quickly the snake moves and how much the player's score increases with each successful food collected. The available difficulty levels are easy, medium, and hard.
* **Sound**: allows the player to turn sound effects on or off.
* **Reset highscores**: allows the player to reset the high scores table, deleting all stored scores and names.

## About
The "About" menu option displays information about the game, including the version number and credits.

## How to Play
The "How to Play" menu option provides instructions for gameplay, including controls and objectives.

## Pictures
<img src ="https://i.imgur.com/K3kArA5.jpg" alt="Picture 1 - Snakey Game" style="width: 360px; height: 480px;"/>
<img src ="https://i.imgur.com/SaQmMOx.jpg" alt="Picture 2 - Snakey Game" style="width: 360px; height: 480px;"/>
<img src ="https://i.imgur.com/15eixbh.jpg" alt="Picture 3 - Snakey Game" style="width: 360px; height: 480px;"/>
<img src ="https://i.imgur.com/gJUvKP5.jpg" alt="Picture 4 - Snakey Game" style="width: 360px; height: 480px;"/>

## Video
https://www.youtube.com/watch?v=A14ZXG4NRKk&ab_channel=AlinaVoiculescu

<a href="http://www.youtube.com/watch?feature=player_embedded&v=A14ZXG4NRKk" target="_blank"><img src="http://img.youtube.com/vi/A14ZXG4NRKk/0.jpg" alt="Video - Snakey Game" width="480" height="360" border="10"></a>

## Additional Notes
* In order to ensure optimal gameplay, it is recommended that the joystick be held in a non-neutral position continuously. This will allow for more precise control of the snake, ensuring that it moves in the desired direction. If the joystick is held in a neutral position and used to try and change the snake's direction, it may not always respond as expected.

* Additionally, when navigating through the settings menu, it is important to be mindful of the speed at which the joystick is moved. Sometimes, when attempting to exit a submenu and return to the main settings menu, the game may instead navigate to the main menu due to the joystick being held too much. A shorter move is expected for functioning properly. To avoid this issue, it is recommended to move the joystick in a controlled and deliberate manner when navigating through the menu.

## Addendum
As an additional note, it has come to our attention that the original design of the arrow icons used to indicate scrolling options in the menus may have caused confusion for some players. Specifically, the use of double-sided arrows (**`<-------------->`**) and vertically oriented arrows (**`^`** and **`v`**) may have made it difficult to discern whether the player could scroll left, right, up, or down.

To address this issue, the arrow icons have been modified to include single-sided arrows. The updated icons are as follows:

* Left-facing arrow: **`<--------------`**
* Double-sided arrow: **`<------------->`**
* Right-facing arrow: **`-------------->`**
* Upward-facing arrow: **`^`**
* Downward-facing arrow: **`v`**

We hope that these changes will improve the clarity and usability of the game's menus for all players.

Please see the following video demonstration of the updated feature: https://www.youtube.com/watch?v=MK7rPxRiK_U&ab_channel=AlinaVoiculescu

<a href="http://www.youtube.com/watch?feature=player_embedded&v=MK7rPxRiK_U" target="_blank"><img src="http://img.youtube.com/vi/MK7rPxRiK_U/0.jpg" alt="Video - Snakey Game update" width="480" height="360" border="10"></a>

## Conclusion
Snakey Game provides a fun and challenging experience for players of all skill levels. Its adjustable difficulty and customizable sound settings allow for a personalized gameplay experience, and its high score feature adds an element of competition. With its matrix display, joystick, and buzzer, the game offers a visually and audibly stimulating experience for the player.