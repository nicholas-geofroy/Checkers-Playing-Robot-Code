#include <stdlib.h>

 const int empty = 0; //empty space
 const int player1 = 1; //human player
 const int player2 = -1; //AI player
 const int player1King = 2; //human king
 const int player2King = -2; //AI king
 const int player2KingY = 0;
 const int invalidTile = -10;
 int count;


//==================================================STRUCTURES FOR REFERENCE=============================
/*struct coordinate{
  int x;
  int y;
};

struct moves{
  struct coordinate start;
  struct coordinate destination;
  bool selected;
  
};*/ 

const int maxNmbrOfMovesToGenerate = 100;

struct moves *generateMove(int board[8][8], int player, int *nmbrOfMoves){ //changes nmbrOfMoves to the number of consecutive moves that need to be made by the AI
  

  struct moves possibleMoves[maxNmbrOfMovesToGenerate] = {};
  findLegalMoves(board, possibleMoves, player);
  
  return maxMove(board, possibleMoves, nmbrOfMoves); //returns a pointer to the best move
  
}

void findLegalMoves(int board[8][8], struct moves possibleMoves[], int state){
  //This funciton iterates through the checkers board and finds the possible moves for the specific player
  count = 0; //global variable that is used to taly the total number of moves generated
  
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      if(board[i][j] == state || board[i][j] == state*2){        
         scanPiece(j,i,board, possibleMoves);
      }        
    }
    
  }
  
  
  
  
  return;
}

void scanPiece(int x, int y, int board[8][8],struct moves possibleMoves[]){ 
  
  int state = board[y][x]; //state of the checkers piece we are looking at

  //===================================REGUALR PIECE=============================================
  if(state == player1 || state == player2){ //if the piece is a regular piece

    //IN FRONT AND TO THE RIGHT OF THE PIECE
    int temp = checkTile(board, x+1,y+state); //checks to the right and front of the piece
    if (temp == empty){  //checks for an empty space
      setPossibleMoves(x,y,x+1,y+state,possibleMoves);
    }
    //checks for an enemy with an empty space on the opposite side of it    
    if((temp == -1*state || temp == -2*state) && checkTile(board,x+2,y+2*state) != invalidTile){
      checkForJump(x,y,state,x+1,y+state,board,possibleMoves);
    }

    //IN FRONT AND TO THE LEFT OF THE PIECE
    temp = checkTile(board, x-1, y+state);
    if (temp == empty){
      setPossibleMoves(x,y,x-1,y+state,possibleMoves);
    }
    if((temp == -1*state || temp == -2*state) && checkTile(board,x-2,y+2*state) != invalidTile){
      checkForJump(x,y,state,x-1,y+state,board,possibleMoves);
    }
  }

  //===============================KING PIECE==================================================
  
  if(state == player1King || state == player2King){

    //FRONT RIGHT
    int temp = checkTile(board, x+1,y+1);
    if (temp == empty)setPossibleMoves(x,y,x+1,y+1,possibleMoves);
    if((temp == -1*state/2 || temp == -1*state) && checkTile(board,x+2,y+2) != invalidTile) checkForJump(x,y,state,x+1,y+1,board,possibleMoves);

    //FRONT LEFT
    temp = checkTile(board, x-1, y+1);
    if (temp == empty)setPossibleMoves(x,y,x-1,y+1,possibleMoves);
    if((temp == -1*state/2 || temp == -1*state) && checkTile(board,x-2,y+2) != invalidTile) checkForJump(x,y,state,x-1,y+1,board,possibleMoves);

    //BACK RIGHT
    temp = checkTile(board, x+1,y-1);
    if (temp == empty)setPossibleMoves(x,y,x+1,y-1,possibleMoves);
    if((temp == -1*state/2 || temp == -1*state) && checkTile(board,x+2,y-2) != invalidTile) checkForJump(x,y,state,x+1,y-1,board,possibleMoves);

    //BACK RIGHT
    temp = checkTile(board, x-1, y-1);
    if (temp == empty)setPossibleMoves(x,y,x-1,y-1,possibleMoves);
    if((temp == -1*state/2 || temp == -1*state) && checkTile(board,x-2,y-2) != invalidTile) checkForJump(x,y,state,x-1,y-1,board,possibleMoves);
  }
  

}
int checkTile(int board[8][8], int x, int y){ //Checkes the specific tile in the array and outputs its value (if the tile is valid)
  if(x>7 || x < 0 || y > 7 || y < 0) return invalidTile;
  else return board[y][x]; 
}

void checkForJump(int originx, int originy, int originState, int enemyx, int enemyy, int board[8][8],struct moves possibleMoves[]){ //function to detect jumping that calls itself recursively for multiple jumps
  
  
  if(enemyx == 100 && enemyy == 100){ //these parameters will only be set to 100 if the funstion is called recursively
      
    if(originState*originState == 1){ //if the piece is a pawn
      //if either of the two pieces infront of it are 'jumpable' calls this function again
      if((checkTile(board, originx+1,originy+originState) == -1*originState || checkTile(board, originx+1,originy+originState) == -2*originState) && checkTile(board, originx+2,originy+2*originState) != invalidTile){
        checkForJump(originx,originy,originState,originx+1,originy+originState, board,possibleMoves);
      }
      if((checkTile(board, originx-1,originy+originState) == -1*originState || checkTile(board, originx-1,originy+originState) == -2*originState) && checkTile(board, originx-2,originy+2*originState) != invalidTile){
        checkForJump(originx,originy,originState,originx-1,originy+originState, board,possibleMoves);
      }
        
    }
    if(originState*originState == 4){ //the piece we are looking at is a king, checks all the possible four tiles around it and sees if a jump is available
      
      if((checkTile(board, originx+1,originy+1) == -1*originState || checkTile(board, originx+1,originy+1) == -1*originState/2) && checkTile(board, originx+2,originy+2) != invalidTile){
        checkForJump(originx,originy,originState,originx+1,originy+1, board,possibleMoves);
      }
      
      if((checkTile(board, originx-1,originy+1) == -1*originState|| checkTile(board, originx-1,originy+1) == -1*originState/2) && checkTile(board, originx-2,originy+2) != invalidTile){
        checkForJump(originx,originy,originState,originx-1,originy+1, board,possibleMoves);
      }
      
      if((checkTile(board, originx+1,originy-1) == -1*originState || checkTile(board, originx+1,originy-1) == -1*originState/2) && checkTile(board, originx+2,originy-2) != invalidTile){
        checkForJump(originx,originy,originState,originx+1,originy-1, board,possibleMoves);
      }
      
      if((checkTile(board, originx-1,originy-1) == 1*originState || checkTile(board, originx-1,originy-1) == 1*originState/2) && checkTile(board, originx-2,originy-2) != invalidTile){
        checkForJump(originx,originy,originState,originx-1,originy-1, board,possibleMoves);
      }
    }
    return; //if there is no possible second jump, ends the call
  }
  
  int destinationX = (2*enemyx-originx);
  int destinationY = (2*enemyy-originy);
  
  if(board[destinationY][destinationX] == empty){
      setPossibleMoves(originx,originy,destinationX,destinationY,possibleMoves);
      checkForJump(destinationX,destinationY,originState,100,100,board,possibleMoves);
      
  }
  else return;
    
}

void setPossibleMoves(int x1, int y1, int x2, int y2,struct moves possibleMoves[]){
  
  possibleMoves[count].start.x = x1;
  possibleMoves[count].start.y = y1;
  possibleMoves[count].destination.x = x2;
  possibleMoves[count].destination.y = y2;
  count++;
  
}



int value(int board[8][8]){ //Ranks the checkers board based on this arbitrary scoring method
  int value = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j ++){
      if(board[i][j] == player1) value-=2; //minus 2 points of each enemy piece
      if(board[i][j] == player2) value+=2; //plus 2 points for each AI piece
      if((j == 0 || j == 7) && board[i][j] > empty) value-=1; //dislikes if the enemy has a piece on the side (impossible to jump) 
      if((j == 0 || j == 7) && board[i][j] < empty) value+=1; //if the move will allow player 2 to get a side piece increases the score
      if(board[i][j] == player1King) value -= 5; //minus 5 points for every human king
      if(board[i][j] == player2King) value += 5; //plus 5 points for every AI king 
    }
  }
  return value;
}

struct moves *maxMove(int board[8][8],struct moves possibleMoves[], int *lengthOfBestMove){
  
  int bestVal = -10000; //temporary val that will be changed to be the highest bossible value
  int bestBoard[8][8] = {}; //the move that maximises value will be saved here
  struct moves *bestMove;

  for(int i = 0; i < count;){ //for each of the moves generated in the other code
    
    int newBoard[8][8] = {};
    //copies the current board layout into newBoard
    for(int j = 0; j < 8; j++){
      for(int k = 0; k < 8; k++){
        newBoard[j][k] = board[j][k];
      }
    }
   
   int moveNmbr = 0; //value for counting the number of jumps made in the case of multiple jumps
   struct moves *tempMove = &possibleMoves[i]; //used to save the first move in the case of multiple jumps
    do{

      //moves the piece to its destination
      int temp = newBoard[possibleMoves[i].start.y][possibleMoves[i].start.x];
      newBoard[possibleMoves[i].start.y][possibleMoves[i].start.x] = empty;
      if(possibleMoves[i].destination.y == player2KingY) {
        temp = player2King;
      }
      newBoard[possibleMoves[i].destination.y][possibleMoves[i].destination.x] = temp;
      
      if(abs(possibleMoves[i].start.x - possibleMoves[i].destination.x) == 2 && abs(possibleMoves[i].start.y - possibleMoves[i].destination.y) == 2){ //meaning a jump move
        
        newBoard[(possibleMoves[i].start.y-possibleMoves[i].destination.y)/2 + possibleMoves[i].destination.y][(possibleMoves[i].start.x-possibleMoves[i].destination.x)/2 + possibleMoves[i].destination.x] = empty; //deletes the piece that has been jumped
        
      }
      moveNmbr++;
      i++;
    }while(i < count && possibleMoves[i-1].destination.x == possibleMoves[i].start.x && possibleMoves[i-1].destination.y == possibleMoves[i].start.y); //will continue to loop if there are successive jump moves after each other
   
    int temp = value(newBoard);
    
    if(temp > bestVal){ //if this new move is better than the old best move

      //saves the best move into the respective variables
      bestMove = tempMove;
      *lengthOfBestMove = moveNmbr;
      bestVal = temp;
      
      for(int j = 0; j < 8; j++){
        for(int k = 0; k < 8; k++){
          bestBoard[j][k] = newBoard[j][k];
        }
      }
    }  
  }
  
  return bestMove; //returns the location of the best move in the moves array
}




