package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;

//
// The graphic inlet that appears on the top af the objects.
//
public class ErmesObjInlet extends ErmesObjInOutlet {

  int itsInletNum;

  public ErmesObjInlet( int theInletNum, ErmesObject theOwner, int x_coord, int y_coord) 
  {
    super( theOwner, x_coord, y_coord);
    itsInletNum = theInletNum;
  }

  public void Paint_specific(Graphics g) 
  {
    if ( !itsOwner.itsSketchPad.itsGraphicsOn)
      return;

    g.setColor(Color.black);

    if ( itsOwner.itsSketchPad.itsRunMode)
      g.fillRect( itsX+1, itsY+7, getPreferredSize().width-2, 2);
    else 
      {
	if (selected) 
	  {
	    g.drawOval( itsX+1, itsY, getPreferredSize().width-2, getPreferredSize().height-4);
	    g.drawLine( itsX+3, itsY+4, itsX+3, itsY+8);
	  } 
	else
	  g.fillRect( itsX+1, itsY+7, getPreferredSize().width-2, 2);
      }
  }


  public boolean IsInlet() 
  {
    return true;
  }

  public int GetInletNum() 
  {
    return itsInletNum;
  }

  public void updateAnchorPoint() 
  {
    itsAnchorPoint.setLocation( itsX+3, itsY+6);
  }
}
