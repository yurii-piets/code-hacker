#include <TM1638.h>

char CODE[] = "3E6F5678";
int DISP = 10;
int TIMES = 0;

// Ustawienia pinów
const int strobe = 7;
const int clock = 9;
const int data = 8;

const int DISPLAY_SIZE = 9;
const int STEP_DISP = 10;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'};

TM1638 module(data, clock, strobe);

//Trzy możliwe stany programu
const int IN_PROGRESS = 0;
const int WAITING = 1;
const int FINISHED = 2; 

int state;

//Definicje funkcji
char* getRandomArray();
int pow2(int);
void step_delay();
void handleClick();


void setup() {
  Serial.begin(9600);
  module.clearDisplay();
}

void loop() {
  char *disp = (char *) malloc(DISPLAY_SIZE);
  int leds = 0;
  module.clearDisplay();
  module.setLEDs(leds);
  state = IN_PROGRESS;
  
  for(int i = 0; i < DISPLAY_SIZE-1; ++i){    
    char *randomArray = getRandomArray();
    while(randomArray[i] != CODE[i]){
      free(randomArray);
      randomArray = getRandomArray();

      for(int j = 0; j < DISPLAY_SIZE-1; ++j){
         disp[j] = j < i ? CODE[j] : randomArray[j];
      }
      
      module.setDisplayToString(disp);
      //step_delay();
      delay(DISP);
      
      state = IN_PROGRESS;
      handleClick();
    }

    leds |= (int) pow2(i);    
    module.setLEDs(leds);
    free(randomArray);
  }
  
  free(disp);
  
  state = FINISHED;
  readInput(); //remove
  //handleClick(); //uncomment
}

void handleClick(){
  if(state == IN_PROGRESS){
    int keys = module.getButtons();
    if (keys == 1){
      state = WAITING;
    }
  }

  switch(state){
    case IN_PROGRESS:
      break;
    case WAITING:
      for(int key = 0; key != 1; key = module.getButtons()){
        delay(200);
      }
      state = IN_PROGRESS;
      break;
    case FINISHED:
      for(int key = 0; key != 2; key = module.getButtons()){}
      state = IN_PROGRESS;
      break;
    default:
      Serial.println("Strange program state");
      state = IN_PROGRESS;
      break;
  }
}


void readInput(){
  while(Serial.available() > 0){
    char rc = Serial.read();
    switch(rc){
      case 'C':
        Serial.println("C command");
        if(Serial.available() > 7){
          initCode();
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      case 'N':
        Serial.println("N command");
        if(Serial.available() > 1){
          initTimes();
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      case 'D':
        Serial.println("D command");
        if(Serial.available() > 1){
          initDisp();
        } else {
          Serial.println("Wrong format of command");
        }
        break;
      default:
        Serial.println("Wrong command");
        break;
    }
  }
}

void initCode(){
  char newCode[DISPLAY_SIZE] = "\0";
  for(int i = 0; i < 8 && Serial.available() > 0; ++i){
    char rc = Serial.read();
    if(isAllowed(rc)){
      newCode[i] = rc;
    }else{
      Serial.print("Occured wrong value: ");Serial.println(rc);
      return;
    }
  }
  memcpy(CODE, newCode, 8);
  Serial.println(CODE);
}

void initTimes(){
  char newTimes[4] = "\0";
  for(int i = 0; i < 3 && Serial.available() > 0; ++i){
    char rc = Serial.read();
    if(rc >= '0' && rc <= 'rc'){
      newTimes[i] = rc;
    }else{
      Serial.print("Occured wrong value: ");Serial.println(rc);
      return;
    }
  }  
  int tmp;
  sscanf(newTimes, "%d", &TIMES);
  Serial.println(TIMES);
}

void initDisp(){
  char newTimes[5] = "\0";
  for(int i = 0; i < 5 && Serial.available() > 0; ++i){
    char rc = Serial.read();
    if(rc >= '0' && rc <= 'rc'){
      newTimes[i] = rc;
    }else{
      Serial.print("Occured wrong value: ");Serial.println(rc);
      return;
    }
  }  
  int tmp;
  sscanf(newTimes, "%d", &DISP);
  Serial.println(DISP);
}

boolean isAllowed(char value){
  for(int i = 0 ; i < 16; ++i){
    if(allowedChars[i] == value){
      return true;
    }
  }
  return false;
}

void flush(){
  while(Serial.available() > 0){
    Serial.read();
  }  
}

void step_delay(){
  for(int k = 0; k < DISP; k += STEP_DISP){        
    delay(STEP_DISP);
  }
}

char* getRandomArray(){
  char *current = (char *) malloc(DISPLAY_SIZE);

  for(int i=0; i < DISPLAY_SIZE; ++i){
    int randomValue = random(0, 16);
    current[i] = allowedChars[randomValue];
  }
  
  return current;
}

int pow2(int p){ 
  if(p < 0){
    return 0;
  }
  
  int result = 1;
  
  for(int i = 0; i < p ; ++i){
    result *=2;
  }

  return result;
}
