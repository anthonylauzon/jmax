package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;

//
// The graphic inlet contained in subpatchers
//

public class ErmesObjIn extends ErmesObjInOut {

  public ErmesObjIn(ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject, ((FtsInletObject) theFtsObject).getPosition());
  }

  public void redefine( String text) 
  {
    ((FtsInletObject)itsFtsObject).setPosition( itsId);
  }

  public void paint(Graphics g) 
  {
    if (isSelected())
      g.setColor( Settings.sharedInstance().getSelectedColor());
    else
      g.setColor( Settings.sharedInstance().getObjColor());

    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();

    g.fill3DRect( x + 1, y + 1, w - 2,  h - 2, true);

    //the triangle
    Color color = g.getColor();

//    g.setColor( color.brighter());
//     int xPoints[] = { x + 1, x + w/2, x + w - 2};
//     int yPoints[] = { y + 1, y + h/2 - 1, y + 1};
//     g.fillPolygon( xPoints, yPoints, 3);
//
//      g.setColor( color.darker());
//      g.drawLine( x + 2, y + 2, x + w/2, y + h/2);

    int xpwd2 = x + w/2;
    int S = 4;

    g.setColor( color.brighter());
    g.drawLine( xpwd2, y + 2 + S, xpwd2 + S, y + 2);

    g.setColor( color.darker());
    g.drawLine( xpwd2 - S - 1, y + 2, xpwd2 - 1, y + 2 + S);

    int ys = y + h - getFontMetrics().getDescent() - 1;
    String s = "" + itsId;
    int xs = xpwd2 - getFontMetrics().stringWidth( s)/2;

    g.setFont( getFont());
    g.setColor( Color.black);
    g.drawString( s, xs, ys);

    super.paint( g);
  }

  public void editContent()
  {
    //we want to choose among the different Inlet number

    itsSketchPad.itsInPop.SetNewOwner(this);
    itsSketchPad.itsInPop.show(itsSketchPad, getX(), getY());
  }
}
