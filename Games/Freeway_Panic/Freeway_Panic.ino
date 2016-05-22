#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <TFTSprites.h>

//7.0 or 3.2
#define ScreenSize 7.0 
#define IsWithin(x,a,b) (x >= a && x <= b)

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
//UTFT    myGLCD(ITDB32S, 38, 39, 40, 41);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);
Triangle /*Tup(&B), Tdown(&B),*/ Tleft(&B), Tright(&B);
Box Mode(&B), Pause(&B), Panic(&B);

TFTSprites TFTG(&myGLCD);

extern uint8_t BigFont[];

enum {Road, Wall, Player, Obstruct, Crash};
byte Grid[11][6] =
{
  {1, 0, 0, 0, 0, 1}, // loading row
  {1, 0, 0, 0, 0, 1}, // 0
  {1, 0, 0, 0, 0, 1}, // 1
  {1, 0, 0, 0, 0, 1}, // 2
  {1, 0, 0, 0, 0, 1}, // 3
  {1, 0, 0, 0, 0, 1}, // 4
  {1, 0, 0, 0, 0, 1}, // 5
  {1, 0, 0, 0, 0, 1}, // 6
  {1, 0, 0, 0, 0, 1}, // 7
  {1, 0, 0, 0, 0, 1}, // 8
  {1, 0, 0, 2, 0, 1}  // 9
};

int PSize;
unsigned int PSx;
unsigned int PSy;

void setup()
{
  // put your setup code here, to run once:
  //Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision((ScreenSize == 7.0?PREC_MEDIUM: PREC_LOW));
  myGLCD.fillScr(BLACK);

  PSize = (ScreenSize == 7.0? 30 : 18);
  int DispX = myGLCD.getDisplayXSize();
  int DispY = myGLCD.getDisplayYSize();
  PSx = (DispX - (6 * PSize)) / 2;
  PSy = (DispY - (10 * PSize)) / 2;

  Tleft.Coords(DispX / (ScreenSize == 7.0? 8:5), (ScreenSize == 7.0?250:120), (ScreenSize == 7.0?70:50), Tri_left);
  Tright.Coords(DispX - (DispX / (ScreenSize == 7.0? 8:5)), (ScreenSize == 7.0?250:120), (ScreenSize == 7.0?70:50), Tri_right);

  Tleft.Colors(0xFFFF, FILL);
  Tright.Colors(0xFFFF, FILL);

  Tleft.Draw();
  Tright.Draw();
  //-----------------------------------------------------
  Mode.Coords((ScreenSize == 7.0? 40:1), (ScreenSize == 7.0? 100:5), AUTO, AUTO);
  Pause.Coords((ScreenSize == 7.0? 170:215), (ScreenSize == 7.0? 100:5), AUTO, AUTO);
  Panic.Coords((ScreenSize == 7.0? 40:1), (ScreenSize == 7.0? 100:5), AUTO, AUTO);

  Mode.Colors(RED, GREEN, ROUNDED, NOFILL);
  Pause.Colors(RED, GREEN, ROUNDED, NOFILL);
  Panic.Colors(RED, GREEN, ROUNDED, NOFILL);

  Mode.Text(" Hard ", " Easy ", WHITE, Big);
  Pause.Text("Pause", "Start", WHITE, Big);
  Panic.Text("PANIC!", "", ORANGE, Big);

  Mode.Draw();
  Pause.Draw();
  //-----------------------------------------------------
  drawGrid();
}

void loop()
{
  // put your main code here, to run repeatedly:
  static byte Y = 0, Pos, diff, speedInc, Level = 1, diffTog, panic = false;
  static unsigned long prevTime = millis(), Interval = 800, timer, Score = 0;

  diffTog = Mode.Toggle();

  if (diffTog & byte(Panic.Delay(1000)))
    panic = true;
  if (diffTog == false)
    panic = false;

  diff = diffTog ? (panic ? 1 : 2) : 3;
  speedInc = diffTog ? (panic ? 300: 200) : 0;

  //Serial.println(panic);

  if (Pause.Toggle())
  {
    byte DP = D_Pad();
    timer = millis() - prevTime;
    Pos = movePlayer(DP);

    if (timer >= (Interval - speedInc))
    {
      if ( (Y % diff) == 0)
        CreateBarrier(panic);

      moveGrid();
      Y++;

      prevTime += (Interval - speedInc);
      makeScore(Score);
      if (detectObstruction(Pos))
      {
        drawGrid();
        delay(1000);
        cleanroad();
        Score = 0;
        Level = 1;
        Interval = 800;
      }
      else
        Score += 10;

      if (Score > 0 && (Score % 500) == 0)
      {
        Interval -= 50;
        Level++;
      }

      if (Interval < 400) Interval = 400;
    }
    drawGrid();
    showLevel(Level, diff);
  }
  else prevTime = millis();
  //debug();

}

byte D_Pad()
{
  byte Dir = 0;
  if (Tleft.Touch())
    Dir = 3;
  if (Tright.Touch())
    Dir = 4;

  return Dir;
}

void drawGrid()
{
  myGLCD.setFont(BigFont);
  static char sprite;

  for (int row = 1; row < 11; row++)
    for (int col = 0; col < 6; col++)
    {
      if (Grid[row][col] == Wall)
        sprite = '|';
      else if (Grid[row][col] == Player)
        sprite = '^';
      else if (Grid[row][col] == Obstruct)
        sprite = 'V';
      else if (Grid[row][col] == Crash)
        sprite = 'X';
      else // Road
        sprite = ' ';

      myGLCD.setColor((sprite == 'V') ? RED : WHITE);
      if (sprite == 'V')
        TFTG.drawSprite_16(Abv_truck, PSx + col * PSize, PSy + (row - 1) * PSize, 1, Downwards);
      else if (sprite == '^')
        TFTG.drawSprite_16(Abv_racecar, PSx + col * PSize, PSy + (row - 1) * PSize, 1, Upwards);
      else
        myGLCD.printChar(sprite, PSx + col * PSize, PSy + (row - 1) * PSize);
    }
}
void debug()
{
  for (int row = 1; row < 11; row++)
    for (int col = 0; col < 6; col++)
      myGLCD.printNumI(Grid[row][col], 250 + PSx + col * PSize, PSy + (row - 1) * PSize);
}

byte movePlayer(byte Dir)
{
  static int Px = 3, Py = 10, LPx = 3, LPy = 10;
  switch (Dir)
  {
    case 3: Px -= 1; break;
    case 4: Px += 1; break;
  }

  if (Px < 1) Px = 1;
  if (Px > 4) Px = 4;

  if (IsWithin(Px, 0, 5))
  {
    Grid[LPy][LPx] = Road;
    Grid[Py][Px] = Player;
    LPx = Px;
    LPy = Py;
  }
  return Px;
}

void moveGrid()
{
  for (int y = 8; y > -1; y--)
    for (byte x = 1; x < 5; x++)
    {
      Grid[y + 1][x] = Grid[y][x];
      if ( y == 0 )Grid[y][x] = Road;
    }
}

void CreateBarrier(byte P)
{
  for (byte C = 0, rand; C < (P?2:3); C++)
  {
    rand = byte(random(1, 5));
    Grid[0x0][rand] = Obstruct;
  }
}

byte detectObstruction(byte pos)
{
  for (byte x = 1; x < 5; x++)
  {
    if (Grid[9][pos] == Obstruct)
    {
      Grid[9][pos] = Crash;
      Grid[10][pos] = Crash;
      return 1;
    }
  }
  return 0;
}

void cleanroad()
{
  for (int y = 10; y > -1; y--)
    for (byte x = 1; x < 5; x++)
      Grid[y][x] = Road;
}

void makeScore(long S)
{
  myGLCD.printNumI(S, PSx + (ScreenSize == 7.0? 40:12), PSy - PSize, 5, '0');
}

void showLevel(byte L, byte D)
{
  char buf[11] = "Level:E   ";
  if(D == 1)   
    buf[6] = 'P';
  else if(D == 2)
    buf[6] = 'H';
  else
    buf[6] = 'E';
    
  buf[8] = (L/10) + '0';
  buf[9] = (L%10) + '0';
  myGLCD.print(buf, PSx + 5, PSy + PSize * 10);
}

