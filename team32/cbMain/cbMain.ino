#include <Servo.h>
#include <stdlib.h>
#include <stdio.h>
#include <FillPat.h>
#include <LaunchPad.h>
#include <OrbitBoosterPackDefs.h>
#include <OrbitOled.h>
#include <OrbitOledChar.h>
#include <OrbitOledGrph.h>
#include "pins.h"

struct coordinate{
  int x;
  int y;
  bool isSelected;
};

struct moves{
  struct coordinate start;
  struct coordinate destination;
  
};

int currentPlayer = 0;

void UIUpdate(struct moves *playerMoves, int playernmbr);
void changeUIState(int n);
struct moves *generateMove(int board[8][8], int player, int * nmbrOfMoves); //takes an 8x8 checkers board correllating to the actual board. Each element has a 1 for player 1's piece, a 2 for player 2's piece, or a zero for no piece on it

static enum state {
  UIState = 0,
  AIState = 1
} programState = UIState;

void flushBuffer(){//since serial.flush() doesn't work
  while(Serial.available()){
    Serial.read();
  }
}

void setup() {

  
  Serial.begin(9600);
  pinMode(LbuttonPin, INPUT);
  pinMode(RbuttonPin, INPUT);
  pinMode(UbuttonPin, INPUT);
  pinMode(DbuttonPin, INPUT);

  //Motor Setup
  
  initServo();
  
  initX();
  initY();


  
  homeX();
  homeY();

  //OLED Setup
  OrbitOledInit();
  OrbitOledClear();
  OrbitOledClearBuffer();
  OrbitOledSetFillPattern(OrbitOledGetStdPattern(iptnSolid));
  OrbitOledSetDrawMode(modOledSet);
  
  
}

void loop() {

  switch(programState){
    case UIState:
      if(UIUpdate() == true){
        
        programState = AIState;
      }
      break;
    
    
    case AIState:
      
      flushBuffer();
      
      //board is the array that is gotten from the camera)
      bool hasStarted = false;
      bool notEnd = true;
      int board[8][8] = {0};
      int i = 0; //row
      int j = 0; //column
      do{
        int incomingInt = Serial.read();
        
       //to convert from char to int since serial.read reads integers as characters
        incomingInt -=48;
        
        if(hasStarted){
          //Serial.println(incomingInt); for testing
          //Converts the incoming data to the format the checkersAI code needs for calculations
          if(incomingInt == 1) incomingInt =-1;
          if(incomingInt == 2) incomingInt = -2;
          if(incomingInt == 3) incomingInt = 1;
          if(incomingInt == 4) incomingInt = 2;
          
          if(incomingInt != -49){ //-49 is the serial read reading nothing as Serial.Available() buggs out sometimes
            board[i][j] = incomingInt; //will only
          
            j++;
            if(j == 8){
              i++;
              j=0;  
            }
            if(incomingInt == 6) notEnd = false; //the input will always end with a 6
          }
        }
        if(incomingInt == 5) hasStarted = true; //the input will always begin with a 5
      } while(notEnd);
      
      int nmbrOfMoves = 0; 
      struct moves* bestMoves = generateMove(board,-1, &nmbrOfMoves); //Calls the AI code
   

      for (int i = 0; i<nmbrOfMoves;i++){
          if(abs(bestMoves[i].start.x - bestMoves[i].destination.x) == 2 && abs(bestMoves[i].start.y - bestMoves[i].destination.y) == 2){ //meaning a jump move
            initX();
            initY();
              
            kill(7-((bestMoves[i].start.x-bestMoves[i].destination.x)/2 + bestMoves[i].destination.x),7-((bestMoves[i].start.y-bestMoves[i].destination.y)/2 + bestMoves[i].destination.y)); //deletes the piece that has been jumped, the 7- is due to the motors being oriented different than the camera input or AI code
            moveFromTo(bestMoves[i].start.x,bestMoves[i].start.y,bestMoves[i].destination.x,bestMoves[i].destination.y);
          }
          else{ //if no jump is made
              initX();
              initY();
            moveFromTo(bestMoves[i].start.x, bestMoves[i].start.y,bestMoves[i].destination.x,bestMoves[i].destination.y);
          }
          if(bestMoves[i].destination.y == 0 && board[bestMoves[i-(nmbrOfMoves-1)].start.y][bestMoves[i-(nmbrOfMoves-1)].start.x]*board[bestMoves[i-(nmbrOfMoves-1)].start.y][bestMoves[i-(nmbrOfMoves-1)].start.x] == 1) promote(7-bestMoves[i].destination.x,7-bestMoves[i].destination.y); //if the piece is a pawn and it reaches the king 'row'
          
      }

      //initialises UI for the player's turn
      changeUIState(1);//when the ui turns on again it goes back to player select
      programState = UIState;
      delay(250);
      OrbitOledClear();
      OrbitOledClearBuffer();
      OrbitOledInit();
      break;

  }

}



