/*
The MIT License (MIT)
Copyright (c) 2016 Andrew Mascolo Jr
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

//Sprites
#ifndef TFTSpritesV2_h
#define TFTSpritesV2_h

#ifdef UTFT_h
#define DISPLAY UTFT
#else
#define DISPLAY ITDB02
#endif

#define spritebyte(x) pgm_read_byte(&spriteList.sprite[x]) 
/*
enum {Upwards = 0, Downwards, toLeft, toRight, Idle, Mirror = 1};

enum {
  Blank,
  Abv_racecar,
  Side_racecar,
  Abv_helicopter,
  Side_helicopter,
  Abv_airplane,
  Side_airplane,
  Sm_house,
  Lg_house,
  Abv_truck,
  Side_truck,
  Abv_tank,
  Side_tank,
  Abv_speedboat,
  Side_speedboat,
  Abv_jet,
  Side_jet,
  Pine_tree,
  Club,
  Spade,
  Diamond,
  Full_heart,
  Half_heart,
  Empty_heart,
  Alien1,
  Skull,
  Abv_jetMissile_1,
  Abv_jetMissile_2
};*/
/*
const byte ColorPallet[16] =
{
  0x0, 0x01, 0x10, 0x11, 0x05, 0x50, 0x55, 0x0A,
  0xA0, 0xAA, 0x0E, 0xE0, 0xEE, 0x0F, 0xF0, 0xFF
};*/

//=====================COLOR_PALLET==========================
#define BLACK   0x0
#define LIGHT_RED	0xFD14
#define RED     0xF800
#define DARK_RED    0x6000
#define LIGHT_ORANGE 0xFF52
#define ORANGE  0xFD00
#define DARK_ORANGE 0xFA80
#define LIGHT_YELLOW 0xFFF4
#define YELLOW  0xD7E0
#define DARK_YELLOW 0xCE40
#define LIGHT_GREEN 0xB7F6
#define GREEN   0x07E0
#define DARK_GREEN 0x0320
#define LIGHT_BLUE 0xA51F
#define BLUE    0x001F
#define DARK_BLUE 0x000C
#define PURPLE  0xF81F
#define CYAN    0x07FF
#define GREY    0x8410
#define WHITE   0xFFFF
#define TRANSPARENT 0x01

class TFTSpritesV2
{
    struct mySprites
    {
      uint8_t* sprite;
      uint8_t x_size;
      uint8_t y_size;
      uint8_t offset;
      uint8_t num_of_sprites;
    };
    
	mySprites spriteList;
  public:
    TFTSpritesV2(DISPLAY *Disp): _Disp(Disp) {  }

    DISPLAY * getDisplay() {
      return _Disp;
    }

    void setSpriteList(uint8_t* sprite)
    {
      spriteList.sprite 		= sprite;
      spriteList.x_size 		= spritebyte(0);
      spriteList.y_size 		= spritebyte(1);
      spriteList.offset 		= spritebyte(2);
      spriteList.num_of_sprites = spritebyte(3);
    }

    void drawSprite(byte c, int x, int y, int S, int Fc, long Bc)
    {
      word temp;
      unsigned char ch;
	  bool transparent = false;
	  
	  if (Bc==TRANSPARENT)
		transparent=true;

      cbi(_Disp->P_CS, _Disp->B_CS);

      temp = ((c - spriteList.offset) * ((spriteList.x_size / 8) * spriteList.y_size)) + 4;

      for (int Y = 0; Y < ((spriteList.x_size / 8) * spriteList.y_size)*S; Y += (spriteList.x_size / 8) * S)
      {
        for (byte s = 0; s < S; s++)
        {
          _Disp->setXY(x, y + s + (Y / (spriteList.x_size / 8)), x + spriteList.x_size * S - 1, y + s + (Y / (spriteList.x_size / 8)));

          for (int xx = (spriteList.x_size / 8) - 1; xx >= 0; xx--)
          {
            ch = spritebyte(temp + xx);
            for (byte i = 0; i < 8 * S; i++)
            {
              if ((ch & (1 << i / S)) != 0)
              {
                _Disp->setPixel(Fc);
              }
              else
              {
                if (!transparent)
                  _Disp->setPixel(Bc);
              }
            }
          }
        }
        temp += (spriteList.x_size / 8);
      }

      sbi(_Disp->P_CS, _Disp->B_CS);
      _Disp->clrXY();
    }

    void rotateSprite(byte c, int x, int y, int pos, int S, int deg, int Fc, long Bc)
    { 
	  bool transparent = false;
	  
	  if (Bc==TRANSPARENT)
		transparent=true;
	  
      if (deg == 0)
      {
        drawSprite(c, x, y, S, Fc, Bc);
        return;
      }

      byte i, j, ch;
      word temp;
      int newx, newy;
      double radian;
      radian = deg * 0.0175;

      cbi(_Disp->P_CS, _Disp->B_CS);

      temp = ((c - spriteList.offset) * ((spriteList.x_size / 8) * spriteList.y_size)) + 4;
      for (j = 0; j < spriteList.y_size * S; j += S)
      {
        for (int zz = 0; zz < (spriteList.x_size / 8)*S; zz += S)
        {
          ch = spritebyte(temp + (zz / S));
          for (i = 0; i < (8 * S); i++)
          {
            for (int s = 0; s < S; s++)
            {
              newx = x + (((i + (zz * 8) + (pos * spriteList.x_size)) * cos(radian)) - ((j + s) * sin(radian)));
              newy = y + (((j + s) * cos(radian)) + ((i + (zz * 8) + (pos * spriteList.x_size)) * sin(radian)));

              _Disp->setXY(newx, newy, newx + 1, newy + 1);

              if ((ch & (1 << (7 - (i / S)))) != 0)
              {
                _Disp->setPixel(Fc);
              }
              else
              {
                if (!transparent)
                  _Disp->setPixel(Bc);
              }
            }
          }
        }
        temp += (spriteList.x_size / 8);
      }
      sbi(_Disp->P_CS, _Disp->B_CS);
      _Disp->clrXY();
    }
	
  protected:
    DISPLAY *_Disp;

};
#endif
