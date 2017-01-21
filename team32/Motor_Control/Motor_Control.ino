#include <assert.h>
#include <math.h>
#include "configuration.h"

static const int X_ENABLE_PIN = 6; //driver enable pin
static const int X_STEP_PIN = 5; //driver step pin
static const int X_DIR_PIN = 4; //driver direction pin
static const int X_LIMIT_PIN = 10;

static const int Y_ENABLE_PIN = 26; //driver enable pin
static const int Y_STEP_PIN = 25; //driver step pin
static const int Y_DIR_PIN = 24; //driver direction pin
static const int Y_LIMIT_PIN = 30;


//Global values for the current position of the 



struct position{
  int x;
  int y;
};

struct position currentTile;

void setup() {  
  Serial.begin(BAUD);  
  initX();
  initY();
  
  homeX();
  homeY();
}

void moveTo(int x, int y){ //only works if dx = dy, or x=0 or y=0, since these are the only movements needed
  //x and y distances to move
  int dx = (x-currentTile.x)*TILE_WIDTH;
  int dy = (y-currentTile.y)*TILE_WIDTH;
    Serial.println(dx);
  Serial.println(dy);
  if (dx==0) {
    stepY(-dy);
    currentTile.y = y;
    return;
  }
  if (dy==0){
    stepX(dx);
    currentTile.x = x;
    return;
  }

  stepDiag(dx, dy);

  currentTile.x = x;
  currentTile.y = y;
}

void moveFrom(int x1, int y1, int x2, int y2){ //This will be used for moving the pieces - not jumping


}

void initX(){
  pinMode(X_LIMIT_PIN, INPUT); //Init X axis limit switch
  pinMode(X_ENABLE_PIN ,OUTPUT); // Enable  
  pinMode(X_STEP_PIN,OUTPUT); // Step  
  pinMode(X_DIR_PIN,OUTPUT); // Dir
  digitalWrite(X_ENABLE_PIN,LOW); // Turns on the motor driver
 
}

void initY(){
  pinMode(Y_LIMIT_PIN, INPUT); //Init X axis limit switch
  pinMode(Y_ENABLE_PIN ,OUTPUT); // Enable  
  pinMode(Y_STEP_PIN,OUTPUT); // Step  
  pinMode(Y_DIR_PIN,OUTPUT); // Dir
  digitalWrite(Y_ENABLE_PIN,LOW); // Turns on the motor driver
}

void stepX(int mm){ //takes in mm movement and direction
  if (mm>=0) digitalWrite(X_DIR_PIN,HIGH); 
  else digitalWrite(X_DIR_PIN,LOW); 
  int i;
  double steps = STEPS_PER_MM*abs(mm);
  for (i = 0; i<steps;i++){
    digitalWrite(X_STEP_PIN,HIGH); // Output high    
    delay(SPEED); // Wait    
    digitalWrite(X_STEP_PIN,LOW); // Output low    
    delay(SPEED); // Wait  
  }
}

void stepDiag(int mmX, int mmY){
  if (mmX>=0) digitalWrite(X_DIR_PIN,HIGH); 
  else digitalWrite(X_DIR_PIN,LOW); 
  
  if (mmY>=0) digitalWrite(Y_DIR_PIN,LOW); 
  else digitalWrite(Y_DIR_PIN,HIGH); 
  
  int i;
  double steps = sqrt((mmX*mmX)+(mmY*mmY))*STEPS_PER_MM;
  steps *= 0.7;
  Serial.println(steps);
  for (i = 0; i<steps;i++){
    digitalWrite(X_STEP_PIN,HIGH);
    digitalWrite(Y_STEP_PIN,HIGH);    
    delay(SPEED); // Wait    
    digitalWrite(X_STEP_PIN,LOW);
    digitalWrite(Y_STEP_PIN,LOW);
    delay(SPEED); // Wait  
  }
}

void stepY(int mm){ //takes in mm movement and direction
  if (mm>=0) digitalWrite(Y_DIR_PIN,HIGH);  
  else digitalWrite(Y_DIR_PIN,LOW); 
  
  int i;
  double steps = STEPS_PER_MM*abs(mm);
  for (i = 0; i<steps;i++){
    digitalWrite(Y_STEP_PIN,HIGH); // Output high    
    delay(SPEED); // Wait    
    digitalWrite(Y_STEP_PIN,LOW); // Output low    
    delay(SPEED); // Wait  
  }
}

void homeX(){ //Homing the x axis moves the X axis until it reaches it's 0 point, i.e. when the limit switch is pressed
  digitalWrite(X_DIR_PIN,LOW); 
  while (digitalRead(X_LIMIT_PIN)==LOW){
    digitalWrite(X_STEP_PIN,HIGH);
    delay(SPEED);
    digitalWrite(X_STEP_PIN,LOW);
    delay(SPEED);
  }
   stepX(10);
   currentTile.x = 0;
}

void homeY(){ //Homing the y axis
  digitalWrite(Y_DIR_PIN,HIGH); 
  while (digitalRead(Y_LIMIT_PIN)==LOW){
    digitalWrite(Y_STEP_PIN,HIGH);
    delay(SPEED);
    digitalWrite(Y_STEP_PIN,LOW);
    delay(SPEED);
  }
  stepY(-72);
  currentTile.y=0;
  
}


void loop() {
  while (Serial.available()){
    String input;
    input = Serial.readString();
    int x = input[0]-97;
    int y = input[1]-49;
    Serial.print("MOVING TO ");
    Serial.print(x);
    Serial.print("   ");
    Serial.println(y);
    moveTo(x,y);
  }
}
