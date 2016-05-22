#define IsWithin(x,a,b) (x >= a && x <= b)
enum {_Dead, _Alive, _Hit, _Limbo};

struct Field
{
  int X, Y, X2, Y2, Mult;
  int Color;
} BField;

template<class T> inline Print &operator<<(Print &_port, T str)
{
  _port.print(str);
  return _port;
}

class simpleTimer
{
    unsigned long prevTime;
  public:
    simpleTimer() {
      prevTime = millis();
    }
    bool Run(unsigned long T)
    {
      if (millis() - prevTime >= T)
      {
        prevTime += T;
        return true;
      } else return false;
    }
};
