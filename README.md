# Version 2

This branch contains the version 2 of the platformer. This version was made to experiment with the integration of a small circuit based on an Arduino-compatible board with the game.
The circuit uses the following components:

- an Arduino-compatible ESP8266 LOLIN(WeMos) D1 R1 board
- a HD44780 LCD display
- a RC522 RFID module used with a MIFARE Classic 1K card
- 2 resistors (100 ohms)
- 2 LEDs

It allows the user to scan their RFID badge, and get visual feedbacks with the LCD and LEDs.
Here is what the circuit looks like:

![setup](https://user-images.githubusercontent.com/75886520/148432857-61476ce4-ea1b-428d-9c76-13d5b7e734d0.png)
![setup](https://user-images.githubusercontent.com/75886520/148432872-ebdfff0e-62be-48b9-a9e2-ae9fd36e0fa2.jpg)

## The new level

Version 2 adds a new level to the game to work with the circuit.

This third level really has nothing to do with the two previous ones... <br>
In this new level, the player gets to be a hacker whose goal is to infiltrate a fictional company. At some point, that hacker ends up in a room full of control terminals. All those terminals have to be activated with the right employee badge to unlock access to the next room!

Luckily, our hacker somehow managed to get their hands on an employee badge. The trick is that each terminal can only be activated by one specific employee of the company (employee A for terminal 1, employee B for terminal 2, etc...). Hence, activating all the terminals in the room would in theory require more than just one employee badge.

It is worth mentionning that the badge found by our definitely lucky hacker, allows them to activate one terminal in the room. From there, being a hacker, our hero is able to work their way through the terminal, and hack it to retrieve useful intel about employees and their access rights to the terminals. The exploit also allows the hacker to reconfigure the employee badge they found, and write new employee IDs into it. Well, it seems our hacker now has everything to overcome this little challenge!

One thing though: things are never so easy! The data the hacker gets from each terminal is always corrupted and incomplete. Let's see if there still is a way to get everything they need by cross-referencing each terminal's dumped data...

## Technical details

The user still plays as a little blue square in the game. The level has 4 terminal for the player to activate. The player can try to activate a terminal by:
- Getting their avatar (the blue cube) in front of a terminal.
- Scanning their badge on the reader.

If the employee ID on the badge matches the one required to activate the terminal, that terminal activates. Otherwise, the player gets his access to the terminal denied (temporarily).
