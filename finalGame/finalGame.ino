#include <AsyncDelay.h>
#include <Adafruit_Circuit_Playground.h>

//timers
AsyncDelay lightTimer;
AsyncDelay toneTimer;

//switch
int switchPin = 7;
volatile bool switchFlag = 0;
bool switchState = 0;

//right button
int buttonBPin = 5;
volatile bool buttonBFlag = 0;

//left button
int buttonAPin = 4;
volatile bool buttonAFlag = 0;

//game Variables
int currentLed = 0;
int stackColors[10] = {0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000};
int levelColors[3] = {0x00FF00, 0x0000FF, 0xFF0000};
int level = 0;
int bounceDirection = 1;

bool hasLeftStacked = 0;
bool hasRightStacked = 0;

int leftCurrentStack = 5;
int rightCurrentStack = 4;

bool gameOver = 0;
bool victory = 0; 
bool secondTone = 0;

//for notes
float midi[127];
int A_four = 440; // a is 440 hz...



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CircuitPlayground.begin();
  generateMIDI();

  //switch
  pinMode(switchPin, INPUT_PULLUP);
  switchState = digitalRead(switchPin);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchChange, CHANGE);

  //button
  pinMode(buttonBPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(buttonBPin), buttonBClick, FALLING);

  pinMode(buttonAPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(buttonAPin), buttonAClick, FALLING);

  //timers setup
  lightTimer.start(1000, AsyncDelay::MILLIS);
  toneTimer.start(1000, AsyncDelay::MILLIS);

  //set first light
  CircuitPlayground.setPixelColor(currentLed, 0xFF00FF);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!gameOver) {
    //check switch
    if (switchFlag) {
      delay(5); //debounce
      switchState = digitalRead(switchPin);

      switchFlag = 0;
    }

    //when right button pressed
    if (buttonAFlag) {
      delay(5); //debounce
      if (!hasLeftStacked) {  //make sure you can only add one light to a side per cycle
        if (currentLed == leftCurrentStack) { //check if current lit up led is the correct one
          hasLeftStacked = 1;
          leftCurrentStack += 1;
          stackColors[currentLed] = levelColors[level];
          
          if (switchState) { // play sound if switch is on
            noTone(CPLAY_BUZZER);
            tone(CPLAY_BUZZER, midi[60]);
            toneTimer.start(100, AsyncDelay::MILLIS);
            secondTone = 1;
          }
        } else {
          gameOver = 1;  //if it is the wrong light end the game
          if (switchState) {
            CircuitPlayground.playTone(midi[50], 300);
            CircuitPlayground.playTone(midi[48], 300);
            CircuitPlayground.playTone(midi[46], 300);
            CircuitPlayground.playTone(midi[42], 500);
          }
        }
      }

      buttonAFlag = 0;
    } 


    //when left button pressed
    if (buttonBFlag) {
      delay(5); //debounce
      if (!hasRightStacked) {  //make sure you can only add one light to a side per cycle
        if (currentLed == rightCurrentStack) { //check if current lit up led is the correct one
          hasRightStacked = 1;
          rightCurrentStack += -1;
          stackColors[currentLed] = levelColors[level];

          if (switchState) { // play sound if switch is on
            noTone(CPLAY_BUZZER);
            tone(CPLAY_BUZZER, midi[60]);
            toneTimer.start(100, AsyncDelay::MILLIS);
            secondTone = 1;
          }
        } else {
          gameOver = 1; //if it is the wrong light end the game
          if (switchState) {
            CircuitPlayground.playTone(midi[50], 300);
            CircuitPlayground.playTone(midi[48], 300);
            CircuitPlayground.playTone(midi[46], 300);
            CircuitPlayground.playTone(midi[42], 500);
          }
        }
      }
      

      buttonBFlag = 0;
    }


    //light pattern
    if (lightTimer.isExpired()) {   
      CircuitPlayground.setPixelColor(currentLed, stackColors[currentLed]);  //turn the current pixel back to its orignial color
      if ((currentLed == 9 && bounceDirection == 1) || (currentLed == 0 && bounceDirection == -1)) {  //change the direction the light moves if at one of the edges
        bounceDirection *= -1;
      } 

      currentLed += bounceDirection;  //increment to next led
      CircuitPlayground.setPixelColor(currentLed, 0xFF00FF);  //set new current led to purple

      //reset has stacked for each side
      if (currentLed == 4 && bounceDirection == 1) {
        hasLeftStacked = 0;
      } else if (currentLed == 5 && bounceDirection == -1) {
        hasRightStacked = 0;
      } 

      //if the stack is full increase the level by 1 and reset the stacks
      if (rightCurrentStack == -1 && leftCurrentStack == 10) {
        level += 1;
        rightCurrentStack = 4;
        leftCurrentStack = 5;      
      }

      lightTimer.start(240 - level * 20, AsyncDelay::MILLIS); //start delay for next light pattern cycle, time gets faster as level increases
    }

    //stop tones from playing if they have lasted their duration
    if (toneTimer.isExpired()) {
      noTone(CPLAY_BUZZER);

      if (secondTone) {
        tone(CPLAY_BUZZER, midi[70]);
        toneTimer.start(200, AsyncDelay::MILLIS);  
        secondTone = 0;      
      }
    }

    //check if won
    if (level == 3) {
      victory = 1;
      gameOver = 1;
    }
    

  } else if (victory) { //play victory melody
    if (switchState) {
      CircuitPlayground.playTone(midi[60], 500);
      CircuitPlayground.playTone(midi[64], 500);
      CircuitPlayground.playTone(midi[67], 500);
      CircuitPlayground.playTone(midi[72], 500);
    }
  }
}

void switchChange () {
  switchFlag = 1;
}

void buttonBClick () {
  buttonBFlag = 1;
}

void buttonAClick () {
  buttonAFlag = 1;
}

void generateMIDI()
{
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}