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
#ifndef TFTSprites_h
#define TFTSprites_h

#ifdef UTFT_h
#define DISPLAY UTFT
#else
#define DISPLAY ITDB02
#endif

extern uint16_t Sprites[];
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
};

const byte ColorPallet[16] =
{ 
  0x0, 0x01, 0x10, 0x11, 0x05, 0x50, 0x55, 0x0A,
  0xA0, 0xAA, 0x0E, 0xE0, 0xEE, 0x0F, 0xF0, 0xFF
};

//=====================COLOR_PALLET==========================
#define BLACK   0x0001
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
#define TRANSPARENT 0x0

class TFTSprites
{
   public:
    TFTSprites(DISPLAY *Disp/*, TOUCH *Touch*/): _Disp(Disp)/*, _Touch(Touch)*/ {}

    DISPLAY * getDisplay() {
      return _Disp;
    }

    // TOUCH * getTouch() {
      // return _Touch;
    // }

    void Cust_Mono_Sprite_16(int * art, int x, int y, int scale, byte dir)
    {
      int S = (16 * scale);
      for (int r = 0; r < S; r++)
        for (int c = 0; c < S; c++)
        {
          if (dir == 1) // down
          {
            if (bitRead(art[15 - (r / scale)], 15 - c / scale))
              _Disp->drawPixel(x + c,  y + r);
          }
          else if (dir == 2) // left
          {
            if (bitRead(art[r / scale], c / scale))
              _Disp->drawPixel(x + r, y + c);
          }
          else if (dir == 3) // right
          {
            if (bitRead(art[15 - (r / scale)], 15 - c / scale))
              _Disp->drawPixel(x + r,  y + c);
          }
          else // up
          {
            if (bitRead(art[r / scale], 15 - c / scale))
              _Disp->drawPixel(x + c, y + r);
          }
        }
    }
	
	void Cust_Color_Sprite_16(uint8_t *image, int x, int y, int Ssize, byte dir = 0)
	{
		for (int i = 0; i <= 0xFF; i++)
		{
			_Disp->fch = ColorPallet[(image[((dir==1 || dir==3)? (255-i) : i)] >> 4)];
			_Disp->fcl = ColorPallet[(image[((dir==1 || dir==3)? (255-i) : i)] & 0x0F)];
			
			if(!_Disp->fch & !_Disp->fcl) // 0x0 = transparent, if you want black then use 0x01
			  continue;
			
			if(Ssize > 1)
			  switch(dir)
			  {
			    case 0: 
				case 1: _Disp->fillRect(x + Ssize * (i % 16), y + Ssize * (i / 16), x + Ssize * (i % 16) + Ssize, y + Ssize * (i / 16) + Ssize); break;
				case 2:
				case 3: _Disp->fillRect(x + Ssize * (i / 16), y + Ssize * (i % 16), x + Ssize * (i / 16) + Ssize, y + Ssize * (i % 16) + Ssize); break;
			  }
			else
			{
			  switch(dir)
			  {
				case 0:
				case 1:  _Disp->drawPixel(x + Ssize * (i % 16), y + Ssize * (i / 16)); break;
				case 2: 
				case 3:  _Disp->drawPixel(x + Ssize * (i / 16), y + Ssize * (i % 16)); break;
			  }
			}
		}
	}

    void drawSprite_16(int sprite, int x, int y, int scale, byte dir, byte Mir, int Fc = 0xffff, int Bc = -1)
    {
      sprite *= 16;
      int S = (16 * scale);
	  int Color;
	  
      for (int r = 0; r < S; r++)
        for (int c = 0; c < S; c++)
        {	
          if (dir == 1) // down
          {
		    if( pgm_read_word(&Sprites[sprite + (15 - (r / scale))+2]) & (1 << (15 - (Mir? S-c : c)  / scale)) )
              _Disp->setColor(Fc);
			else
			{
			  if(Bc != -1)
			    _Disp->setColor(Bc);
			  else 
			    continue;
			}
			_Disp->drawPixel(x + c,  y + r);
          }
          else if (dir == 2) // left
          {
		    if((pgm_read_word(&Sprites[sprite + (r / scale)+2]) & (1 << ((Mir? S-c : c)  / scale))))
			  _Disp->setColor(Fc);
			else
			{
			  if(Bc != -1)
			    _Disp->setColor(Bc);
			  else 
			    continue;
			}
            _Disp->drawPixel(x + r, y + c);
          }
          else if (dir == 3) // right
          {
		    if(pgm_read_word(&Sprites[sprite + (15 - (r/ scale))+2]) & (1 << (15 - (Mir? S-c : c)  / scale)))
		      _Disp->setColor(Fc);
			else
			{
			  if(Bc != -1)
			    _Disp->setColor(Bc);
			  else 
			    continue;
			}
            _Disp->drawPixel(x + r,  y + c);
          }
          else // up
          {
		    if(pgm_read_word(&Sprites[sprite + (r / scale)+2]) & (1 << (15 - (Mir? S-c : c)  / scale)))
		      _Disp->setColor(Fc);
			else
			{
			  if(Bc != -1)
			    _Disp->setColor(Bc);
			  else 
			    continue;
			}
            _Disp->drawPixel(x + c, y + r);
          }
        }
    }
	
	void drawSprite_16(int sprite, int x, int y, int scale, byte dir, byte Mir = 0)
    {
      sprite *= 16;
      int S = (16 * scale);
      for (int r = 0; r < S; r++)
        for (int c = 0; c < S; c++)
        {	
          if (dir == 1) // down
          {
            if (pgm_read_word(&Sprites[sprite + (15 - (r / scale))+2]) & (1 << (15 - (Mir? S-c : c)  / scale)))
              _Disp->drawPixel(x + c,  y + r);
          }
          else if (dir == 2) // left
          {
            if (pgm_read_word(&Sprites[sprite + (r / scale)+2]) & (1 << ((Mir? S-c : c)  / scale)))
              _Disp->drawPixel(x + r, y + c);
          }
          else if (dir == 3) // right
          {
            if (pgm_read_word(&Sprites[sprite + (15 - (r/ scale))+2]) & (1 << (15 - (Mir? S-c : c)  / scale)))
              _Disp->drawPixel(x + r,  y + c);
          }
          else // up
          {
		    if(pgm_read_word(&Sprites[sprite + (r / scale)+2]) & (1 << (15 - (Mir? S-c : c)  / scale)))
            //if (bitRead(pgm_read_word(&Sprites[sprite + (r / scale)]), 15 - (Mir? S-c : c)  / scale))
              _Disp->drawPixel(x + c, y + r);
          }
        }
    }

    void drawSprite_8(int sprite, int x, int y, int scale, byte dir)
    {
      sprite *= 8;
      int S = (8 * scale);
      for (int r = 0; r < S; r++)
        for (int c = 0; c < S; c++)
        {
          if (dir == 1) // down
          {
            if (bitRead(pgm_read_word(&Sprites[sprite + (7 - (r / scale))]), 7 - c / scale))
              _Disp->drawPixel(x + c,  y + r);
          }
          else if (dir == 2) // left
          {
            if (bitRead(pgm_read_word(&Sprites[sprite + (r / scale)]), c / scale))
              _Disp->drawPixel(x + r, y + c);
          }
          else if (dir == 3) // right
          {
            if (bitRead(pgm_read_word(&Sprites[sprite + (7 - (r / scale))]), 7 - c / scale))
              _Disp->drawPixel(x + r,  y + c);
          }
          else // up
          {
            if (bitRead(pgm_read_word(&Sprites[sprite + (r / scale)]), 7 - c / scale))
              _Disp->drawPixel(x + c, y + r);
          }
        }
    }
    
  protected:
    DISPLAY *_Disp;

};
#endif
