# Simon Project 

## Synopsis 

This project is an attempted recreation of the memory game Simon. Player pushes buttons following
a pattern displayed by randomly sequenced lights/sound. To progress a level, the player has to complete a 
sequence without mistakes, and player has to repeat the level if mistake is made with entirely new lights. 
When player succeeds a 9 light sequence, the player wins and the game is finished. 

## Project Components

Parts used in this project: 

- Atmega 1284 
- Atmel Cable/Debugger
- 4 buttons
- 4 LEDS
- LCD Screen
- 1 Potentiometer, 4 resistors 
- Battery
- Atmel Cable/Debugger
- Wires
- Speaker

Programs used in this project: 

- AVR Studio 7.0

## Installation 

- Connect PORTA to inputs, PORTB to outputs, PORTC and PORTD to LCD Screen, speaker to PB6, and connect the debugger and gnd/vcc wires accordingly
- Install AVR Studio 7.0
- Create New Project
- Include "includes" folder in directories
- Add "main.c" into project

## How to Play

- Follow Instructions on LCD Screen
- Wait for sequence of leds to light
- Press buttons according to light/sound sequence - do not hold the buttons!
- Get to six points to win!


## Project Bugs/Review 

Difficulties encountered included button presses, as my buttons were not the most responsive. Code is also not optimized for buttons held down. 
Another difficulty was getting consistent periods, and delay for the "losing" screen. 
I learned how to use my parts, especially the LCD Screen wiring and PORT configurations more effectively, but need to have a cleaner breadboard. 
Ports used are C and D by the LCD Screen, and PORTA for inputs and PORTB for outputs. 

## Links 

Link to [demo](https://www.youtube.com/watch?v=Um30llKExAQ). 
