#include <Adafruit_TFTLCD.h> 
#include <Adafruit_GFX.h>    
#include <TouchScreen.h>

#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0 
#define LCD_RESET A4 

#define TS_MINX 120
#define TS_MINY 111
#define TS_MAXX 954
#define TS_MAXY 911

#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


extern uint8_t circle[];

extern uint8_t x_bitmap[];

int gameScreen = 1;
int moves = 1;

int winner = 0;  //0 = Draw, 1 = Human, 2 = CPU


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364);

int board[]={0,0,0,0,0,0,0,0,0};// holds position data 0 is blank, 1 human, 2 is computer


boolean buttonEnabled = true;

void setup() {
  
  tft.reset();
  Serial.begin(9600);
  Serial.print("Starting...");
  randomSeed(analogRead(0));
 
  initDisplay();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(WHITE);
  tft.drawRect(0,0,319,240,YELLOW);
  
  tft.setCursor(30,40);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Vamos a jugar triqui!");
  
  tft.setCursor(115,80);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Por: Andres   \n\n    Esteban y Santiago");
  
  tft.setCursor(50,150);
  tft.setTextColor(BLUE);
  tft.setTextSize(2);
  tft.print("Elija una opcion");
  
  tft.fillRect(50,180, 85, 40, RED);
  tft.drawRect(50,180,85,40, BLACK);
  tft.setCursor(60,190);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("PVE");

  tft.fillRect(165,180, 85, 40, RED);
  tft.drawRect(165,180,85,40, BLACK);
  tft.setCursor(170,190);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("PVP");

}
void loop() {
  reset:
  TSPoint p = ts.getPoint();
  
  if (p.z > ts.pressureThreshhold) {
    //This is the PVE
   p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
   p.y = map(p.y, TS_MAXY, TS_MINY, 0, 480);
       
   if(p.x>235 && p.x<280 && p.y>50 && p.y<135 && buttonEnabled){
    
     buttonEnabled = false;
    
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tft.fillScreen(BLUE);
    
    tft.drawRect(0,0,319,240,BLACK);

    drawVerticalLine(125);

    drawVerticalLine(195);

    drawHorizontalLine(80);

    drawHorizontalLine(150);
    playGame();
 
   }
    if(p.x>235 && p.x<280 && p.y>200 && p.y<330 && buttonEnabled){
    //This is the PVP
    buttonEnabled = false;
    
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tft.fillScreen(WHITE);
    
    tft.drawRect(0,0,319,240,BLACK);

    drawVerticalLine(125);

    drawVerticalLine(195);

    drawHorizontalLine(80);

    drawHorizontalLine(150);
    playGame2();
   
   
   }  
  }
}
void(* resetFunc) (void) = 0;
void drawHorizontalLine(int y)
{
  int i=0;
  for(i=0;i<7;i++)
  {
    tft.drawLine(60,y+i,270,y+i,BLACK);
  }
}

void drawVerticalLine(int x)
{
  int i=0;
  for(i=0;i<7;i++)
  {
    tft.drawLine(x+i,20,x+i,220,BLACK);
  }
}

void resetGame()
{
  buttonEnabled = false;
  for(int i=0;i<9;i++)
  {
   board[i]=0;
  }
  moves = 1;
  winner = 0;
  gameScreen=2;
}
void initDisplay()
{
  tft.reset();
  tft.begin(0x9325);
  tft.setRotation(1);
}
void drawGameOverScreen()
{
   
  tft.fillScreen(WHITE);

  //Draw frame
  tft.drawRect(0,0,319,240,BLACK);

  //Print "Game Over" Text
  tft.setCursor(50,30);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.print("GAME OVER");
  delay(1000);
  initDisplay();
  resetFunc();
  

 if(winner == 0)
{
  //Print "DRAW!" text 
  tft.setCursor(110,100);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.print("Empataste");
}
 if(winner == 1)
{
  //Print "HUMAN WINS!" text 
  tft.setCursor(40,100);
  tft.setTextColor(BLUE);
  tft.setTextSize(1);
  tft.print("Ganaste");
}

 if(winner == 2)
{
  //Print "CPU WINS!" text 
  tft.setCursor(60,100);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.print("Entorno gana");
  delay(200);
  initDisplay();
  
}

   createPlayAgainButton();

   pinMode(XM, OUTPUT);
   pinMode(YP, OUTPUT);

   
}

 void createPlayAgainButton()
{
   //Create Red Button
  tft.fillRect(60,180, 200, 40, WHITE);
  tft.drawRect(60,180,200,40,BLACK);
  tft.setCursor(72,188);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.print("Juega otra vez");
  delay(2000);
  initDisplay();
}

void playGame()
{
  do
  {
    if(moves%2==1)
    {
     arduinoMove();
     printBoard();
     checkWinner();
    }else
    {
      playerMove(); 
      printBoard();
      checkWinner();  
    }
    moves++;
  }
  while (winner==0 && moves<10); 
  if(winner == 1)
  {
    Serial.println("Ganaste");
    delay(500);
    tft.fillScreen(WHITE);
    tft.drawRect(0,0,319,240,BLACK);
    tft.setCursor(50,30);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Ganaste");
    delay(1000);
    gameScreen=3;
    delay(1000);
    drawGameOverScreen();
  }else if(winner ==2)
  {
     Serial.println("Entorno gana");
     delay(500);
     tft.fillScreen(WHITE);
     tft.drawRect(0,0,319,240,BLACK);
     tft.setCursor(50,30);
     tft.setTextColor(BLACK);
     tft.setTextSize(3);
     tft.print("Entorno gana");
     delay(1000);   
     gameScreen=3;
     delay(1000);
     drawGameOverScreen();
  }else
  {
    Serial.println("Empate");
    delay(500);
    tft.fillScreen(WHITE);
    tft.drawRect(0,0,319,240,BLACK);
    tft.setCursor(50,30);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Empate");
    delay(1000);   
    gameScreen=3;
    delay(1000);
    drawGameOverScreen();
  }
  
}
void playGame2()
{
  do
  {
    if(moves%2==1)
    {
     playerMove2();
     printBoard();
     checkWinner();
    }else
    {
      playerMove(); 
      printBoard();
      checkWinner();
        
    }
    delay(500);
    moves++;
  }
  while (winner==0 && moves<11); 
  if(winner == 1)
  {
    Serial.println("Gana player 1");
    delay(500);
    tft.fillScreen(WHITE);
    tft.drawRect(0,0,319,240,BLACK);
    tft.setCursor(50,30);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Gana player 1");
    delay(1000);
    gameScreen=3;
    delay(1000);
    drawGameOverScreen();
  }else if(winner ==2)
  {
     Serial.println("Gana player 2");
     delay(500);
     tft.fillScreen(WHITE);
     tft.drawRect(0,0,319,240,BLACK);
     tft.setCursor(50,30);
     tft.setTextColor(BLACK);
     tft.setTextSize(3);
     tft.print("Gana player 2");
     delay(1000);
     gameScreen=3;
     delay(1000);
     drawGameOverScreen();
  }else
  {
    Serial.println("Empate");
    delay(500);
    tft.fillScreen(WHITE);
    tft.drawRect(0,0,319,240,BLACK);
    tft.setCursor(50,30);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("Empate");
    delay(1000);
    gameScreen=3;
    delay(1000);
    drawGameOverScreen();
  }
  
}
void playerMove()
{
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  TSPoint p;
  boolean validMove = false;
  Serial.print("\nTurno del jugador:");
  do
  {    
    p = ts.getPoint();  //Get touch point  
    if (p.z > ts.pressureThreshhold)
    {
      p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
      p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
      Serial.println(p.x);
      Serial.println(p.y);
  

      if((p.y<80)&& (p.x>195)) //6
      {
        if(board[6]==0)
        {
          Serial.println("Punto presionado: ");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[6]=1;
          drawPlayerMove(6);  
        }
      }
       else if((p.x>125 && p.x<=195)&& (p.y<80)) //3
      {
       
        if(board[3]==0)
        {
         Serial.println("Punto presionado: ");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[3]=1;
          drawPlayerMove(3);  
        }
      }
       else if((p.x<125)&& (p.y<80)) //0
      {
        if(board[0]==0)
        {
          Serial.println("Punto presionado: ");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[0]=1;
          drawPlayerMove(0);  
        }
      }

    else if((p.x>0 && p.x<115)&& (p.y<150 && p.y>80)) //1
      {
        if(board[1]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[1]=1;
          drawPlayerMove(1);  
        }
      }

       else if((p.x>195)&& (p.y<80)) //2
      {
        if(board[2]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[2]=1;
          drawPlayerMove(2);  
        }
      }

      else if((p.x>125 && p.x<=195)&& (p.y<150 && p.y>80)) //4
      {
        if(board[4]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[4]=1;
          drawPlayerMove(4);  
        }
      }

       else if((p.x>125 && p.x<=195)&& (p.y>150)) //5
      {
        if(board[5]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[5]=1;
          drawPlayerMove(5);  
        }
      }

        else if((p.x>195)&& (p.y<150 && p.y>80)) //7
      {
        if(board[7]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[7]=1;
          drawPlayerMove(7);  
        }
      }

       else if((p.x>195)&& (p.y>150)) //8
      {
        if(board[8]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[8]=1;
          drawPlayerMove(8);  
        }
      }
      
    }
   }while(p.z<ts.pressureThreshhold); 
}
void playerMove2()
{
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  TSPoint p;
  boolean validMove = false;
  Serial.print("\nTurno del jugador 2:");
  do
  {    
    p = ts.getPoint();  //Get touch point  
    if (p.z > ts.pressureThreshhold)
    {
      p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
      p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
      Serial.println(p.x);
      Serial.println(p.y);
  

      if((p.y<80)&& (p.x>195)) //6
      {
        if(board[6]==0)
        {
          Serial.println("Punto presionado: ");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[6]=2;
          drawCpuMove(6);  
        }
      }
       else if((p.x>125 && p.x<=195)&& (p.y<80)) //3
      {
       
        if(board[3]==0)
        {
         Serial.println("Punto presionado: ");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[3]=2;
          drawCpuMove(3);  
        }
      }
       else if((p.x<125)&& (p.y<80)) //0
      {
        if(board[0]==0)
        {
          Serial.println("Punto presionado: ");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[0]=2;
          drawCpuMove(0);  
        }
      }

    else if((p.x>0 && p.x<115)&& (p.y<150 && p.y>80)) //1
      {
        if(board[1]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[1]=2;
          drawCpuMove(1);  
        }
      }

       else if((p.y>150)&& (p.x<80)) //2
      {
        if(board[2]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[2]=2;
          drawCpuMove(2);  
        }
      }

      else if((p.x>125 && p.x<=195)&& (p.y<150 && p.y>80)) //4
      {
        if(board[4]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[4]=2;
          drawCpuMove(4);  
        }
      }

       else if((p.x>125 && p.x<=195)&& (p.y>150)) //5
      {
        if(board[5]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[5]=2;
          drawCpuMove(5);  
        }
      }

        else if((p.x>195)&& (p.y<150 && p.y>80)) //7
      {
        if(board[7]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[7]=2;
          drawCpuMove(7);  
        }
      }

       else if((p.x>195)&& (p.y>150)) //8
      {
        if(board[8]==0)
        {
          Serial.println("Punto presionado");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[8]=2;
          drawCpuMove(8);  
        }
      }
      
    }
   }while(p.z<ts.pressureThreshhold); 
}
void printBoard()
{
  int i=0;
  Serial.println("Board: [");
  for(i=0;i<9;i++)
  {
    Serial.print(board[i]);
    Serial.print(",");
  }
  Serial.print("]");
}

int checkOpponent()
{
  if(board[0]==1 && board[1]==1 && board[2]==0)
  return 2;
  else if(board[0]==1 && board[1]==0 && board[2]==1)
  return 1;
  else if (board[1]==1 && board [2]==1 && board[0]==0)
  return 0;
  else if (board[3]==1 && board[4]==1 && board[5]==0)
  return 5;
  else if (board[4]==1 && board[5]==1&& board[3]==0)
  return 3;
  else if (board[3]==1 && board[4]==0&& board[5]==1)
  return 4;
  else if (board[1]==0 && board[4]==1&& board[7]==1)
  return 1;
  else
  return 100;
}

void arduinoMove()
{
  int b = 0;
  int counter =0;
  int movesPlayed = 0;
  Serial.print("\nTurno entorno:");

  int firstMoves[]={0,2,6,8}; // will use these positions first

  for(counter=0;counter<4;counter++) //Count first moves played
  {
    if(board[firstMoves[counter]]!=0) // First move is played by someone
    {
      movesPlayed++;
    }
  }  
  do{
    if(moves<=2)
    {
      int randomMove =random(4); 
      int c=firstMoves[randomMove];
      
      if (board[c]==0)
      {  
        delay(1000);
        board[c]=2;
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b=1;
      }   
        
    }else
    {
    int nextMove = checkOpponent();
    if(nextMove == 100)
    {  
    if(movesPlayed == 4) //All first moves are played
    {
      int randomMove =random(9); 
      if (board[randomMove]==0)
      {  
        delay(1000);
        board[randomMove]=2;
        Serial.print(randomMove);
        Serial.println();
        drawCpuMove(randomMove);
        b=1;
      }   
    }else
    {
      int randomMove =random(4); 
      int c=firstMoves[randomMove];
      
      if (board[c]==0)
      {  
        delay(1000);
        board[c]=2;
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b=1;
      }   
  }
    }else
    {
       delay(500);
       board[nextMove]=2;
       drawCpuMove(nextMove);
       b=1;
    }
  }
  }
  while (b<1);
}

void drawCpuMove(int move)
{
  switch(move)
  {
    case 0: tft.setCursor(55,15);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O");break;
    case 1: tft.setCursor(130,15);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O"); break;
    case 2: tft.setCursor(205,15);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O"); break;
    case 3: tft.setCursor(55,85);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O");  break;
    case 4: tft.setCursor(130,85);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O"); break;
    case 5: tft.setCursor(205,85);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O"); break;
    case 6: tft.setCursor(55,155);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O"); break;
    case 7: tft.setCursor(130,155);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O"); break;
    case 8: tft.setCursor(205,155);
            tft.setTextColor(YELLOW);
            tft.setTextSize(9);
            tft.print("O"); break;
  }
}

void drawPlayerMove(int move)
{
  switch(move)
  {
    
    case 0: tft.setCursor(55,15);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X");break;
    case 1: tft.setCursor(130,15);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X"); break;
    case 2: tft.setCursor(205,15);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X"); break;
    case 3: tft.setCursor(55,85);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X");  break;
    case 4: tft.setCursor(130,85);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X"); break;
    case 5: tft.setCursor(205,85);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X"); break;
    case 6: tft.setCursor(55,155);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X"); break;
    case 7: tft.setCursor(130,155);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X"); break;
    case 8: tft.setCursor(205,155);
            tft.setTextColor(RED);
            tft.setTextSize(9);
            tft.print("X"); break; }
}

void checkWinner() 
// checks board to see if there is a winner
// places result in the global variable 'winner'
{
  int qq=0;
  // Circles win
  if (board[0]==1 && board[1]==1 && board[2]==1) { 
    winner=1; 
  }
  if (board[3]==1 && board[4]==1 && board[5]==1) { 
    winner=1; 
  }
  if (board[6]==1 && board[7]==1 && board[8]==1) { 
    winner=1; 
  }  
  if (board[0]==1 && board[3]==1 && board[6]==1) { 
    winner=1; 
  }
  if (board[1]==1 && board[4]==1 && board[7]==1) { 
    winner=1; 
  }
  if (board[2]==1 && board[5]==1 && board[8]==1) { 
    winner=1; 
  }  
  if (board[0]==1 && board[4]==1 && board[8]==1) { 
    winner=1; 
  }
  if (board[2]==1 && board[4]==1 && board[6]==1) { 
    winner=1; 
  }
  // crosses win?
  if (board[0]==2 && board[1]==2 && board[2]==2) { 
    winner=2; 
  }
  if (board[3]==2 && board[4]==2 && board[5]==2) { 
    winner=2; 
  }
  if (board[6]==2 && board[7]==2 && board[8]==2) { 
    winner=2; 
  }  
  if (board[0]==2 && board[3]==2 && board[6]==2) { 
    winner=2; 
  }
  if (board[1]==2 && board[4]==2 && board[7]==2) { 
    winner=2; 
  }
  if (board[2]==2 && board[5]==2 && board[8]==2) { 
    winner=2; 
  }  
  if (board[0]==2 && board[4]==2 && board[8]==2) { 
    winner=2; 
  }
  if (board[2]==2 && board[4]==2 && board[6]==2) { 
    winner=2; 
  }
 
}
