#include "Utilities.h"
class Player : public Field
{
  private:
    struct Players
    {
      byte Type;
      int  Color;
      byte Health;
      int X, lastX;
      int Y, lastY;
      byte Dead;
      byte Direction;
    };

    byte MaxOpps;

    enum {_Dead, _Alive, _Hit, _Limbo};

    int colors[3] = {RED, YELLOW, GREEN};
    byte Types[3] = {Abv_tank, Abv_helicopter, Abv_jet};
    byte _AD, _AR;
    //unsigned int BulletDuration;

  protected:
    TFTSprites *TFTG;

  public:
    byte NOC; // Number of Combatants
    Players *Combatant;
    
    Player( byte Max, TFTSprites *sprites) : MaxOpps(Max), TFTG(sprites)
    {
      NOC = 0;
      Combatant = new Players[Max];
      _AR = true;
      _AD = true;

      for (byte idx = 0; idx < Max; idx++)
      {
        Combatant[idx].Dead = _Dead;
        Combatant[idx].Type = Abv_tank;
        Combatant[idx].Color = GREEN;
        Combatant[idx].Direction = Downwards;
        Combatant[idx].Health = 3;
      }
    }

    void SetAttributesForPlayer(byte PL, int Type, int Color, int X, int Y, byte R, byte D)
    {
      PL -= 1;

      if (NOC > 0)
      {
        if ((PL < NOC) && (PL < MaxOpps))
        {
          Combatant[PL].Type = Type;
          Combatant[PL].Color = Color;
          Combatant[PL].X = X;
          Combatant[PL].Y = Y;
        }
      }
      colors[2] = Color;
      _AD = D;
      _AR = R;
    }

    void SetBattleField(struct Field &Battlefield)
    {
      BField = Battlefield;
    }

    void CreateOpponent()
    {
      NOC++;
      if (NOC >= MaxOpps)
        NOC = MaxOpps;

      RepurposeOpponent(NOC - 1);
      Serial << "Created Op: " << NOC << "\n";
    }

    void MoveOpponent(byte Direction, byte withBoundaries)
    {
      for (byte i = 0; i < NOC; i++)
      {
        if ((Combatant[i].Dead == _Alive) || (Combatant[i].Dead == _Hit))
        {
          Serial << "Moving Op: " << i << "\n";
          if (Direction != 255)
            Combatant[i].Direction = Direction;

          Combatant[i].lastX = Combatant[i].X;
          Combatant[i].lastY = Combatant[i].Y;
          switch (Combatant[i].Direction)
          {
            case Upwards:   Combatant[i].Y -= 16; break;
            case Downwards: Combatant[i].Y += 16; break;
            case toLeft:    Combatant[i].X -= 16; break;
            case toRight:   Combatant[i].X += 16; break;
            case Idle:      break;
          }

          if (withBoundaries)
          {
            Combatant[i].X = constrain(Combatant[i].X, BField.X + 1, BField.X2 - 16 - 1);
            Combatant[i].Y = constrain(Combatant[i].Y, BField.Y + 1, BField.Y2 - 16 - 1);
          }

          TFTG->drawSprite_16(Blank, Combatant[i].lastX, Combatant[i].lastY, 1, Combatant[i].Direction, false, Combatant[i].Color, BField.Color);
          TFTG->drawSprite_16(Combatant[i].Type, Combatant[i].X, Combatant[i].Y, 1, Combatant[i].Direction, false, Combatant[i].Color, -1);
        }
      }
      CheckCasualties(_AR, _AD);
    }

    void CheckCasualties(byte allowRepurpose, byte allowDie)
    {
      for (byte i = 0; i < NOC; i++)
      {
        // See if an enemy goes outside the field bounds
        if (!IsWithin(Combatant[i].Y, BField.Y + 8, BField.Y2 - 20) || !IsWithin(Combatant[i].X, BField.X + 16, BField.X2 - 16))
        {
          TFTG->drawSprite_16(Blank, Combatant[i].X, Combatant[i].Y, 1, Combatant[i].Direction, false, Combatant[i].Color, BField.Color);

          if (allowDie)
            Combatant[i].Dead = _Limbo;

          if (allowRepurpose)
            RepurposeOpponent(i);
        }
      }
    }

    void RepurposeOpponent(byte idx)
    {
      if ((Combatant[idx].Dead == _Dead) || (Combatant[idx].Dead == _Limbo))
      {
        if (Combatant[idx].Dead == _Limbo)
        {
          Combatant[idx].Direction = random(0, 4);
          Serial << "In Limbo: " << idx << "\n";
        }

        if (Combatant[idx].Dead == _Dead)
        {
          if (_AR == true)
            Combatant[idx].Type = Types[random(0, 3)];
          Combatant[idx].Health = 3;
        }

        switch (Combatant[idx].Direction)
        {
          case Upwards:
            Combatant[idx].X = BField.X + (16 * random(1, 16));
            Combatant[idx].Y = BField.Y2 - 16;
            break;
          case Downwards:
            Combatant[idx].X = BField.X + (16 * random(1, 16));
            Combatant[idx].Y = BField.Y + 1;
            break;
          case toLeft:
            Combatant[idx].X = BField.X2 - 16;
            Combatant[idx].Y = BField.Y + (16 * random(1, 16));
            break;
          case toRight:
            Combatant[idx].X = BField.X + 1;
            Combatant[idx].Y = BField.Y + (16 * random(1, 16));
            break;
        }
      }

      if ((Combatant[idx].Dead == _Hit) || (Combatant[idx].Dead == _Dead))
      {
        Serial << "Hit: " << idx << "\n";
        Combatant[idx].Color = colors[Combatant[idx].Health - 1];
        if (Combatant[idx].Dead == _Hit)
          Combatant[idx].Direction = random(0, 4);
      }

      Combatant[idx].Dead = _Alive;
    }

    void RefillHealth()
    {
      for (byte i = 0; i < NOC; i++)
      {
        Combatant[i].Health = 3;
      }
    }

    byte TotalCombatants()
    {
      return NOC;
    }

    void Shoot()
    {

    }
};
