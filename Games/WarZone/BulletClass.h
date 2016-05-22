class Bullet
{
  private:
    struct Bullets
    {
      byte Type;
      int  Color;
      int X, lastX;
      int Y, lastY;
      byte Dead;
      int Speed;
      byte Direction;
    };

    int BulletTypes[3] = {Pine_tree, Abv_jetMissile_1, Abv_jetMissile_2};
    byte MaxBullets;

    enum {_Dead, _Alive, _Hit, _Limbo};
    int Fired = 0, Score = 0;

  protected:
    TFTSprites *TFTG;

  public:
    byte NOB; // number of bullets
    Bullets *_Bullets;

    Bullet( byte Max, TFTSprites *sprites) : MaxBullets(Max), TFTG(sprites)
    {
      NOB = 0;
      _Bullets = new Bullets[Max];

      for (byte idx = 0; idx < Max; idx++)
      {
        _Bullets[idx].Dead = _Dead;
        _Bullets[idx].Type = BulletTypes[0]; 
        _Bullets[idx].Color = WHITE;
      }
    }

    void SetBattleField(struct Field &Battlefield)
    {
      BField = Battlefield;
    }

    void Shoot(Player &Op)
    {
      if (Fired < MaxBullets)
      {
        byte randShot = random(0, Op.TotalCombatants());
        _Bullets[Fired].Direction = Op.Combatant[randShot].Direction;
        switch (Op.Combatant[randShot].Direction)
        {
          case Upwards:
            _Bullets[Fired].X = Op.Combatant[randShot].X;
            _Bullets[Fired].Y = Op.Combatant[randShot].Y - 16;
            break;
          case Downwards:
            _Bullets[Fired].X = Op.Combatant[randShot].X;
            _Bullets[Fired].Y = Op.Combatant[randShot].Y + 16;
            break;
          case toLeft:
            _Bullets[Fired].X = Op.Combatant[randShot].X - 16;
            _Bullets[Fired].Y = Op.Combatant[randShot].Y;
            break;
          case toRight:
            _Bullets[Fired].X = Op.Combatant[randShot].X + 16;
            _Bullets[Fired].Y = Op.Combatant[randShot].Y;
            break;
        }
        
        _Bullets[Fired].Dead = _Alive;
        Fired++;
      }
      else Reload();
    }

    void Reload()
    {
      Fired = 0;
    }

    void MoveBullet()
    {
      for (byte i = 0; i < MaxBullets; i++)
      {
        if (_Bullets[i].Dead == _Alive)
        {
          _Bullets[i].lastX = _Bullets[i].X;
          _Bullets[i].lastY = _Bullets[i].Y;

          switch (_Bullets[i].Direction)
          {
            case Upwards: _Bullets[i].Y -= 16; break;
            case Downwards: _Bullets[i].Y += 16; break;
            case toLeft: _Bullets[i].X -= 16; break;
            case toRight: _Bullets[i].X += 16; break;
            case Idle: break;
          }
          TFTG->drawSprite_16(Blank, _Bullets[i].lastX, _Bullets[i].lastY, 1, _Bullets[i].Direction, false, _Bullets[i].Color, BField.Color);
          TFTG->drawSprite_16(_Bullets[i].Type, _Bullets[i].X, _Bullets[i].Y, 1, _Bullets[i].Direction, false, _Bullets[i].Color, -1);
        }
      }
    }

    void CheckDeadBullets()
    {
      if (Fired > 0)
      {
        for (byte i = 0; i < MaxBullets; i++)
        {
          // See if the bullet goes outside the field bounds
          if (_Bullets[i].Dead == _Alive)
          {
            if (!IsWithin(_Bullets[i].Y, BField.Y + 16, BField.Y2 - 32) || !IsWithin(_Bullets[i].X, BField.X + 16, BField.X2 - 32))
            {
              int tempColor = BField.Color;
              if ((_Bullets[i].Y < BField.Y) || (_Bullets[i].Y > BField.Y2) || (_Bullets[i].X < BField.X) || (_Bullets[i].X > BField.X2))
                tempColor = BLACK;

              TFTG->drawSprite_16(Blank, _Bullets[i].X, _Bullets[i].Y, 1, _Bullets[i].Direction, false, _Bullets[i].Color, tempColor);
              _Bullets[i].Dead = _Dead;

              Fired--;
              if (Fired < 0)
              {
                Fired = 0;
                break;
              }
            }
          }
        }
      }
    }

    void CheckBulletHitPlayer(Player Op)
    {
      if (Fired > 0)
      {
        for (byte i = 0; i < MaxBullets; i++)
        {
          for (byte j = 0; j < Op.TotalCombatants(); j++)
          {
            if (IsWithin(_Bullets[i].Y, Op.Combatant[j].Y - 1, Op.Combatant[j].Y + 17) &&
                IsWithin(_Bullets[i].X, Op.Combatant[j].X - 1, Op.Combatant[j].X + 17) )
            {
              _Bullets[i].Dead = _Dead;
              TFTG->drawSprite_16(Blank, _Bullets[i].X, _Bullets[i].Y, 1, _Bullets[i].Direction, false, _Bullets[i].Color, BField.Color);

              Op.Combatant[j].Health--;
              Op.Combatant[j].Dead = _Hit;
              _Bullets[i].X = -1;
              _Bullets[i].Y = -1;

              if (Op.Combatant[j].Health <= 0)
              {
                Op.Combatant[j].Dead = _Dead;
                TFTG->drawSprite_16(Skull, Op.Combatant[j].X, Op.Combatant[j].Y, 1, Upwards, false, Op.Combatant[j].Color, BField.Color);
                Op.Combatant[j].X = -1;
                Op.Combatant[j].Y = -1;
                Score++;
              }

              Op.RepurposeOpponent(j);
            }
          }
        }
      }
    }

    unsigned int getScore()
    {
      return Score;
    }

    void ResetScore()
    {
      Score = 0;
    }
};
