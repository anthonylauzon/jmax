package ircam.jmax.editors.patcher;

import java.awt.*;

//
// The edit field contained in the editable objects (ErmesObjMessage, ErmesObjExternal).
//

public interface ObjectRenderer
{
  public void update();

  public boolean canResizeWidthTo(int width);

  public int getHeight();

  public int getWidth();

  public void setBackground(Color color);

  public void render(Graphics g, int x, int y, int w, int h);
}



