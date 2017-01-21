#include <string.h>

const unsigned int line1Y = 5;
const unsigned int line2Y = 20;

const unsigned int col1X = 5;
const unsigned int col2X = 20;
const unsigned int col3X = 85;

const unsigned int wordSize = 8;
const unsigned int blinkPeriod = 600;

static enum State{
  mainMenu = 0,
  playerMove = 1,
  movingMotors = 2,
  doneUI = 3
} displayState = mainMenu;

struct button{
  int pin;
  bool state;
  bool prevState;
};



struct button Ubutton={UbuttonPin,false,false};


void changeUIState(int n){
  //resets the button states just in case they were down when the state was previously changed
  Ubutton.state = false;
  Ubutton.prevState = false;
  switch (n){
    case 0:
      displayState = mainMenu;
      break;
    case 1:
      displayState = playerMove;
      break;
    case 2:
      displayState = movingMotors;
      break;
     
  }
}

bool UIUpdate(){

    flushBuffer(); //clears the serial buffer as it messes up the OLED
  
  switch (displayState){
    case mainMenu:
      updateMainMenu();
      break;
    case playerMove:
      updateMoveSelect();
      break;
    case movingMotors:
      movingMotorsDisplay();
      break;
    case doneUI:
      return true;
  }

  Ubutton.prevState = Ubutton.state;
  Ubutton.state = digitalRead(Ubutton.pin);
  
  OrbitOledUpdate();
  return false;
}

void updateMainMenu(){

  
  char txt[] = "Press BTN2";
  int n = sizeof(txt)/sizeof(txt[0]);
  
  OrbitOledMoveTo(col1X,line1Y);
  OrbitOledDrawString("Checkersbot9000");
  OrbitOledMoveTo(col2X,line2Y);
  OrbitOledDrawString("Press BTN 2");
  
    if(!Ubutton.state && Ubutton.prevState){ //once the button is pressed changes the state
      displayState = playerMove;
      OrbitOledClear();
      OrbitOledClearBuffer();
      OrbitOledInit();
      Ubutton.prevState = false;
      delay(100);
      
    
  }

  
}

void updateMoveSelect(){

  OrbitOledMoveTo(col1X, line1Y);
  OrbitOledDrawString("Press B2 to");
  OrbitOledMoveTo(col1X, line2Y);
  OrbitOledDrawString("end your turn");

    if(Ubutton.state == false && Ubutton.prevState == true){ //if one of the buttons has been pressed down
      
      displayState = movingMotors;
      Ubutton.prevState = false; //sets it back to on
      
      OrbitOledClear();
      OrbitOledClearBuffer();
      OrbitOledInit();
           
    }

}

void movingMotorsDisplay(){
  
  OrbitOledMoveTo(col1X, line1Y);
  OrbitOledDrawString("It's My Turn");
  OrbitOledMoveTo(col2X,line2Y);
  OrbitOledDrawString("Now");
  displayState = doneUI;
  
}


