#include <TM1638.h>

// Ustawienia pinów
const int strobe = 7;
const int clock = 9;
const int data = 8;

const int DISPLAY_SIZE = 9;
const char allowedChars[] = {'0','1','2','3','4','5','6','7','8','9','A','b','C','d','E','F'}; 

TM1638 module(data, clock, strobe);

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
  char code[DISPLAY_SIZE] = "bd0A16F7";
  
  for(int i = 0; i < DISPLAY_SIZE; ++i){
    char *randomArray = "constant";
    while(randomArray[i] != code[i]){
      randomArray = getRandomArray();
      
      for(int j = 0; j < DISPLAY_SIZE; ++j){
        display[j] = j < i ? code[j] : randomArray[j];
      }
      
      module.setDisplayToString(display);
      delay(50);
    }
  }

  delay(10000);
}
