#include <UTFT.h>
#include <UTouch.h>
#include<TFT_ExtensionV2.h>
#include <TFTSprites.h>
#include "Utilities.h"

#define IsWithin(x,a,b) (x >= a && x <= b)

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);
Triangle Tup(&B), Tdown(&B), Tleft(&B), Tright(&B), TopL(&B), TopR(&B), BotL(&B), BotR(&B);
Circle Fire(&B);
TFTSprites TFTG(&myGLCD);

extern uint8_t BigFont[];

int PSize;
unsigned int PSx;
unsigned int PSy;

simpleTimer Fired;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK);

  PSize = 30;
  PSx = (myGLCD.getDisplayXSize() - (9 * PSize)) / 2;
  PSy = (myGLCD.getDisplayYSize() - (9 * PSize)) / 2;

  Tup.Coords(130, 160, 40, Tri_up);
  Tdown.Coords(130, 340, 40, Tri_down);
  Tleft.Coords(50, 250, 40, Tri_left);
  Tright.Coords(210, 250, 40, Tri_right);
  TopL.Coords(60, 170, 40, 50);
  TopR.Coords(200, 170, 40, -50);
  BotL.Coords(60, 330, 40, 130);
  BotR.Coords(200, 330, 40, -130);

  Tup.Colors(0xFFFF, NOFILL);
  Tdown.Colors(0xFFFF, NOFILL);
  Tleft.Colors(0xFFFF, NOFILL);
  Tright.Colors(0xFFFF, NOFILL);
  TopL.Colors(0xFF00, NOFILL);
  TopR.Colors(0x0FF0, NOFILL);
  BotL.Colors(0x00FF, NOFILL);
  BotR.Colors(0xF00F, NOFILL);

  Tup.Draw();
  Tdown.Draw();
  Tleft.Draw();
  Tright.Draw();

  Fire.Coords(130, 250, 40);
  Fire.Colors(GREEN, 0xFFFF, FILL);
  Fire.Text("FIRE", BLACK, Big);
  Fire.Draw();

  myGLCD.setColor(WHITE);
  myGLCD.drawRect(PSx, PSy, PSx + (9 * PSize), PSy + (9 * PSize));
}

void loop() {
  // put your main code here, to run repeatedly:
  static byte DP;
  DP = D_Pad();
  //myGLCD.printNumI(DP, 0, 0);
  movePlayer(DP);

}

byte D_Pad()
{
  byte Dir = 0;
  if (Tup.Touch())
    Dir = 1;
  if (Tdown.Touch())
    Dir = 2;
  if (Tleft.Touch())
    Dir = 3;
  if (Tright.Touch())
    Dir = 4;
  if (TopL.Touch())
    Dir = 5;
  if (TopR.Touch())
    Dir = 6;
  if (BotL.Touch())
    Dir = 7;
  if (BotR.Touch())
    Dir = 8;

  return Dir;
}

void movePlayer(byte Dir)
{
  static int Px = PSx + (9 * PSize) / 2, Py = PSx + (9 * PSize) / 4, LPx = 0, LPy = 0;
  static byte ShotLazer = false;
  switch (Dir)
  {
    case 1: Py -= 1; break;
    case 2: Py += 1; break;
    case 3: Px -= 1; break;
    case 4: Px += 1; break;
    case 5: Py -= 1; Px -= 1; break;
    case 6: Py -= 1; Px += 1; break;
    case 7: Px -= 1; Py += 1; break;
    case 8: Px += 1; Py += 1; break;
  }

  if (IsWithin(Px, PSx + 1, PSx + (9 * PSize) - 1) && IsWithin(Py, PSy + 1, PSy + (9 * PSize) - 1))
  {
    if (Px != LPx || Py != LPy)
    {
      myGLCD.setBackColor(TRANSPARENT);
      myGLCD.setColor(BLACK);
      myGLCD.printChar('#', LPx, LPy);
      myGLCD.setColor(WHITE);
      myGLCD.printChar('#', Px, Py);
      LPx = Px;
      LPy = Py;
    }
  }

  if (Fire.Touch())
  {
    myGLCD.setColor(RED);
    myGLCD.drawLine(PSx, PSy, Px + 8, Py + 8); //TL
    myGLCD.drawLine(PSx + (9 * PSize), PSy, Px + 8, Py + 8); //TR
    myGLCD.drawLine(PSx, PSy + (9 * PSize), Px + 8, Py + 8); //BL
    myGLCD.drawLine(PSx + (9 * PSize), PSy + (9 * PSize), Px + 8, Py + 8); //BR
    
    LPx = Px;
    LPy = Py;
    ShotLazer = true;
  }
  
  if(ShotLazer && Fired.Run(200))
  {
    myGLCD.setColor(BLACK);
    myGLCD.drawLine(PSx, PSy, LPx + 8, LPy + 8); //TL
    myGLCD.drawLine(PSx + (9 * PSize), PSy, LPx + 8, LPy + 8); //TR
    myGLCD.drawLine(PSx, PSy + (9 * PSize), LPx + 8, LPy + 8); //BL
    myGLCD.drawLine(PSx + (9 * PSize), PSy + (9 * PSize), LPx + 8, LPy + 8); //BR
   ShotLazer = false; 
  }

}
