package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;


//
// The base class of the ermes objects which are user-editable (ErmesObjMessage, ErmesObjExternal, ErmesObjPatcher).
//

abstract public class ErmesObjEditableObject extends ErmesObject implements FtsInletsListener, FtsOutletsListener
{
  boolean editing = false;
  TextRenderer textRenderer;

  ErmesObjEditableObject( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super( theSketchPad, theFtsObject);

    textRenderer = new TextRenderer(this);
    textRenderer.update();

    if (getWidth() == -1)
      setWidth( getFontMetrics().stringWidth( "pack 1 2 3") + 2*getTextXOffset());
  }

  public void updateDimensions()
  {
    textRenderer.update();
    super.setHeight(textRenderer.getPreferredSize().height + getTextHeightOffset());
  }

  public void updateDimensionsNoConnections()
  {
    textRenderer.update();
    super.setHeightNoConnections(textRenderer.getPreferredSize().height + getTextHeightOffset());
  }

  // redefined from base class

  public  void setWidth(int w) 
  {
    // textRenderer.update();

    if (textRenderer.canResizeWidthTo(w - getTextWidthOffset()))
      {
	super.setWidth(w);
	super.setHeight(textRenderer.getPreferredSize().height + getTextHeightOffset());
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
    textRenderer.setFont(f);
    textRenderer.update();
    super.setHeight(textRenderer.getPreferredSize().height + getTextHeightOffset());
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

  static Container ic = new Panel();

  public void DrawParsedString(Graphics g) 
  {
    if (editing)
      return;

    textRenderer.setBackground(getTextBackground());

    SwingUtilities.paintComponent(g,
				  textRenderer,
				  ic,
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
}










