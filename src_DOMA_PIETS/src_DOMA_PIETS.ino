#include <TM1638.h>

char CODE[] = "3AE61Cb1";
int TIMES = 10;
int DISP = 100;

// Ustawienia pinów
const int strobe = 7;
const int clock = 9;
const int data = 8;

const int DISPLAY_SIZE = 8;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'};
// const char allowedChars[] = "0123456789AbCdEF";

TM1638 module(data, clock, strobe);

//Cztery możliwe stany programu
typedef enum {IN_PROGRESS, WAITING, FINISHED, RESET} states;

//Definicje funkcji
void handleClick(states *);
void readInput(states *);
boolean initCode();
boolean initTimes();
boolean initDisp();
boolean isAllowed(char);


void setup() {
  Serial.begin(9600);
  module.clearDisplay();
}

void loop(){
  char *display = (char *) malloc(DISPLAY_SIZE + 1);
  int leds = 0;
  module.setLEDs(leds);
  states state = IN_PROGRESS;

  int charToDisplay = 0;

  for(int i = 0; i < DISPLAY_SIZE; i++) {
    display[i] = allowedChars[charToDisplay];
  }

  module.setDisplayToString(display);

  for(int i = 0; i < DISPLAY_SIZE; ++i){
    for(int j = 0; j < TIMES; ++j){
      charToDisplay+=1;
      if(charToDisplay>15) { // zamiast wielkiego switcha
         charToDisplay = 0;
      }

      for(int k = i; k < DISPLAY_SIZE; ++k){
        display[k] = allowedChars[charToDisplay];
      }

      module.setDisplayToString(display);

      handleClick(&state);
      readInput(&state);

      if(state == RESET){
        free(display);
        return;
      }
      delay(DISP);
    }
    leds = (leds << 1) +1; // chyba szybsze
    module.setLEDs(leds);
    display[i] = CODE[i];
    module.setDisplayToString(display);
  }
  free(display);

  state = FINISHED;
  handleClick(&state);
}

void handleClick(states *state){
  if (*state == IN_PROGRESS) {
    int keys = module.getButtons();
    if (keys == 1) {
      *state = WAITING;
    }
  }

  switch(*state){
    case IN_PROGRESS:
      break;
    case WAITING:
      Serial.println("in");
      for(int key = 0; key != 1; key = module.getButtons()){
        delay(200);
      }
      Serial.println("out");
      *state = IN_PROGRESS;
      break;
    case FINISHED:
      for(int key = 0; key != 2; key = module.getButtons()){
        readInput(state);
        delay(100);
        if(*state == RESET){
          break;
        }
      }
      break;
    default:
      Serial.println("Strange program state");
      *state = IN_PROGRESS;
      break;
  }
}

void readInput(states *state){
  while(Serial.available() > 0){
    char rc = Serial.read();
    switch(rc){
      case 'C':
        Serial.println("C command");
        if(Serial.available() > 0) {
          boolean newState = initCode();
          if(newState){
            *state = RESET;
          }
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      case 'N':
        Serial.println("N command");
        if(Serial.available() > 0) {
          initTimes();
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      case 'D':
        Serial.println("D command");
        if(Serial.available() > 0) {
          initDisp();
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      default:
        Serial.print("Wrong command: ");Serial.println(rc);
        break;
    }
  }
}

boolean initCode(){
  char newCode[DISPLAY_SIZE+1] = "\0";
  for(int i = 0; i < 8 && Serial.available() > 0; ++i){
    char rc = Serial.read();
    if(isAllowed(rc)){
      newCode[i] = rc;
    } else {
      Serial.print("Occured wrong value: "); Serial.println(rc);
      return false;
    }
  }
  memcpy(CODE, newCode, 8);
  Serial.println(CODE);
  return true;
}

boolean initTimes(){
  char newTimes[4] = "\0";
  for(int i = 0; i < 3 && Serial.available() > 0; ++i){
    char rc = Serial.read();
    if(rc >= '0' && rc <= '9'){
      newTimes[i] = rc;
    } else {
      Serial.print("Occured wrong value: "); Serial.println(rc);
      return false;
    }
  }
  sscanf(newTimes, "%d", &TIMES);
  Serial.println(TIMES);
  return true;
}

boolean initDisp(){
  char newTimes[5] = "\0";
  for(int i = 0; i < 5 && Serial.available() > 0; ++i){
    char rc = Serial.read();
    if(rc >= '0' && rc <= '9'){
      newTimes[i] = rc;
    } else {
      Serial.print("Occured wrong value: "); Serial.println(rc);
      return false;
    }
  }
  sscanf(newTimes, "%d", &DISP);
  Serial.println(DISP);
  return true;
}

boolean isAllowed(char value){
  for(int i = 0 ; i < 16; ++i){
    if(allowedChars[i] == value){
      return true;
    }
  }
  return false;
}
