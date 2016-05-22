// Use IDE 1.0.6 or 1.5.x
#include <UTFT.h>
#include <UTouch.h>
#include <TFT_ExtensionV2.h>
#include <TFT_ScaleFonts.h>

template<class T> inline Print &operator <<(Print &str, T arg)
{
  str.print(arg);
  return str;
}

extern uint8_t SmallFont[];

//UTFT    myGLCD(CTE70, 25, 26, 27, 28);
UTFT    myGLCD(ITDB32S,25, 26, 27, 28);
UTouch  myTouch( 6, 5, 4, 3, 2);
TFT_ScaleFonts myFont(&myGLCD);

Base B(&myGLCD, &myTouch); // Base class, NEEDED!!!

Circle *Keys[5];
Circle *Numbers[9];
Circle Button1(&B);
Box CheckBtn(&B), TimerBtn(&B);
Box EasyBtn(&B), MediumBtn(&B), HardBtn(&B), PlayBtn(&B), YesBtn(&B), NoBtn(&B);
Box * DifficultyBtns[3] = {&EasyBtn, &MediumBtn, &HardBtn};
Radio Difficulty;

unsigned long Randcode = 0, code = 0;
byte Randomcode[5];
int MaxColumns = 0;
byte GameOver = true, c = 0, GoodNum = 0, Row = 0;
long TIME;
int R = 280, C = 25;
int Clock;
byte Code[5] = {0}, Seq[5] = {0};
byte Disabled[(5 * 2) - 1] = {false};
unsigned long Score = 0;

void setup()
{
  Serial.begin(115200);

  myGLCD.InitLCD(PORTRAIT);
  myGLCD.setFont(SmallFont);
  myGLCD.clrScr();
  myTouch.InitTouch(PORTRAIT);
  myTouch.setPrecision(PREC_HI);
  myFont.TextBounds(0,0, 240, 320);// NEW FEATURE TO MY SCALEFONTS LIBRARY.
  
  myGLCD.fillScr(0);

  EasyBtn.Coords(10, 100, 70, 140);
  EasyBtn.Colors(GREEN, DARK_YELLOW, ROUNDED, FILL);
  EasyBtn.Text("EASY", BLACK, Small);

  MediumBtn.Coords(80, 100, 160, 140);
  MediumBtn.Colors(GREEN, DARK_YELLOW, ROUNDED, FILL);
  MediumBtn.Text("MEDIUM", BLACK, Small);

  HardBtn.Coords(170, 100, 230, 140);
  HardBtn.Colors(GREEN, DARK_YELLOW, ROUNDED, FILL);
  HardBtn.Text("HARD", BLACK, Small);

  PlayBtn.Coords(10, 150, 230, 190);
  PlayBtn.Colors(GREEN, DARK_YELLOW, ROUNDED, FILL);
  PlayBtn.Text("Play", BLACK, Big);

  YesBtn.Coords(10, 100, 70, 140);
  YesBtn.Colors(GREEN, RED, ROUNDED, FILL);
  YesBtn.Text("Yes", BLACK, Big);

  NoBtn.Coords(170, 100, 230, 140);
  NoBtn.Colors(GREEN, RED, ROUNDED, FILL);
  NoBtn.Text("No", BLACK, Big);

  myGLCD.setColor(GREEN);
  myGLCD.drawRect(0, 0, 239, 319);

  MaxColumns = StartupScreen();
  Serial.println(MaxColumns);
  TIME = (MaxColumns - 2);
  Clock = 60 * TIME;

  myGLCD.fillScr(0);

  for (byte i = 0; i < MaxColumns; i++)
  {
    Keys[i] = new Circle(&B);
    myGLCD.setColor(RED);
    myGLCD.drawLine(147 + i * 20, 170, 147 + i * 20, 270);
    myGLCD.drawLine(147 + i * 20, 170, 142 + i * 20, 150);
    myGLCD.setColor(BLUE);
    myGLCD.drawLine(147 + i * 20, 130, 147 + i * 20, 60);
    myGLCD.drawLine(152 + i * 20, 150, 147 + i * 20, 130);
  }

  for (byte i = 0; i < (MaxColumns * 2) - 1; i++)
  {
    Numbers[i] = new Circle(&B);
  }

  TimerBtn.Coords(137, 20, 137 + MaxColumns * 20, 60);
  TimerBtn.Colors(YELLOW, ROUNDED, FILL);
  TimerBtn.Text("00:00", BLACK, Small);
  TimerBtn.ReDraw();

  CheckBtn.Coords(137, 270, 137 + MaxColumns * 20, 310);
  CheckBtn.Colors(GREEN, RED, ROUNDED, FILL);
  CheckBtn.Text("Check", BLACK, Small);
  CheckBtn.ReDraw();

  makeBoard();
  makeNumbers();
  myGLCD.setColor(DARK_GREEN);
  myGLCD.drawRect(0, 0, 239, 319);

  myGLCD.setColor(WHITE);
  myGLCD.fillRect(5, 255, 8, 245);
  myGLCD.printNumI(Score, CENTER, 2, 8, '0');
}

void loop()
{
  static unsigned long prevTime = millis();

  if (GameOver)
  {
    Randcode = RandomCode();
    GameOver = false;
  }

  R = 280;
  C = 22;

  if (c >= MaxColumns)
  {
    R = 305;
    C = 34;
  }

  Numbers[c]->Coords(C + 25 * (c % MaxColumns), R, 10);
  if (Numbers[c]->Touch() & !Disabled[c])
  {
    Disabled[c] = true;
    char buff[2] = " ";
    Numbers[c]->getText(buff);
    SetKey(GoodNum, Row, buff);

    code = (code * 10) + buff[0] - '0';
    Serial << "GN:" << GoodNum << " Code:" << code << " RandomNum:" << Randcode << "\n";
    Code[GoodNum] = buff[0] - '0';
    GoodNum++;
  }
  
  if(TimerBtn.Delay(1000))
  {
     GameoverScreen("YOU QUIT");
     Reset(); 
  }

  if ((GoodNum > (MaxColumns - 1)) || CheckBtn.DoubleClick() || (Clock <= 0))
  {
    if (Logic(Randomcode, Code, Seq))
    {
      delay(1000);
      GameoverScreen("YOU WIN");
      Score += Clock * (10 - Row);
      Reset();
    }
    else
    {
      Serial.println("Incorrect code, Try again");
      Clock -= 5;
      delay(1000);
      Row++;
      myGLCD.setColor(BLACK);
      myGLCD.fillRect(5, 255 - (25 * (Row - 1)), 8, 245 - (25 * (Row - 1)));
      myGLCD.setColor(WHITE);
      myGLCD.fillRect(5, 255 - (25 * Row), 8, 245 - (25 * Row));
    }

    if ((Row == 10) || (Clock <= 0))
    {
      delay(1000);
      GameoverScreen("YOU LOSE");
      Reset();
    }

    for (byte i = 0; i < (MaxColumns * 2) - 1; i++)
      Disabled[i] = false;

    myGLCD.setColor(BLACK);
    myGLCD.fillRect(140, 170, 135 + MaxColumns * 20, 130);
    for (byte i = 0; i < MaxColumns; i++)
    {
      myGLCD.setColor(RED);
      myGLCD.drawLine(147 + i * 20, 170, 142 + i * 20, 150);
      myGLCD.setColor(BLUE);
      myGLCD.drawLine(152 + i * 20, 150, 147 + i * 20, 130);
      Code[i] = 0;
    }

    code = 0;
    GoodNum = 0;
  }

  c++;
  if (c > (MaxColumns * 2) - 2) c = 0;

  if (GameOver == false)
  {
    if (millis() - prevTime >= 1000)
    {
      Clock--;
      char buff[] = "00:00";
      sprintf(buff, "%02d:%02d", Clock / 60, Clock % 60);
      TimerBtn.Text(buff, BLACK, Small);
      TimerBtn.ReDraw();
      prevTime = millis();
    }
  }
}

byte StartupScreen()
{
  EasyBtn.ReDraw();
  MediumBtn.ReDraw();
  HardBtn.ReDraw();
  PlayBtn.ReDraw();

  byte gameDifficulty = 0;
  byte Diff;

  myGLCD.setColor(WHITE);
  myFont.print("CODE BREAKER", CENTER, 50, 2, 0);
  myGLCD.print("Hold Timer 1 second to quit.", 10, 260);
  myGLCD.print("Portrait only on 3.2in", 10, 280);
  myGLCD.print("Wrong codes are -5 seconds", 10, 300);
  
  while (!PlayBtn.DoubleClick())
  {
    if ((Diff = Difficulty.RadioButtons(DifficultyBtns)))
    {
      gameDifficulty = Diff + 2;
    }
  }
  
  Score = 0; 
  return gameDifficulty;
}

void Reset()
{
  myGLCD.clrScr();
  makeBoard();
  makeNumbers();
  myGLCD.setColor(WHITE);
  myGLCD.fillRect(5, 255, 8, 245);

  c = 0;
  Row = 0;
  myGLCD.setColor(BLACK);
  myGLCD.fillRect(140, 170, 135 + MaxColumns * 20, 130);
  for (byte i = 0; i < MaxColumns; i++)
  {
    myGLCD.setColor(RED);
    myGLCD.drawLine(147 + i * 20, 170, 142 + i * 20, 150);
    myGLCD.drawLine(147 + i * 20, 170, 147 + i * 20, 270);
    myGLCD.setColor(BLUE);
    myGLCD.drawLine(152 + i * 20, 151, 147 + i * 20, 130);
    myGLCD.drawLine(147 + i * 20, 130, 147 + i * 20, 60);
    Randomcode[i] = 0;
  }
  CheckBtn.ReDraw();
  TimerBtn.ReDraw();
  Clock = 60 * TIME;
  myGLCD.setColor(DARK_GREEN);
  myGLCD.drawRect(0, 0, 239, 319);
  
  myGLCD.setColor(WHITE);
  myGLCD.printNumI(Score, CENTER, 2, 8, '0');
}

void GameoverScreen(char * str)
{
  myGLCD.clrScr();
  myGLCD.setColor(DARK_GREEN);
  myGLCD.drawRect(0, 0, 239, 319);
  
  myGLCD.setColor(WHITE);
  myFont.print(str, CENTER, 20, 3, 0);
  myFont.print("Play Again?", CENTER, 60, 2, 0);
  YesBtn.ReDraw();
  NoBtn.ReDraw();

  while (true)
  {
    if (YesBtn.Delay(500))
    {
      GameOver = true;
      break;
    }

    if (NoBtn.Delay(500))
    {
      setup();
      GameOver = true;
      break;
    }
  }
}

void makeBoard()
{
  int C = 22, R = 250;

  for (byte r = 0; r < 10; r++)
  {
    for (byte c = 0; c < MaxColumns; c++)
    {
      Keys[c]->Coords(C + 25 * c, R - 25 * r, 10);
      Keys[c]->Colors(GREY, FILL);
      Keys[c]->Padding(1, WHITE);
      Keys[c]->Text("?", WHITE, Small);
      Keys[c]->ReDraw();
    }
  }
}

void SetKey(byte Col, byte Row, char* Num)
{
  int ColorPallet[9] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, CYAN, DARK_RED, DARK_BLUE};
  int C = 22, R = 250;
  Keys[Col]->Coords(C + 25 * Col, R - 25 * Row, 10);
  Keys[Col]->Colors(ColorPallet[Num[0] - '1'], FILL);
  Keys[Col]->Padding(1, WHITE);
  char buff[2] = " ";
  strcpy(buff, Num);
  Keys[Col]->Text(buff, WHITE, Small);
  Keys[Col]->ReDraw();
}

void makeNumbers()
{
  int ColorPallet[9] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, CYAN, DARK_RED, DARK_BLUE};
  char *NumText[9] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
  int C = 22, R = 280;
  for (byte c = 0; c < (MaxColumns * 2) - 1; c++)
  {
    if (c >= MaxColumns)
    {
      R = 305;
      C = 34;
    }
    Numbers[c]->Coords(C + 25 * (c % MaxColumns), R, 10);
    Numbers[c]->Colors(ColorPallet[c], FILL);
    Numbers[c]->Padding(1, WHITE);
    Numbers[c]->Text(NumText[c], BLACK, Small);
    Numbers[c]->ReDraw();
  }
}

byte Logic(byte *RandC, byte *code, byte *Seq)
{
  byte Matched = 0;
  byte InSeq = 0;

  myGLCD.setColor(BLACK);
  myGLCD.fillRect(140, 130, 135 + MaxColumns * 20, 170);
  for (byte i = 0; i < MaxColumns; i++)
  {
    for (byte j = 0; j < MaxColumns; j++)
    {
      if (RandC[i] == code[j])
      {
        InSeq++;
      }
    }

    Seq[i] = false;
    if (RandC[i] == code[i])
    {
      Seq[i] = true;
      Matched++;
    }

    myGLCD.setColor(RED);
    myGLCD.drawLine(147 + i * 20, 170, (Seq[i] ? 147 : 142) + i * 20, 150);
  }

  for (byte C = 0; C < MaxColumns; C++)
  {
    myGLCD.setColor(BLUE);
    myGLCD.drawLine( (((C + 1) <= InSeq) ? 147 : 152) + C * 20, 150, 147 + C * 20, 130);
  }
  Serial << "Order:" << InSeq << " Matched:" << Matched << "\n";
  if (Matched == MaxColumns)
    return true;

  return false;
}

unsigned long RandomCode()
{
  unsigned long code = 0;
  byte uniqueCount = 0;

  while (uniqueCount < MaxColumns)
  {
    byte num = random(1, (MaxColumns * 2));
    byte found = false;
    for (byte j = 0; j < MaxColumns; j++)
    {
      if (Randomcode[j] == num)
      {
        found = true;
        break;
      }
    }

    if (found == false)
    {
      Randomcode[uniqueCount] = num;
      code = code * 10 + Randomcode[uniqueCount];
      uniqueCount++;
    }
  }
  return code;
}
