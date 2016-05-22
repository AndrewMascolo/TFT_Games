#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <TFTSprites.h>
//http://cpp.sh/7qu33
#define IsWithin(x,a,b) (x >= a && x <= b)

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);
Circle Fire(&B);
Triangle Tup(&B), Tdown(&B), Tleft(&B), Tright(&B);
TFTSprites TFTG(&myGLCD);

extern uint8_t BigFont[];

int PSize;
unsigned int PSx;
unsigned int PSy;
struct Bullets
{
  byte live = 0;
  int X, Y;
  byte dx, dy;
};

Bullets bullet[30];
struct Tank
{
  int X;
  int Y;
  int color;
  byte life;
} P1;

byte complete = false;
enum {Road, Tree, Player, Truck, House, Bullet};

byte GameGrid[] =
{
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  1, 1, 0, 0, 4, 4, 0, 4, 4, 0, 1,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 0, 0, 0, 0, 0, 4, 3, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,

  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 3, 4, 0, 4, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 4, 0, 0, 0, 3, 0, 3, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,

  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

};

void setup()
{
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

  Tup.Colors(GREEN, 0xFFFF, NOFILL);
  Tdown.Colors(GREEN, 0xFFFF, NOFILL);
  Tleft.Colors(GREEN, 0xFFFF, NOFILL);
  Tright.Colors(GREEN, 0xFFFF, NOFILL);

  Tup.Draw();
  Tdown.Draw();
  Tleft.Draw();
  Tright.Draw();
  Fire.Coords(130, 250, 40);
  Fire.Colors(GREEN, 0xFFFF, FILL);
  Fire.Text("FIRE", BLACK, Big);
  Fire.Draw();
  myGLCD.setColor(WHITE);
  myGLCD.drawRect(PSx - 1, PSy - 1, PSx + (11 * PSize) + 1, PSy + (11 * PSize) + 1);
  P1.color = RED;
  for(byte i = 0; i < 30; i++)
    bullet[i].live = 0;
}

void loop()
{
  // put your main code here, to run repeatedly:
  static int DP, LDP, counter = 0, f = -1;
  static int px = 0, py = 0, bullets;
  static unsigned long lastM = micros();
  D_Pad(&DP, &LDP);
  if (Fire.Delay(500))
    f = addBullet(P1.X, P1.Y, LDP);
  movePlayer(&px, &py, DP);
  drawGrid(px, py, LDP);

  if (((micros() - lastM) >= 100000))
  {
    counter++;
    if (counter > 30)
      counter = 0;
    fireBullet(f, counter);
    lastM += 100000;
    myGLCD.setColor(WHITE);
    myGLCD.printNumI(counter, 100, 0);
  }
  myGLCD.setColor(WHITE);
  myGLCD.printNumI(f, 150, 0);
}

void D_Pad(int *Dir, int* Ldp)
{
  *Dir = 0;
  byte _up, _down, _left, _right, tog = true;
  static byte _lup, _ldown, _lleft, _lright;
  if ((_up = Tup.Delay(100)) != _lup)
  {
    if (_up)
      *Dir = 1;
    _lup = _up;
  }
  if ((_down = Tdown.Delay(100)) != _ldown)
  {
    if (_down)
      *Dir = 2;
    _ldown = _down;
  }
  if ((_left = Tleft.Delay(100)) != _lleft)
  {
    if (_left)
      *Dir = 3;
    _lleft = _left;
  }
  if ((_right = Tright.Delay(100)) != _lright)
  {
    if (_right)
      *Dir = 4;
    _lright = _right;
  }

  if (*Dir)
    *Ldp = *Dir;
}

byte drawGrid(int Px, int Py, int dir)
{
  int _move = Px + Py * 33;
  static int col = 0, location = 0, lastDir = 1, X, Y;

  X = (PSx + (location % 11) * PSize);
  Y = PSy + (location / 11) * PSize;
  if (GameGrid[col + _move] == 1)
    TFTG.drawSprite_16(Pine_tree, X, Y, 1, Upwards, 0, GREEN, 0);
  if (GameGrid[col + _move] == 2)
    TFTG.drawSprite_16(Abv_tank, X, Y, 1, dir - 1, 0, P1.color, 0);
  if (GameGrid[col + _move] == 3)
    TFTG.drawSprite_16(Side_truck, X, Y, 1, Upwards, 0, BLUE, 0);
  if (GameGrid[col + _move] == 4)
    TFTG.drawSprite_16(Lg_house, X, Y, 1, Upwards, 0, WHITE, 0);
  if (GameGrid[col + _move] == 5)
  {
    myGLCD.setColor(WHITE);
    myGLCD.fillCircle(X + 8, Y + 8, 4);
    //GameGrid[col + _move] = 0;
  }
  if (GameGrid[col + _move] == 0)
    myGLCD.printChar(' ', X, Y);
  //delay(500);
  if (location < 120)
  {
    location++;
    col++;
    if ((col > 10) && ((col % 11) == 0))
      col += 22;
    if (col >= 340) // 340
      col = 0;
    complete = false;
  }
  else
  {
    location = 0;
    complete = true;
  }

  return complete;
}

void movePlayer(int *x, int *y, byte Dir)
{
  static int Px = 5, Py = 5 , LPx = 5, LPy = 5;

  // Move player and check for an obstruction
  switch (Dir)
  {
    case 1: if (GameGrid[Px + (Py - 1) * 33] != 1)Py -= 1; break;
    case 2: if (GameGrid[Px + (Py + 1) * 33] != 1)Py += 1; break;
    case 3: if (GameGrid[(Px - 1) + Py * 33] != 1)Px -= 1; break;
    case 4: if (GameGrid[(Px + 1) + Py * 33] != 1)Px += 1; break;
  }
  if (Py < 1)  Py = 1;
  if (Py > 33) Py = 32;
  if (Px < 1)  Px = 1;
  if (Px > 33) Px = 32;

  if (Dir)
  {
    if (IsWithin(Px, 0, 33) && IsWithin(Py, 0, 33))
    {
      if (Px != LPx || Py != LPy)
      {
        P1.X = Px;
        P1.Y = Py;

        *x = Px - 5;
        if (*x < 0) *x = 0;
        if (*x > 22) *x = 22;

        *y = Py - 5;
        if (*y < 0) *y = 0;
        if (*y > 22) *y = 22;

        GameGrid[LPx + LPy * 33] = 0;
        GameGrid[P1.X + P1.Y * 33] = 2;
        LPx = Px;
        LPy = Py;
      }
    }
  }
}

int addBullet(int x, int y, byte Dir)
{
  int found = -1;
  for (byte i = 0; i < 30; i++)
  {
    if (bullet[i].live == NULL)
    {
      bullet[i].live = 1;
      found = i;
      break;
    }
  }

  if (found >= 0)
  {
    int i = found;
    //bullet[i] = malloc(sizeof(Bullets));
    bullet[i].X = x;
    bullet[i].Y = y;
    switch (Dir)
    {
      case 1: bullet[i].dy = -1; break;
      case 2: bullet[i].dy = 1; break;
      case 3: bullet[i].dx = -1; break;
      case 4: bullet[i].dx = 1; break;
    }
  }
  return found;
}

void fireBullet(int found, byte i)
{
  if (i < found)
  {
    if (bullet[i].live)
    {
      bullet[i].X += bullet[i].dx;
      bullet[i].Y += bullet[i].dy;
      if ( ((bullet[i].X <= 0) || (bullet[i].X >= 11)) /*||  ((bullet[i].Y < 0) || (bullet[i].Y > 11)) */)
        cleanBullets(i);

      myGLCD.setColor(WHITE);
      myGLCD.printNumI(bullet[i].X, 0, 0);
      myGLCD.printNumI(bullet[i].Y, 50, 0);
      GameGrid[bullet[i].X + bullet[i].Y * 33] = Bullet;
    }
  }
}

void cleanBullets(byte i)
{
  if (bullet[i].live)
  {
    bullet[i].live = NULL;
    GameGrid[bullet[i].X + bullet[i].Y * 33] = 0;
  }
}


