package ircam.jmax.editors.patcher.objects;

import ircam.jmax.editors.patcher.interactions.Squeack;

public class OutletSensibilityArea extends InletOutletSensibilityArea
{
  public int getSqueackBits()
  {
    return Squeack.OUTLET;
  }
}
