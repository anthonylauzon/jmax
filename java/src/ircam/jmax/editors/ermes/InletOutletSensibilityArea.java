package ircam.jmax.editors.ermes;

class InletOutletSensibilityArea extends SensibilityArea {
  static int height = 4;

  void setNumber( int n)
    {
      this.n = n;
    }

  int getNumber()
    {
      return n;
    }

  private int n = 0;
}
