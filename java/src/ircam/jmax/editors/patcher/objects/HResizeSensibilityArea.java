package ircam.jmax.editors.patcher.objects;

import ircam.jmax.editors.patcher.interactions.Squeack;

public class HResizeSensibilityArea extends SensibilityArea
{
  static int width = 4;

  public int getSqueackBits()
  {
    return Squeack.HRESIZE_HANDLE;
  }
}
