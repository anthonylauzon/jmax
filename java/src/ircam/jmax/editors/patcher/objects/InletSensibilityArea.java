package ircam.jmax.editors.patcher.objects;

import ircam.jmax.editors.patcher.interactions.Squeack;

public class InletSensibilityArea extends InletOutletSensibilityArea
{
  public int getSqueackBits()
  {
    return Squeack.INLET;
  }
}
