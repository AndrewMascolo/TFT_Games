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
