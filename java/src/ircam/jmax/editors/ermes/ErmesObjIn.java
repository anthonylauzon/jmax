package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;

//
// The graphic inlet contained in subpatchers
//
class ErmesObjIn extends ErmesObject {
  Dimension textDimensions = new Dimension();
  int itsId;

  public ErmesObjIn(ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject);
  }

  public void Init() 
  {
    itsId = ((FtsInletObject) itsFtsObject).getPosition();
    super.Init();
  }


  public void redefineFtsObject() 
  {
    ((FtsInletObject)itsFtsObject).setPosition(itsId);
  }

  public void Paint_specific(Graphics g) 
  {
    textDimensions.setSize( itsFontMetrics.stringWidth( "" + (itsId+1)), itsFontMetrics.getHeight());

    if (!itsSelected)
      g.setColor(itsLangNormalColor);
    else
      g.setColor(itsLangSelectedColor);

    g.fillRect( getItsX()+1, getItsY()+1, getItsWidth()-2, getItsHeight()-2);
    g.fill3DRect(getItsX()+2, getItsY()+2, getItsWidth()-4, getItsHeight()-4, true);
    g.setColor(Color.black);
    //the box
    g.drawRect( getItsX()+0, getItsY()+ 0, getItsWidth()-1, getItsHeight()-1);
    //the triangle
    g.drawLine( getItsX()+ 1, getItsY()+1, getItsX() + getItsWidth()/2, getItsY()+getItsHeight()/2);
    g.drawLine( getItsX() + getItsWidth()/2, 
		getItsY() + getItsHeight()/2, 
		getItsX() + getItsWidth() - 1,
		getItsY() + 1);

    g.setFont(getFont());
    g.drawString( "" + (itsId+1), getItsX()+2, getItsY()-2);
  }

  void ResizeToNewFont(Font theFont) 
  {
    itsSketchPad.repaint();
  }

  public void MouseDown_specific(MouseEvent evt, int x, int y) 
  {
    if (itsSketchPad.itsRunMode || evt.getClickCount() == 1) 
      {
	itsSketchPad.ClickOnObject(this, evt, x, y);
      } else 
	{ //we want to choose among the different Inlet number
	  itsSketchPad.itsInPop.SetNewOwner(this);
	  itsSketchPad.itsInPop.show(itsSketchPad, getItsX(), getItsY());
        }
  }

  void ChangeInletNo(int numberChoosen) 
  {
    if (itsId != numberChoosen) 
      {
	itsId = numberChoosen;
	redefineFtsObject();
      }
    itsSketchPad.repaint();
  }

  public void ResizeToText(int theDeltaX, int theDeltaY) 
  {
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;

    if ( aWidth < getMinimumSize().width)
      aWidth = getMinimumSize().width;
    if ( aHeight < getMinimumSize().height)
      aHeight = getMinimumSize().height;
    resizeBy( aWidth - getItsWidth(), aHeight - getItsHeight());
  }


  static Dimension minimumSize = new Dimension(15, 15);

  public Dimension getMinimumSize() 
  {
    return minimumSize;
  }

  static Dimension preferredSize = new Dimension(20, 20);

  public Dimension getPreferredSize() 
  {
    return preferredSize;
  }
}
