#include <TM1638.h>

//char CODE[] = "3AE61Cb1";
char CODE[] = "0AbCdEF1";
int DISP = 100;
int TIMES = 0;

// Ustawienia pinów
const int strobe = 7;
const int clock = 9;
const int data = 8;

const int DISPLAY_SIZE = 8;
const int STEP_DISP = 10;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'};

TM1638 module(data, clock, strobe);

//Cztery możliwe stany programu
const int IN_PROGRESS = 0;
const int WAITING = 1;
const int FINISHED = 2; 
const int RESET = 3;

//Definicje funkcji
char* getRandomArray();
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

void loop() {
  char *display = (char *) malloc(DISPLAY_SIZE + 1);
  int leds = 0;
  module.clearDisplay();
  module.setLEDs(leds);
  int state = IN_PROGRESS;
  
  for(int i = 0; i < DISPLAY_SIZE; ++i){    
    char *randomArray = getRandomArray();
    while(randomArray[i] != CODE[i]){
      free(randomArray);
      randomArray = getRandomArray();

      for(int j = 0; j < DISPLAY_SIZE; ++j){
          display[j] = j < i ? CODE[j] : randomArray[j];
      }
      
      module.setDisplayToString(display);
      delay(DISP);
      
      state = IN_PROGRESS;
      handleClick(&state);
      readInput(&state);
      if(state == RESET){
        return;
      }
    }

    leds |= pow2(i);    
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
      for(int key = 0; key != 1; key = module.getButtons()){
        delay(200);
      }
      *state = IN_PROGRESS;
      break;
    case FINISHED:
      Serial.println("finished");
      for(int key = 0; /*!initCode() ||*/ key != 2; key = module.getButtons());
      *state = IN_PROGRESS;
      Serial.println(*state);
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
