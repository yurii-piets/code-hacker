#include <TM1638.h>

char CODE[] = "3AE61Cb1";
int DISP = 100;
int TIMES = 10;

// Ustawienia pinów
const int strobe = 7;
const int clock = 9;
const int data = 8;

const int DISPLAY_SIZE = 8;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'};

TM1638 module(data, clock, strobe);

//Cztery możliwe stany programu
const int IN_PROGRESS = 0;
const int WAITING = 1;
const int FINISHED = 2; 
const int RESET = 3;

//Definicje funkcji
void handleClick(int *);
int pow2(int);
boolean isAllowed(char);
void readInput(int *);
boolean initCode();
boolean initDisp();
boolean initTimes();

void setup() {
  Serial.begin(9600);
  module.clearDisplay();
}

void loop(){
  char *display = (char *) malloc(DISPLAY_SIZE + 1);
  int leds = 0;
  module.clearDisplay();
  module.setLEDs(leds);
  int state = IN_PROGRESS;
 
  char displayLetter = '0';
  
  for(int i = 0; i < DISPLAY_SIZE; i++) {
    display[i] = displayLetter;
  }

  module.setDisplayToString(display);

  for(int i = 0; i < DISPLAY_SIZE; ++i){
    for(int j = 0; j < TIMES; ++j){
      switch(displayLetter){ //next letter to show
        case '9':
          displayLetter = 'A';
          break;
        case 'A':
          displayLetter = 'b';
          break;
        case 'b':
          displayLetter = 'C';
          break;
        case 'C':
          displayLetter = 'd';
          break;
        case 'd':
          displayLetter = 'E';
          break;
        case 'F':
          displayLetter = '0';
          break;
        default:
          displayLetter+=1;
          break;
      }
      
      for(int k = i; k < DISPLAY_SIZE; ++k){
        display[k] = displayLetter;
      }
      
      module.setDisplayToString(display);

      state = IN_PROGRESS;
      handleClick(&state);
      
      state = IN_PROGRESS;
      readInput(&state);

      if(state == RESET){
        free(display);
        return;
      }
      
      delay(DISP);
    }
    leds |= pow2(i);    
    module.setLEDs(leds);
    display[i] = CODE[i];
    module.setDisplayToString(display);
  }
  free(display);

  state = FINISHED;
  handleClick(&state);
  readInput(&state); 
}

void handleClick(int *state){
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
      *state = IN_PROGRESS;
      Serial.println("out");
      break;
    case FINISHED:
      for(int key = 0; /*!initCode() ||*/ key != 2; key = module.getButtons()){
        readInput(state);
        delay(100);
        if(*state == RESET){
          break;
        }
      }
      *state = IN_PROGRESS;
      break;
    default:
      Serial.println("Strange program state");
      *state = IN_PROGRESS;
      break;
  }
}

void readInput(int *state){
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

int pow2(int p){ 
  if(p < 0){
    return 0;
  }
  
  int result = 1;
  while (p-- > 0) {
    result *= 2;
  }
  return result;
}
