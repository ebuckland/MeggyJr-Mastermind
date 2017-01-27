/*
 * MasterMind for the Meggy Jr - Eric Buckland
 * 
 * Introduction:
 * This is Mastermind, a logical singleplayer game where you try and guess a key generated 
 * by the computer, with clues. Using educated guesses over time, you need to work out the
 * color code in 7 moves. If you guess the code correctly, you win. If you don't, you lose.
 * Have fun.
 * 
 * Guess pegs (on the left):
 * These are the guesses you have submitted. The pegs shift up when you submit, so the oldest
 * guess is on the top. You can use the history to logically deduce what you should guess
 * to solve the code.
 * 
 * Clue pegs (to the right):
 * A purple peg tells you that there is a peg that is the correct color in the right place.
 * A white peg tells you that you have the correct color in the wrong place.
 * 
 * Controls:
 * Right and left arrowkeys to move between the pegs in the guess.
 * Up and down arrowkeys to change color. There are 6 colors.
 * A to submit guess.
 * 
 */

#include <MeggyJrSimple.h>

byte triesLeft = 6;

byte keyPegs[4];
byte boardPegs[8][8];
byte currentPeg = 0;
unsigned long blinkMillis;
bool playTone = true;
bool randomSeeded = false;

void setup() 
{
  // setup the meggy and play tune
  Serial.begin(9600);
  MeggyJrSimpleSetup();
  if (playTone)
    Tone_Start(12282,100);
  delay(100);
  if (playTone)
    Tone_Start(9282,200);
  
  
  // display auxiliary leds animation
  displayAuxAnimation();

  blinkMillis = millis();
}

void loop() {
  // check for user input, and refresh the screen
  CheckButtonsPress();
  if (Button_Up) {
      if (boardPegs[currentPeg][0] >= 6) 
      {
        boardPegs[currentPeg][0] = 1;
      } 
      else 
      {
        boardPegs[currentPeg][0] ++;
      }
      blinkMillis = millis();
  } else if (Button_Right) 
  {
      currentPeg ++;
      if (currentPeg == 4) 
      {
        currentPeg = 0;
      }
  } else if (Button_Down) 
  {
      if (boardPegs[currentPeg][0] <= 1) 
      {
        boardPegs[currentPeg][0] = 6;
      } 
      else 
      {
        boardPegs[currentPeg][0] --;
      }
      blinkMillis = millis();
  } else if (Button_Left) 
  {
      if (currentPeg == 0) 
      {
        currentPeg = 3;
      } 
      else 
      {
        currentPeg --;
      }
  } else if (Button_A) 
  {
    if (!randomSeeded) 
    {
      createKey();
      randomSeeded = true;
    }
    if (noBlanks()) 
    {
      submitPegs();
    }
  }

  ClearSlate();
  
  drawBoard();

  DisplaySlate();

  delay(100);
}

void createKey() 
{
  // create the key when the user submits his first guess
  randomSeed(millis());
  for (int i = 0; i < 4; i ++)
  {
    keyPegs[i] = random(1,7);
    Serial.print(keyPegs[i]); Serial.print(", "); 
  }
}


bool noBlanks() {
  // check if the initial user input has blanks
  for (int i = 0; i < 4; i++) 
  {
    if (boardPegs[i][0] == 0) 
    {
      if (playTone)
        Tone_Start(16282,100);
      return false;
    }
  }
  return true;
}

void submitPegs() 
{
  
  // shift all the player guesses
  for (int i = 0; i < 8; i++) 
  {
    for (int j = 7; j > 0; j--) 
    {
      boardPegs[i][j] = boardPegs[i][j-1];
    }
  }

  byte colorCountGuess[7];
  byte colorCountAns[7];

  // find the different colors on each group

  for (int i = 0; i < 7; i ++) 
  {
    colorCountGuess[i] = 0;
    colorCountAns[i] = 0;
  }
  
  for (int i = 0; i < 4; i ++) 
  {
     colorCountGuess[boardPegs[i][1]]++;
  }
  
  for (int i = 0; i < 4; i ++) 
  {
     colorCountAns[keyPegs[i]]++;
  }

  // find the black pegs
  
  byte blackPegs = 0;
  for (int i = 0; i < 4; i++) {
    if (keyPegs[i] == boardPegs[i][1]) 
    {
      blackPegs++;
    }
  }

  // find the number of white pegs
  
  byte whitePegs = 0;
  for (int i = 1; i < 7; i++) 
  {
    while (colorCountGuess[i] > 0 && colorCountAns[i] > 0) 
    {
      whitePegs++;
      colorCountGuess[i] --;
      colorCountAns[i] --;
    }
  }
  
  // remove the number of black pegs from the number of white pegs

  whitePegs = whitePegs - blackPegs;

  // draw in the result pegs

  int pegPosition = 4;
  
  for (int i = 0; i < blackPegs; i++) 
  {
    boardPegs[pegPosition][1] = 14;
    pegPosition++;
  }
  for (int i = 0; i < whitePegs; i++) 
  {
    boardPegs[pegPosition][1] = 7;
    pegPosition++;
  }

  // check if won or lost, then stop the program

  if (blackPegs == 4) 
  {
    wonAnimation();
    // do with 11, dimGreen
    delay (4000000);
  }

  // if the tries is 0, then test if won, if not then game over

  if (triesLeft == 0) 
  {
    lostAnimation();
    // do with 8, dimRed
    delay (4000000);
  }
  
  triesLeft--;
}

void wonAnimation() {
  
  // shows a blinking check mark
  for (int i = 0; i < 3; i++) 
  {
    if (i == 0 && playTone)
        Tone_Start(12282,200);
    DrawPx(1,4,11);
    DrawPx(2,3,11);
    DrawPx(3,2,11);
    DrawPx(4,3,11);
    DrawPx(5,4,11);
    DrawPx(6,5,11);
    DisplaySlate();
    delay(300);
    if (i == 0 && playTone)
        Tone_Start(9282,300);
    delay(200);
    blinkMillis = millis();
    drawBoard();
    DisplaySlate();
    delay(600);
  }
}

void lostAnimation() {

  // draws in the correct pegs at the bottom
  for (int i = 0; i < 4; i ++) 
  {
    boardPegs[i][0] = keyPegs[i];
  }
  drawBoard();
  DisplaySlate();

  // show a blinking x mark
  for (int i = 0; i < 3; i++) 
  {
    if (i == 0 && playTone)
      Tone_Start(9282,300);
    DrawPx(1,1,8);
    DrawPx(2,2,8);
    DrawPx(3,3,8);
    DrawPx(4,4,8);
    DrawPx(5,5,8);
    DrawPx(6,6,8);
    DrawPx(1,6,8);
    DrawPx(2,5,8);
    DrawPx(3,4,8);
    DrawPx(4,3,8);
    DrawPx(5,2,8);
    DrawPx(6,1,8);
    DisplaySlate();
    delay(300);
    if (i == 0 && playTone)
      Tone_Start(12282,200);
    delay(200);
    blinkMillis = millis();
    drawBoard();
    DisplaySlate();
    delay(600);
  }
}

void drawBoard() {
  // draw all the pegs on the board, with a blinking selected tile
  for (int i = 0; i < 8; i++) 
  {
    for (int j = 0; j < 8; j++) 
    {
      DrawPx(i,j,boardPegs[i][j]);
      // a timer for blinking lights
      if ((j == 0) && (i == currentPeg)) 
      {
        if (millis() - blinkMillis <= 600) 
        {
          if (boardPegs[i][j] == 0) 
          {
            DrawPx(i,j,White);
          }
        } else if (millis() - blinkMillis <= 1200) 
        {
          DrawPx(i, j, 0);
        } else if (millis() - blinkMillis > 1200) 
        {
            blinkMillis = millis();
        }
      }
    }
  }
}

void displayAuxAnimation() {
  int displayNum[] = {1,3,6,12,24,48,96,192,128};
  for (int i = 0; i < 2; i ++) {
    for (int pos = 0; pos < 9; pos++) 
    { 
      SetAuxLEDs(displayNum[pos]);
      delay(40);
    }
    for (int pos = 8; pos > -1; pos--) 
    { 
      SetAuxLEDs(displayNum[pos]);
      delay(40);
    }
  }
  SetAuxLEDs(0);
}

