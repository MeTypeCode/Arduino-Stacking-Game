#include <AsyncDelay.h>
#include <Adafruit_Circuit_Playground.h>
// #include <string>
// using namespace std;

AsyncDelay lightTimer;
AsyncDelay toneTimer;

int switchPin = 7;
volatile bool switchFlag = 0;
bool switchState = 0;

int buttonBPin = 5;
volatile bool buttonBFlag = 0;

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

int gameOver = 0;

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

  //set first light
  CircuitPlayground.setPixelColor(currentLed, 0xFF00FF);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!gameOver) {
    if (switchFlag) {
      delay(5);
      switchState = digitalRead(switchPin);

      switchFlag = 0;
    }

    if (buttonAFlag) {
      delay(5); //might need to make higher, didn't work earlier
      if (!hasLeftStacked) {
        if (currentLed == leftCurrentStack) {
          hasLeftStacked = 1;
          leftCurrentStack += 1;
          stackColors[currentLed] = levelColors[level];
        } else {
          gameOver = 1;
        }
      }

      buttonAFlag = 0;
    } 

    if (buttonBFlag) {
      delay(5); //might need to make higher, didn't work earlier
      if (!hasRightStacked) {
        if (currentLed == rightCurrentStack) {
          hasRightStacked = 1;
          rightCurrentStack += -1;
          stackColors[currentLed] = levelColors[level];
        } else {
          gameOver = 1;
        }
      }
      

      buttonBFlag = 0;
    }

    if (lightTimer.isExpired()) {
      CircuitPlayground.setPixelColor(currentLed, stackColors[currentLed]);
      if ((currentLed == 9 && bounceDirection == 1) || (currentLed == 0 && bounceDirection == -1)) {
        bounceDirection *= -1;
      } 

      currentLed += bounceDirection;
      CircuitPlayground.setPixelColor(currentLed, 0xFF00FF);

      //reset has stacked for each side
      if (currentLed == 4 && bounceDirection == 1) {
        hasLeftStacked = 0;
      } else if (currentLed == 5 && bounceDirection == -1) {
        hasRightStacked = 0;
      } 

      if (rightCurrentStack == -1 && leftCurrentStack == 10) {
        level += 1;
        rightCurrentStack = 4;
        leftCurrentStack = 5;      
      }

      lightTimer.start(240 - level * 20, AsyncDelay::MILLIS);
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