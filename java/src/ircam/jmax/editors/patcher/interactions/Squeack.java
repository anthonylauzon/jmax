package ircam.jmax.editors.patcher.interactions;

/** A squeack is a mouse action (squeack !); this class
  contains all the constants for coding mouse actions
  in the patcher editor.
  */

public class Squeack
{
  // The following flags are combined to create mouse events integers

  // mouse action; 4 bit reserved

  static final public int MOVE         = 0x1;
  static final public int DRAG         = 0x2;
  static final public int DOWN         = 0x3;
  static final public int UP           = 0x4;
  static final public int DOUBLE_CLICK = 0x5;

  static final public int MOUSE_MASK  = 0xf;


  static final public boolean isMove(int squeack)
  {
    return (squeack & MOUSE_MASK) == MOVE;
  }

  static final public boolean isDrag(int squeack)
  {
    return (squeack & MOUSE_MASK) == DRAG;
  }

  static final public boolean isDown(int squeack)
  {
    return (squeack & MOUSE_MASK) == DOWN;
  }

  static final public boolean isUp(int squeack)
  {
    return (squeack & MOUSE_MASK) == UP;
  }

  static final public boolean isDoubleClick(int squeack)
  {
    return (squeack & MOUSE_MASK) == DOUBLE_CLICK;
  }

  // Modifiers, 4 bit reserved, multiple value possible, but not currently generated

  static final public int SHIFT    = 0x10;
  static final public int CTRL     = 0x20;
  static final public int ALT      = 0x40;

  static final public int MODIFICATION_MASK     = 0xf0;

  static final public boolean isShift(int squeack)
  {
    return (squeack & SHIFT) != 0;
  }

  static final public boolean isCtrl(int squeack)
  {
    return (squeack & CTRL) != 0;
  }

  static final public boolean isAlt(int squeack)
  {
    return (squeack & ALT) != 0;
  }

  // Location, 8 bit reserved

  static final public int UNKNOWN    = 0x0000;
  static final public int BACKGROUND = 0x0100;
  static final public int OBJECT     = 0x0200;
  static final public int CONNECTION = 0x0300;
  static final public int HRESIZE_HANDLE = 0x0400;
  static final public int VRESIZE_HANDLE = 0x0500;
  static final public int INLET      = 0x0600;
  static final public int OUTLET     = 0x0700;

  static final public int LOCATION_MASK       = 0xff00;

  static final public boolean onUnknown(int squeack)
  {
    return (squeack & LOCATION_MASK) == UNKNOWN;
  }

  static final public boolean onBackground(int squeack)
  {
    return (squeack & LOCATION_MASK) == BACKGROUND;
  }

  static final public boolean onObject(int squeack)
  {
    return (squeack & LOCATION_MASK) == OBJECT;
  }

  static final public boolean onConnection(int squeack)
  {
    return (squeack & LOCATION_MASK) == CONNECTION;
  }


  static final public boolean onHResizeHandle(int squeack)
  {
    return (squeack & LOCATION_MASK) == HRESIZE_HANDLE;
  }

  static final public boolean onVResizeHandle(int squeack)
  {
    return (squeack & LOCATION_MASK) == VRESIZE_HANDLE;
  }

  static final public boolean onInlet(int squeack)
  {
    return (squeack & LOCATION_MASK) == INLET;
  }

  static final public boolean onOutlet(int squeack)
  {
    return (squeack & LOCATION_MASK) == OUTLET;
  }
}
