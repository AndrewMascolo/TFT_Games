#include <UTFT.h>
#include <UTouch.h>
#include<TFT_ExtensionV2.h>
#include <TFTGaming.h>

#define IsWithin(x,a,b) (x >= a && x <= b)

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);
Triangle Tup(&B), Tdown(&B), Tleft(&B), Tright(&B);
TFTGaming TFTG(&myGLCD);

extern uint8_t BigFont[];

byte Grid[10][10] = {
  {1, 1, 1, 0, 0, 0, 0, 1, 1, 1}, // 0
  {1, 1, 0, 0, 0, 0, 0, 0, 1, 1}, // 1
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 2
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 3
  {1, 1, 1, 0, 0, 0, 0, 1, 1, 1}, // 4
  {1, 1, 1, 0, 0, 0, 0, 1, 1, 1}, // 5
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 6
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // 7
  {1, 1, 0, 0, 0, 0, 0, 0, 1, 1}, // 8
  {1, 1, 1, 0, 0, 0, 0, 1, 1, 1} // 9
};

int PSize;
unsigned int PSx;
unsigned int PSy;

enum {Road, Wall, Player, Obstruct, Crash};

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

  Tup.Coords(130, 160, 40, up);
  Tdown.Coords(130, 340, 40, down);
  Tleft.Coords(50, 250, 40, left);
  Tright.Coords(210, 250, 40, right);

  Tup.Colors(0xFFFF, NOFILL);
  Tdown.Colors(0xFFFF, NOFILL);
  Tleft.Colors(0xFFFF, NOFILL);
  Tright.Colors(0xFFFF, NOFILL);

  Tup.Text("U", 0xFFFF, Big);
  Tdown.Text("D", 0xFFFF, Big);
  Tleft.Text("L", 0xFFFF, Big);
  Tright.Text("R", 0xFFFF, Big);

  Tup.Draw();
  Tdown.Draw();
  Tleft.Draw();
  Tright.Draw();

  drawGrid();
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

  return Dir;
}

void drawGrid()
{
  myGLCD.setFont(BigFont);
  for (int row = 0; row < 10; row++)
    for (int col = 0; col < 10; col++)
    {
      if (Grid[row][col] == 1)
        TFTG.drawSprite_16(Lg_house, PSx + col * PSize, PSy + row * PSize, 1, Upwards);
      else
        myGLCD.printChar(' ', PSx + col * PSize, PSy + row * PSize);
    }
}

void showPlayer(byte dir)
{
  myGLCD.setFont(BigFont);
  for (int row = 0; row < 10; row++)
    for (int col = 0; col < 10; col++)
    {
      if (Grid[row][col] == 1)
        continue;
      else if (Grid[row][col] == 2)
      {
        if (dir != 0)
          TFTG.drawSprite_16(Abv_tank, PSx + col * PSize, PSy + row * PSize, 1, dir - 1);
      }
      else
        myGLCD.printChar(' ', PSx + col * PSize, PSy + row * PSize);
    }
}

void movePlayer(byte Dir)
{
  static int Px = 5, Py = 5, LPx = 5, LPy = 5;
  switch (Dir)
  {
    case 1: Py -= 1; break;
    case 2: Py += 1; break;
    case 3: Px -= 1; break;
    case 4: Px += 1; break;
  }

  if (Py < 0) Py = 0;
  if (Px < 0) Px = 0;
  if (Py > 9) Py = 9;
  if (Px > 9) Px = 9;

  if (IsWithin(Px, 0, 9) && IsWithin(Py, 0, 9))
  {
    if (Px != LPx || Py != LPy)
    {
      //myGLCD.printChar(' ', LPx, LPy);
      //myGLCD.printChar('0', Px, Py);
      Grid[LPy][LPx] = 0;
      Grid[Py][Px] = 2;
      LPx = Px;
      LPy = Py;
    }
  }
  showPlayer(Dir);
}
