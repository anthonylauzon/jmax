package ircam.jmax.editors.patcher.interactions;

/** A squeack is a mouse action (squeack !); this class
  contains all the constants for coding mouse actions
  in the patcher editor.
  */

public class Squeack
{
  // The following flags are combined to create mouse events integers

  // mouse action; 4 bit reserved

  static public final int MOVE         = 0x1;
  static public final int DRAG         = 0x2;
  static public final int DOWN         = 0x3;
  static public final int UP           = 0x4;
  static public final int DOUBLE_CLICK = 0x5;

  static public final int MOUSE_MASK  = 0xf;


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

  static public final int SHIFT    = 0x10;
  static public final int CTRL     = 0x20;
  static public final int ALT      = 0x40;

  static public final int MODIFICATION_MASK     = 0xf0;

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

  static public final int UNKNOWN    = 0x0000;
  static public final int BACKGROUND = 0x0100;
  static public final int OBJECT     = 0x0200;
  static public final int CONNECTION = 0x0300;
  static public final int HRESIZE_HANDLE = 0x0400;
  static public final int VRESIZE_HANDLE = 0x0500;
  static public final int INLET      = 0x0600;
  static public final int OUTLET     = 0x0700;
  static public final int TEXT       = 0x0800;

  static public final int LOCATION_MASK       = 0xff00;

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

  static final public boolean onText(int squeack)
  {
    return (squeack & LOCATION_MASK) == TEXT;
  }
}



