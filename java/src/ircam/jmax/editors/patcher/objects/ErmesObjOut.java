package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;

//
// The "out" graphic object used in subpatchers.
//

public class ErmesObjOut extends ErmesObjInOut
{
  public ErmesObjOut(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject, ((FtsOutletObject) theFtsObject).getPosition());
  }

  public void redefine( String text) 
  {
    ((FtsOutletObject)itsFtsObject).setPosition( itsId);
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

    int xpwd2 = x + w/2;
    int S = 4;

    g.setColor( color.brighter());
    g.drawLine( xpwd2, y + h - 3 - S, xpwd2 + S, y + h - 3);

    g.setColor( color.darker());
    g.drawLine( xpwd2 - 1 - S, y + h - 3, xpwd2 - 1, y + h - 3 - S);

    int ys = y + getFontMetrics().getAscent() + 1;
    String s = "" + itsId;
    int xs = xpwd2 - getFontMetrics().stringWidth( s)/2;

    g.setFont( getFont());
    g.setColor( Color.black);
    g.drawString( s, xs, ys);

    super.paint( g);
  }

  public void editContent()
  {
    //we want to choose among the different Outlet number

    itsSketchPad.itsOutPop.SetNewOwner( this); //make the Choice pointing to this
    itsSketchPad.itsOutPop.show( itsSketchPad, getX(), getY());
  }
}
