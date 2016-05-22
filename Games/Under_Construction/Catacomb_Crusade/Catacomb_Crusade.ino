#include <UTFT.h>
#include <UTouch.h>
#include<TFT_ExtensionV2.h>
#include <TFTSprites.h>

#define IsWithin(x,a,b) (x >= a && x <= b)

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);
Triangle Tup(&B), Tdown(&B), Tleft(&B), Tright(&B);
TFTSprites TFTG(&myGLCD);

extern uint8_t BigFont[];

int PSize;
unsigned int PSx;
unsigned int PSy;

char lastSide = 127, lastScreen = 127;

enum GameObjects
{
  RR,
  WW,
  PL,
  HH,
  TR
};

byte Map[3][3][144] =
{
  //Left side grid
  {
    {
      WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,    //  X   |    |
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,    //      |    |
      WW, RR, RR, RR, RR, TR, RR, RR, RR, RR, RR, WW,    //      |    |
      WW, RR, RR, RR, RR, TR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, TR, RR, RR, RR, RR, RR, RR,
      WW, RR, RR, RR, RR, TR, RR, RR, RR, RR, RR, RR,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, WW, WW, WW, WW, RR, WW, WW, WW, WW, WW, WW
    },
    {
      WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, TR, TR, RR, RR, RR, RR, WW,
      RR, RR, RR, RR, TR, RR, RR, TR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, TR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, TR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, TR, TR, TR, TR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, WW,
      WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW, WW
    },
    {
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR
    }
  },

  { // middle Grid
    {
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, WW, RR, WW, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR
    },
    {
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR
    },
    {
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR
    }
  },

  { // Right side Grid
    {
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR
    },
    {
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR
    },
    {
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR,
      RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR, RR
    }
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_MEDIUM);
  myGLCD.fillScr(BLACK);

  PSize = 20;
  PSx = (myGLCD.getDisplayXSize() - (9 * PSize)) / 2;
  PSy = (myGLCD.getDisplayYSize() - (9 * PSize)) / 2;

  Tup.Coords(130, 160, 40, Tri_up);
  Tdown.Coords(130, 340, 40, Tri_down);
  Tleft.Coords(50, 250, 40, Tri_left);
  Tright.Coords(210, 250, 40, Tri_right);

  Tup.Colors(0xFFFF, NOFILL);
  Tdown.Colors(0xFFFF, NOFILL);
  Tleft.Colors(0xFFFF, NOFILL);
  Tright.Colors(0xFFFF, NOFILL);

  Tup.Draw();
  Tdown.Draw();
  Tleft.Draw();
  Tright.Draw();
  drawGrid(0, 0);
}

void loop()
{
  // put your main code here, to run repeatedly:
  static int Side = 0, Screen = 0;
  static byte DP;
  DP = D_Pad();
  movePlayer(&Side, &Screen, DP);

  myGLCD.printNumI(Side, 20, 20);
  myGLCD.printNumI(Screen, 60, 20);

  if (Side != lastSide || Screen != lastScreen)
  {
    drawGrid(Side, Screen);
    lastSide = Side;
    lastScreen = Screen;
  }
  showPlayer(Side, Screen, DP);
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

void drawGrid(int side, int screen)
{
  myGLCD.setFont(BigFont);
  for (int col = 0; col < 144; col++)
  {
    if (Map[side][screen][col] == WW)
      TFTG.drawSprite_16(Pine_tree, PSx + (col % 12) * PSize, PSy + (col / 12) * PSize, 1, Upwards);
    else if (Map[side][screen][col] == TR)
      TFTG.drawSprite_16(Side_truck, PSx + (col % 12) * PSize, PSy + (col / 12) * PSize, 1, Upwards);
    else
      myGLCD.printChar(' ', PSx + (col % 12) * PSize, PSy + (col / 12) * PSize);
  }
}

void showPlayer(int side, int screen, byte dir)
{
  myGLCD.setFont(BigFont);
  for (int col = 0; col < 144; col++)
  {
    if (Map[side][screen][col] == WW)
      continue;
    else if (Map[side][screen][col] == TR)
      continue;
    else if (Map[side][screen][col] == PL)
    {
      if (dir != 0)
        TFTG.drawSprite_16(Abv_tank, PSx + (col % 12) * PSize, PSy + (col / 12) * PSize, 1, dir - 1);
    }
    else
      myGLCD.printChar(' ', PSx + (col % 12) * PSize, PSy + (col / 12) * PSize);
  }
}

void movePlayer(int *side, int *screen, byte Dir)
{
  static int Px = 5, Py = 2 * 12, LPx = 5, LPy = 5, lSide = 0, lScreen = 0;

  switch (Dir)
  {
    case 1:
      if (Map[*side][*screen][Px + (Py - 12)] != WW)
        Py -= 12;
      if (Py < 0)
      {
        *side -= 1;
        *side = (((*side - 1) < 0) ? 0 : *side);
        Py = 132;
      }
      break;

    case 2:
      if (Map[*side][*screen][Px + (Py + 12)] != WW)
        Py += 12;
      if (Py >= 144)
      {
        *side += 1;
        *side = (((*side + 1) > 2) ? 2 : *side);
        Py = 0;
      }
      break;

    case 3:
      if (Map[*side][*screen][(Px - (Px > 0)) + Py] != WW)
        Px -= 1;
      if (Px < 0)
      {
        *screen -= 1;
        *screen = (((*screen - 1) < 0) ? 0 : *screen);
        Px = 11;
      }
      break;

    case 4:
      if (Map[*side][*screen][(Px + (Px < 11)) + Py] != WW)
        Px += 1;
      if (Px > 10)
      {
        *screen += 1;
        *screen = (((*screen + 1) > 2) ? 2 : *screen);
        Px = 0;
      }
      break;
  }
  myGLCD.printNumI(Px, 100, 0);

  if (IsWithin(Px, 0, 11) && IsWithin(Py, 0, 144))
  {
    if (Px != LPx || Py != LPy)
    {
      if ((lScreen == *screen) || (lSide == *side))
        Map[*side][*screen][LPx + LPy] = RR;
      else
      {
        Map[lSide][lScreen][LPx + LPy] = RR;
        //drawGrid(lSide, lScreen);
      }
      Map[*side][*screen][Px + Py] = PL;
      lSide = *side;
      lScreen = *screen;
      LPx = Px;
      LPy = Py;
    }
  }
}

