package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

//
// The "out" graphic object used in subpatchers.
//

class ErmesObjOut extends ErmesObject {

  int itsId;

  public ErmesObjOut(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    itsId = ((FtsOutletObject) itsFtsObject).getPosition();
  }


  void redefine( String text)
  {
    ((FtsOutletObject)itsFtsObject).setPosition( itsId);
  }
  
  public void MouseDown_specific(MouseEvent evt, int x, int y) 
  {
    if (itsSketchPad.itsRunMode || evt.getClickCount() == 1) 
      {
	itsSketchPad.ClickOnObject(this, evt, x, y);
      }
    else 
      {	//we want to choose among the different Outlet number
	itsSketchPad.itsOutPop.SetNewOwner( this); //make the Choice pointing to this
	itsSketchPad.itsOutPop.show( itsSketchPad, getX(), getY());
      }
  }

  void ChangeOutletNo( int numberChoosen) 
  {
    if ( itsId != numberChoosen) 
      {
	itsId = numberChoosen;
	redefine( "");
      }
    itsSketchPad.repaint();
  }

  public void Paint_specific(Graphics g) 
  {
    if ( !itsSelected)
      g.setColor(itsLangNormalColor);
    else 
      g.setColor(itsLangSelectedColor);

    g.fillRect( getX()+1, getY()+1, getWidth()-2,  getHeight()-2);
    g.fill3DRect( getX()+2, getY()+2, getWidth()-4,  getHeight()-4, true);
    g.setColor( Color.black);

    //the box
    g.drawRect( getX()+0, getY()+0, getWidth()-1, getHeight()-1);

    //the triangle
    g.drawLine( getX() + 1, 
		getY() + getHeight()-1, 
		getX() + getWidth()/2, 
		getY() + getHeight()/2);

    g.drawLine( getX() + getWidth()/2,
		getY() + getHeight()/2,
		getX() + getWidth()-1,
		getY() + getHeight()-1);

    g.setFont( getFont());
    g.drawString( "" + (itsId+1), getX() + 2, getY() + getHeight() + itsFontMetrics.getHeight() - 1);
  }
	
  void ResizeToNewFont(Font theFont) 
  {
    itsSketchPad.repaint();
  }

  public void ResizeToText( int theDeltaX, int theDeltaY)
  {
    int aWidth = getWidth() + theDeltaX;
    int aHeight = getHeight() + theDeltaY;

    if ( aWidth < getMinimumSize().width)
      aWidth = getMinimumSize().width;

    if( aHeight < getMinimumSize().height) 
      aHeight = getMinimumSize().height;

    resizeBy( aWidth - getWidth(), aHeight - getHeight());
  }

  static Dimension minimumSize = new Dimension(15,15); 

  public Dimension getMinimumSize() 
  {
    return minimumSize;
  }
}
