#include <TM1638.h>

// Ustawienia pinów
const int strobe = 7;
const int clock = 9;
const int data = 8;

TM1638 module(data, clock, strobe);
const int DISPLAY_SIZE = 9;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'}; 


const int WAITING = 0;
const int IN_PROGRESS = 1;
const int FINISHED = 2;

int currentState = IN_PROGRESS;

void setup() {
  Serial.begin(9600);
  module.clearDisplay();
}

char* getRandomArray(){
  char current [DISPLAY_SIZE];

  for(int i=0; i < DISPLAY_SIZE; ++i){
    int randomValue = random(0, 16);
    current[i] = allowedChars[randomValue];
  }
  
  return current;
}

//A b C d E F
void loop() {
  char display[DISPLAY_SIZE];
  char code[DISPLAY_SIZE] = "b10A16F7";
  int leds = 0;
  
  for(int i = 0; i < DISPLAY_SIZE; ++i){
    char *randomArray = "--------";
    
    leds |= (int) pow(2, i - 1);    
    module.setLEDs(leds);
    
    while(randomArray[i] != code[i]){
      randomArray = getRandomArray();
      
      for(int j = 0; j < DISPLAY_SIZE; ++j){
        display[j] = j < i ? code[j] : randomArray[j];
      }
      module.setDisplayToString(display);
      currentState = IN_PROGRESS;
      listenKeys();
      delay(100);
      listenKeys();
    }
  }
  currentState = FINISHED;
  listenKeys();
}

void listenKeys(){
  do{
    int keys = module.getButtons();
    if(keys == 1){
      switch(currentState){
        case IN_PROGRESS:
          currentState = WAITING;
          break;
        case WAITING:
          currentState = IN_PROGRESS;
          break;
        default:
          break;
      }
    }else if (keys == 2){
      if(currentState == FINISHED){
        currentState = IN_PROGRESS;
      }
    }
  
    switch(currentState){
      case IN_PROGRESS:
        return;
      case WAITING:
        delay(50);
        break;
      case FINISHED:
        delay(50);
        break;
      default:
        break;
    }
  }while(1);
}
