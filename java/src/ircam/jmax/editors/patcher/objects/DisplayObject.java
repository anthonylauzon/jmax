package ircam.jmax.editors.patcher.objects;


import java.awt.*;

/** The interface that everything representing a part of the screen
  (objects, connections, sensible areas) 
  should implement. For the moment is empty ... it may stay empty
  or it can get more complex
  */


public interface DisplayObject
{
  public SensibilityArea getSensibilityAreaAt( int mouseX, int mouseY);
  abstract public void paint(Graphics g);
  //  abstract public void redraw();
  abstract public boolean intersects(Rectangle r);
}
