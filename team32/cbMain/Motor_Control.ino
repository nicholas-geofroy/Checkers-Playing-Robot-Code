#include <math.h>
#include "configuration.h"

struct position{
  int x;
  int y;
};

struct position currentTile;

Servo magnetServo;

struct position createPosition(int x, int y){
  struct position newPosition;
  newPosition.x = x;
  newPosition.y = y;
  return newPosition;
}

struct position kings[15] = {
  createPosition(-3,4),createPosition(-3,3),createPosition(-3,2),
  createPosition(-3,0),createPosition(-3,1),createPosition(-2,4),
  createPosition(-2,3),createPosition(-2,2),createPosition(-2,0),
  createPosition(-2,1),createPosition(-1,4),createPosition(-1,3),
  createPosition(-1,2),createPosition(-1,0),createPosition(-1,1)
};

int kingsLeft = 15;

void promote(int x, int y){

  int target_x;
  int target_y = y;//coordinates of the white tile next to the target black tile

  if (x>=4) target_x = x-1;
  else target_x = x+1;


 
  kill(x,y); //kill the piece before replacing it with a king

  moveTo(kings[kingsLeft-1].x, kings[kingsLeft-1].y); //Pick up the next king
  stepX(-20);
  kingsLeft-=1;

  toggleMagnet(1);
  moveTo(-1,1);
  moveTo(0,1);
  stepX(20);
  
  
  while (currentTile.x<target_x){
    if (currentTile.y<target_y||currentTile.y==0){
       moveTo(currentTile.x+1, currentTile.y+1);
    }else if (currentTile.y>=target_y){
       moveTo(currentTile.x+1, currentTile.y-1);
    }
  }

  while (currentTile.y<target_y){
    if (currentTile.x<target_x||currentTile.x==0){
       moveTo(currentTile.x+1, currentTile.y+1);
    }else{
      moveTo(currentTile.x-1, currentTile.y+1);
    }
  }

  moveTo(x,y);
  toggleMagnet(0);
  delay(300);
  homeX();
  homeY();
  
}

void moveTo(int x, int y){

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
  if (abs(dy)==abs(dx)){
    stepDiag(dx, dy);
    currentTile.y = y;
    currentTile.x = x;
    return;
  }
  moveInParts(x, y);
  
}

void moveFromTo(int x1, int y1, int x2, int y2){ //This will be used for moving the pieces - not jumping
  initY();
  initX();
  //toggleMagnet(1);
  Serial.println(x1);
  Serial.println(y1);
 toggleMagnet(0);
  delay(400);
  moveTo(7-x1,7-y1); //from
  toggleMagnet(1);
  delay(400);
  
  Serial.println(x2);
  Serial.println(y2);
  moveTo(7-x2,7-y2); //to
  delay(400);
 //
 toggleMagnet(0);
}

void moveInParts(int x, int y){ //moves x then y
  moveTo(x, currentTile.y);
  moveTo(currentTile.x, y);
}

void toggleMagnet(int up){ //1 for raise, 0 for lower
  if (up){
    magnetServo.write(SERVO_RAISE_ANGLE);
  }else{
    magnetServo.write(SERVO_LOWER_ANGLE);
  }
  delay(80);
}

void kill(int x, int y){
  toggleMagnet(0);

  moveInParts(x, y);
  toggleMagnet(1);

  //moves the piece into a white square towards the center of the board.
  if (x>=4) moveTo(x-1, y);
  else if (x<4) moveTo(x+1, y);

  while(currentTile.y<6){
    moveTo(currentTile.x-1, currentTile.y+1);
    moveTo(currentTile.x+1, currentTile.y+1);
 
  }
  int toggle = (7-currentTile.y); //switch is 0 is y==7, 1 if y==6;
  
  while(currentTile.x>0){
    if (toggle){
      moveTo(currentTile.x-1,currentTile.y+1);
    }else{
      moveTo(currentTile.x-1,currentTile.y-1);
    }
    toggle = !toggle; //invert switch
  }
  stepX(-15);
  moveTo(-2, 7);
  moveTo(-2, 6);
  toggleMagnet(0);
  homeX();
  homeY();
  
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

void initServo(){
  magnetServo.attach(SERVO_PIN);
  toggleMagnet(0);
}

void stepX(int mm){ //takes in mm movement and direction
  Serial.print("MOVING X");
  Serial.println(mm);
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
   stepX(169);
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
  stepY(-44);
  currentTile.y=0;
  
}


/*void loop() { //UNNECESSARY IN MAIN CODE
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
   
}*/
