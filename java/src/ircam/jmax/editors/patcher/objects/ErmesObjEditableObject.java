package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;


//
// The base class of the ermes objects which are user-editable
// (ErmesObjMessage, ErmesObjExternal, ErmesObjPatcher).
//

abstract public class ErmesObjEditableObject extends ErmesObject implements FtsInletsListener, FtsOutletsListener
{
  boolean editing = false;
  ircam.jmax.editors.patcher.ObjectRenderer renderer; // don't ask me why here we need the whole path

  ErmesObjEditableObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    Icon icon = Icons.get(itsFtsObject.getClassName());

    if (icon != null)
      {
	renderer = new IconRenderer(this, icon);
	renderer.update();
	updateDimensions();
      }
    else
      {
	renderer = new TextRenderer(this);
	renderer.update();

	if (getWidth() == -1)
	  setWidth( getFontMetrics().stringWidth( "pack 1 2 3") + 2*getTextXOffset());
      }
  }

  public void updateDimensions()
  {
    renderer.update();
    super.setHeight(renderer.getHeight() + getTextHeightOffset());
    super.setWidth(renderer.getWidth() + getTextWidthOffset());
  }

  public void updateDimensionsNoConnections()
  {
    renderer.update();
    super.setHeightNoConnections(renderer.getHeight() + getTextHeightOffset());
  }

  public void redefine( String text) 
  {
    // Change the renderer if needed

    Icon icon = Icons.get(itsFtsObject.getClassName());

    if (icon != null)
      {
	redraw();
	renderer = new IconRenderer(this, icon);
	renderer.update();
	redraw();
      }
    else if (! (renderer instanceof TextRenderer))
      {
	redraw();
	renderer = new TextRenderer(this);
	renderer.update();
	redraw();
      }

    
    updateDimensions();

    super.redefine(text);
  }


  // redefined from base class

  public  void setWidth(int w) 
  {
    // renderer.update();

    if (renderer.canResizeWidthTo(w - getTextWidthOffset()))
      {
	super.setWidth(w);
	super.setHeight(renderer.getHeight() + getTextHeightOffset());
      }
  }

  // redefined from base class, only when not editing
  // This way, changing the height from outside is forbidden

  public void setHeight( int v)
  {
    if (editing)
      super.setHeight(v);
  }

  // redefined from base class
  public void setFont( Font f)
  {
    super.setFont( f);
    renderer.update();
    super.setHeight(renderer.getHeight() + getTextHeightOffset());
  }

  // ----------------------------------------
  // ``Args'' property
  // ----------------------------------------

  abstract public String getArgs();

  // ----------------------------------------
  // ``TextBackground'' property
  // ----------------------------------------

  abstract public Color getTextBackground();

  // Properties to position correctly the text editor

  abstract public int getTextXOffset();
  abstract public int getTextYOffset();
  abstract public int getTextWidthOffset();
  abstract public int getTextHeightOffset();

  public void setEditing(boolean v)
  {
    editing = v;
  }

  // ----------------------------------------
  // Property handling
  // ----------------------------------------

  public void inletsChanged(int n)
  {
    redraw();
    redrawConnections();
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }

  public void outletsChanged(int n)
  {
    redraw();
    redrawConnections();
    itsSketchPad.getDisplayList().updateConnectionsFor(this);
  }


  public void drawContent(Graphics g) 
  {
    if (editing)
      return;

    renderer.setBackground(getTextBackground());

    renderer.render(g, 
		    getX() + getTextXOffset(),
		    getY() + getTextYOffset(),
		    getWidth() - getTextWidthOffset(),
		    getHeight() - getTextHeightOffset());
  }

  // Text Sensibility area 

  private static SensibilityArea textArea = new TextSensibilityArea();

  public SensibilityArea findSensibilityArea( int mouseX, int mouseY)
  {
    int dx = mouseX - (getX() + getTextXOffset());
    int dy = mouseY - (getY() + getTextYOffset());

    if ((dx >= 0) && (dx < getWidth() - getTextWidthOffset()) &&
	(dy >= 0) && (dy < getHeight() - getTextHeightOffset()))
      {
	textArea.setObject(this);
	return 	textArea;
      }
    else
      return super.findSensibilityArea( mouseX, mouseY);
  }

  // Edit

  public void edit(Point point)
  {
    itsSketchPad.textEditObject(this, point);
  }
}










