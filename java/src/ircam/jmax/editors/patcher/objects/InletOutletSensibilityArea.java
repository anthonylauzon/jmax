package ircam.jmax.editors.patcher.objects;

public abstract class InletOutletSensibilityArea extends SensibilityArea
{
  static int height = 4;

  void setNumber( int n)
    {
      this.n = n;
    }

  public int getNumber()
    {
      return n;
    }

  private int n = 0;
}
