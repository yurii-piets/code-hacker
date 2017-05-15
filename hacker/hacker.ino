#include <TM1638.h>


char CODE[] = "3E6F5678";
int DISP = 200;

// Ustawienia pinów
const int strobe = 7;
const int clock = 9;
const int data = 8;

const int DISPLAY_SIZE = 9;
const int STEP_DISP = 10;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'};

TM1638 module(data, clock, strobe);

//Definicje funkcji
char* getRandomArray();
int pow2(int);
void step_delay();


void setup() {
  Serial.begin(9600);
  module.clearDisplay();
}

void loop() {
  char *disp = (char *) malloc(DISPLAY_SIZE);
  int leds = 0;
  Serial.println("hello");
  module.clearDisplay();
  module.setLEDs(leds);
  
  for(int i = 0; i < DISPLAY_SIZE-1; ++i){    
    char *randomArray = getRandomArray();
    while(randomArray[i] != CODE[i]){
      free(randomArray);
      randomArray = getRandomArray();

      for(int j = 0; j < DISPLAY_SIZE-1; ++j){
         disp[j] = j < i ? CODE[j] : randomArray[j];
      }
      
      module.setDisplayToString(disp);
      step_delay();
    }

    leds |= (int) pow2(i);    
    module.setLEDs(leds);
    free(randomArray);
  }
  
  free(disp);
  Serial.println("bye");
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
