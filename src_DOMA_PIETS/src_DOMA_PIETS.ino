#include <TM1638.h>

// Parametry programu
char CODE[] = "3AE61Cb1";
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
char* getRandomArray();
void handleClick(states *);
void readInput(states *);
boolean initCode();
void initTimes();
void initDisp();
boolean isAllowed(char);
inline void waitTillRelease();

void setup() {
  Serial.begin(9600);
  module.clearDisplay();
}

void loop(){
  char *display = (char *) malloc(DISPLAY_SIZE + 1);
  char *randomArray;
  byte leds = 0;
  module.setLEDs(leds);
  states state = IN_PROGRESS;

  for(byte i = 0; i < DISPLAY_SIZE; ++i){

    for(byte j = 0; j < TIMES; ++j) {

      randomArray = getRandomArray();
      for(byte k = 0; k < DISPLAY_SIZE; ++k){
          display[k] = ( k < i ? CODE[k] : randomArray[k] );
      }
      free(randomArray);
      module.setDisplayToString(display);

      // Zmiany parametrów
      readInput(&state);
      long time = millis() + DISP; // Pobieraj info o przyciskach do tego czasu
      do
      {
        handleClick(&state);
      } while (time > millis());

      if(state == RESET){
        free(display);
        return;
      }
    }

    leds = (leds << 1) +1; // leds*2 +1
    module.setLEDs(leds);
  }
  free(display);
  module.setDisplayToString(CODE); // Wynik łamania kodu
  // Zmiany parametrów po zakończeniu
  state = FINISHED;
  handleClick(&state);
}

char* getRandomArray(){
  char *current = (char *) malloc(DISPLAY_SIZE+1);
  byte randomValue;

  for(byte i=0; i < DISPLAY_SIZE; ++i){
    randomValue = random(0, 16);
    current[i] = allowedChars[randomValue];
  }

  return current;
}

void handleClick(states *state){
  byte key;

  switch(*state){
    case IN_PROGRESS:
      key = module.getButtons();
      if (key == 1) {
        *state = WAITING;
      }
      else break;
      waitTillRelease();
    case WAITING:
      for(key = 0; key != 1; key = module.getButtons()){
        delay(100);
      }
      *state = IN_PROGRESS;
      waitTillRelease();
      break;
    case FINISHED:
      for(key = 0; key != 2; key = module.getButtons()){
        readInput(state);
        delay(100);
        if(*state == RESET){
          break;
        }
      }
      break;
  }
}

void readInput(states *state){
  while(Serial.available() > 0){
    delay(100);
    char rc = Serial.read();
    switch(rc){
      case 'C': // zmiana kodu
        Serial.println("C command");
        if(Serial.available() > 0) {
          if(initCode() != false){
            *state = RESET;
          }
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      case 'N': // zmiana ilości iteracji
        Serial.println("N command");
        if(Serial.available() > 0) {
          initTimes();
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      case 'D': // zmiana czasu wyświetlania
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
  char newCode[DISPLAY_SIZE+1] = "\0";
  char rc;
  for(byte i = 0; i < 8 && Serial.available() > 0; ++i){
    rc = Serial.read();
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

void initTimes(){
  char newTimes[4] = "\0";
  char rc;
  for(byte i = 0; i < 3 && Serial.available() > 0; ++i){
    rc = Serial.read();
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
  char rc;
  for(byte i = 0; i < 5 && Serial.available() > 0; ++i){
    rc = Serial.read();
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

inline void waitTillRelease() {
  while(module.getButtons() != 0) delay(20);
}