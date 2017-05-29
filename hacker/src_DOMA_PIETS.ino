#include <TM1638.h>

//char CODE[] = "3AE61Cb1";
String CODE = "3AE61Cb1";
byte TIMES = 10;
int DISP = 100;

// Ustawienia pinów
const byte strobe = 7;
const byte clock = 9;
const byte data = 8;

const byte DISPLAY_SIZE = 8;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'};

TM1638 module(data, clock, strobe);

//Cztery możliwe stany programu
typedef enum {IN_PROGRESS, WAITING, FINISHED, RESET} states;

//Definicje funkcji
void doHack();
void handleClick(states *);
void readInput(states *);
boolean initCode();
void initTimes();
void initDisp();
boolean isAllowed(char);


void setup() {
  Serial.begin(9600);
  module.clearDisplay();
}

void loop(){
  doHack();
}

void doHack(){
  char *display = (char *) malloc(DISPLAY_SIZE + 1);
  byte leds = 0;
  module.clearDisplay();
  module.setLEDs(leds);
  states state = IN_PROGRESS;
  
  for(byte i = 0; i < DISPLAY_SIZE; ++i){    
    
    char *randomArray = getRandomArray();

    do{
      free(randomArray);      
      
      randomArray = getRandomArray();
      for(byte j = 0; j < DISPLAY_SIZE; ++j){
          display[j] = j < i ? CODE[j] : randomArray[j];
      }
      
      module.setDisplayToString(display);
      
      handleClick(&state);
      readInput(&state);

      delay(DISP);
      if(state == RESET){
        free(display);
        free(randomArray);
        return;
      }
    } while(randomArray[i] != CODE[i]);
    
    leds = (leds << 1) +1;   
    module.setLEDs(leds);
    free(randomArray);
  }
  
  free(display);
  
  state = FINISHED;
  handleClick(&state);
  readInput(&state);   
}

char* getRandomArray(){
  char *current = (char *) malloc(DISPLAY_SIZE+1);

  for(int i=0; i < DISPLAY_SIZE; ++i){
    int randomValue = random(0, 16);
    current[i] = allowedChars[randomValue];
  }
  
  return current;
}

void handleClick(states *state){
  if (*state == IN_PROGRESS) {
    byte key = module.getButtons();
    if (key == 1) {
      *state = WAITING;
    }
  }

  switch(*state){
    case IN_PROGRESS:
      break;
    case WAITING:
      for(byte key = 0; key != 1; key = module.getButtons()){
        key = 0;
        delay(100);
        key = 0;
      }
      *state = IN_PROGRESS;
      break;
    case FINISHED:
      for(byte key = 0; key != 2; key = module.getButtons()){
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
    delay(100);
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
        Serial.print("Wrong command: "); Serial.println(rc);
        break;
    }
  }
}

boolean initCode(){
  String newCode = "\0";
  for(byte i = 0; i < 8 && Serial.available() > 0; ++i){
    char rc = Serial.read();
    if(isAllowed(rc)){
      newCode += rc;
    } else {
      Serial.print("Occured wrong value: "); Serial.println(rc);
      return false;
    }
  }

  if( newCode.length() != 8 ){
    Serial.print("Wrong input length: ");Serial.print(newCode.length());
    return false;
  }

  CODE = newCode;
  Serial.print("New code: "); Serial.println(CODE);
  return true;
}

void initTimes(){
  char newTimes[4] = "\0";
  for(byte i = 0; i < 3 && Serial.available() > 0; ++i){
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

void initDisp(){
  char newTimes[5] = "\0";
  for(byte i = 0; i < 5 && Serial.available() > 0; ++i){
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
  for(byte i = 0 ; i < 16; ++i){
    if(allowedChars[i] == value){
      return true;
    }
  }
  return false;
}
