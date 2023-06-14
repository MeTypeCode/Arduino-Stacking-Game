# Arduino Stacking Game

## Author
Kylan Jagels

## Requirements

Built to run in arduino ide on ardiuno circuit playground express board.

## Description

This game is a mix between the classic aracade stacking game and the spinning light game. To play you will stack the lights on both the left and the right side of the arduino. Pressing the button when the purple LED is at the top of the opposite sides stack will add to the stack (this means you press the left button when the light is on the right side, and the right button for when the light is on the left side). You can only stack once on each side per cycle. The goal is to fill the stacks, at which point you will move onto a harder level where the purple LED will move faster and you will be stacking different color lights. After completing 3 levels you beat the game.

## Input/Output
left button: used to stack on right side
Right button: used to stack on left side
LED lights: show where the stacking block is (purple) and how high has been stacked
Sound: indicates when successful stack and plays tune when lost or win
