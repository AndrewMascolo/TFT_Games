#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <TFTSprites.h>
#include "PlayerClass.h"
#include "BulletClass.h"

//http://cpp.sh/7qu33

UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);
Base B(&myGLCD, &myTouch);
Circle Fire(&B);
Triangle Tup(&B), Tdown(&B), Tleft(&B), Tright(&B);
TFTSprites TFTG(&myGLCD);


simpleTimer
MovePlayerTimer,
MoveEnemyTimer,
MoveBulletTimer,
CleanCombatant;

Field BattleField;
Bullet HeroBullets(20, &TFTG);
Bullet EnemyBullets(10, &TFTG);
Player Enemy(10, &TFTG);
Player Hero(1, &TFTG);

extern uint8_t BigFont[];
extern uint8_t SpriteSheet[];

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_LOW);
  myGLCD.fillScr(BLACK);
  
  Tup.Coords(130, 160, 40, Tri_up);
  Tdown.Coords(130, 340, 40, Tri_down);
  Tleft.Coords(50, 250, 40, Tri_left);
  Tright.Coords(210, 250, 40, Tri_right);

  Tup.Colors(GREEN, 0xFF00, NOFILL);
  Tdown.Colors(GREEN, 0x0FF0, NOFILL);
  Tleft.Colors(GREEN, 0x00FF, NOFILL);
  Tright.Colors(GREEN, 0xF00F, NOFILL);

  Tup.Draw();
  Tdown.Draw();
  Tleft.Draw();
  Tright.Draw();

  Fire.Coords(130, 250, 40);
  Fire.Colors(GREEN, 0xFFFF, FILL);
  Fire.Text("FIRE", BLACK, Big);
  Fire.Draw();

  BField.Mult = 25;
  BField.X = ((myGLCD.getDisplayXSize() - (9 * BField.Mult)) / 2);
  BField.Y = ((myGLCD.getDisplayYSize() - (9 * BField.Mult)) / 2) - 60;
  BField.X2 = BField.X + 16 * BField.Mult;
  BField.Y2 = BField.Y + 16 * BField.Mult;
  BField.Color = DARK_GREEN;

  myGLCD.setColor(WHITE);
  myGLCD.drawRect(BField.X + 1, BField.Y + 1, BField.X2 + 1, BField.Y2 + 1);
  myGLCD.setColor(BField.Color);
  myGLCD.fillRect(BField.X, BField.Y, BField.X2, BField.Y2);

  HeroBullets.SetBattleField(BField);
  EnemyBullets.SetBattleField(BField);
  Hero.CreateOpponent();
  Hero.SetBattleField(BField);
  Hero.SetAttributesForPlayer(1, Abv_tank, CYAN, BField.X + 16 * 8, BField.Y + 16 * 8, false, false);

  Enemy.CreateOpponent();
  Enemy.SetBattleField(BField);

  Hero.MoveOpponent(toRight, true);
}

void loop()
{
  // put your main code here, to run repeatedly:
  static int DP, LDP;
  static byte Direction = Upwards, counter = 0;
  static byte lastShot = 0, shotFired, GameOver = 0;
  static int Score = 0, lastScore = 0, SpeedUp = 0, Level = 1, LastGameScore = 0, HeroDeaths = 0;

  if (MovePlayerTimer.Run(100) & !GameOver)
  {
    D_Pad(&DP, &LDP, false);

    //===========================>MOVE_HERO<=====================================
    if (DP != Idle)
    {
      Direction = DP;
      Hero.MoveOpponent(Direction, true);
    }
  }

  //===============================>MOVE_Combatant<========================================
  if (MoveEnemyTimer.Run(500 - SpeedUp) & !GameOver)
  {
    Enemy.MoveOpponent(255, false);
    myGLCD.setColor(WHITE);
    myGLCD.drawRect(BField.X + 1, BField.Y + 1, BField.X2 + 1, BField.Y2 + 1);
    if (random(0, 5) == 0)
      EnemyBullets.Shoot(Enemy);
  }

  //===========================>MAKE_AND_FIRE_BULLETS<===================================
  if ( ((shotFired = Fire.Delay(200)) != lastShot) & !GameOver)
  {
    if (shotFired)
      HeroBullets.Shoot(Hero);
    lastShot = shotFired;
  }

  if (MoveBulletTimer.Run(100) & !GameOver)
  {
    HeroBullets.MoveBullet();
    HeroBullets.CheckDeadBullets();
    EnemyBullets.MoveBullet();
    EnemyBullets.CheckDeadBullets();
  }

  if (CleanCombatant.Run(50) & !GameOver)
  {
    HeroBullets.CheckBulletHitPlayer(Enemy);
    EnemyBullets.CheckBulletHitPlayer(Hero);

    Score = HeroBullets.getScore();
    HeroDeaths = EnemyBullets.getScore();
    myGLCD.setColor(WHITE);
    char buf[30];
    sprintf(buf, "%04d | Lv:%d | Life:%d | %04d", Score, Level, 3 - HeroDeaths, LastGameScore);

    if (HeroDeaths == 3)
    {
      GameOver = true;
      myGLCD.setColor(WHITE);
      myGLCD.print("GAME OVER", ((BField.X + BField.X2) / 2) -  16 * (strlen("GAME OVER") / 2), (BField.Y + BField.Y2) / 2 );
    }

    myGLCD.print(buf, BField.X - 16, BField.Y - 30);
    if (((Score % 10) == 0) && Score != lastScore)
    {
      if (Level < 10)
      {
        Enemy.CreateOpponent();
        lastScore = Score;
        SpeedUp += 25;
        Level++;
      }

      Enemy.RefillHealth(); // reset health for all enemies
      myGLCD.setColor(BField.Color);
      myGLCD.fillRect(BField.X, BField.Y, BField.X2, BField.Y2);
      TFTG.drawSprite_16(Hero.Combatant[0].Type, Hero.Combatant[0].X, Hero.Combatant[0].Y, 1, Hero.Combatant[0].Direction, false, Hero.Combatant[0].Color, -1);
    }
  }

  //=====================GAMEOVER=============================
  if (GameOver)
  {
    if (Fire.Delay(1000))
    {
      Hero.NOC = 1;
      Enemy.NOC = 1;
      HeroBullets.Reload();
      EnemyBullets.Reload();
      HeroBullets.ResetScore();
      EnemyBullets.ResetScore();

      myGLCD.setColor(BField.Color);
      myGLCD.fillRect(BField.X, BField.Y, BField.X2, BField.Y2);
      GameOver = false;
    }
  }
}

void D_Pad(int *Dir, int* Ldp, byte lock)
{
  *Dir = Idle;
  byte _up, _down, _left, _right;
  static byte _lup, _ldown, _lleft, _lright;
  if ((_up = Tup.Touch()) != (_lup & lock))
  {
    if (_up)
      *Dir = Upwards;
    _lup = _up;
  }
  if ((_down = Tdown.Touch()) != (_ldown & lock))
  {
    if (_down)
      *Dir = Downwards;
    _ldown = _down;
  }
  if ((_left = Tleft.Touch()) != (_lleft & lock))
  {
    if (_left)
      *Dir = toLeft;
    _lleft = _left;
  }
  if ((_right = Tright.Touch()) != (_lright & lock))
  {
    if (_right)
      *Dir = toRight;
    _lright = _right;
  }

  if (*Dir)
    *Ldp = *Dir;
}



