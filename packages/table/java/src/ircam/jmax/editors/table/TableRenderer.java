package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.File;
import ircam.jmax.MaxApplication;

/**
 * The main class for a table representation.
 * This Render acts also like a Layer (the foreground layer of the table)
 */
public class TableRenderer extends AbstractRenderer implements Layer{
  
  /**
   * Constructor.
   */
  public TableRenderer(TableGraphicContext theGc) 
  {  
    gc = theGc;
    setMode(HOLLOW);//the default
    addLayer(this);
    addLayer( new TopLayer( theGc));
  }


  /**
   * This method is the substitution of the ObjectRenderer mechanism
   * for editors based on arrays of (int)values instead of objects. 
   * This mechanism can be represented as an ObjectRenderer that draws Points, but
   * this means building a Point each single value painting...*/
  private void render(Graphics g, int x, int y)
  {
    g.setColor(Color.white);
    int zero = gc.getAdapter().getY(0);
    int width = (int)( gc.getAdapter().getXZoom());
    int height;

    if (width < 1) width = 1;
    
    g.fillRect(x, 0, width, zero);
    g.fillRect(x, zero+1, width, gc.getGraphicDestination().getSize().height);

    g.setColor(Color.black);
    
    if (itsMode == SOLID)
      {
	int startY = (y <= zero)?y:zero; //remember: y are (graphically) INVERTED!  
	height = Math.abs(zero-y);

	g.fillRect(x, startY, 
		   width,
		   height);
      }
    else
      {
	height = (int) gc.getAdapter().getYZoom();
	if (height == 0) height = 1;

	g.fillRect(x,  y, width, height);
      }
  
  }

  /**
   * Renders a single value in the table */
  public void renderPoint(Graphics g, int index)
  {
    if (index < 0 || index >= gc.getDataModel().getSize()) return;
    int val = gc.getDataModel().getValue(index);

    render(g, gc.getAdapter().getX(index), gc.getAdapter().getY(val));
  }

  /**
   * From the Layer interface: renders the content of the table (except the
   * zero line and the surrounding gray, that are rendered by the TopLayer */
  public void render(Graphics g, int order)
  {
    g.setColor(Color.black);

    for (int i = 0; i< gc.getDataModel().getSize(); i++)
      {
	renderPoint(g, i);
      }
  }

  /** 
   * Layer interface */
  public void render(Graphics g, Rectangle r, int order)
  {

    int startIndex = gc.getAdapter().getInvX(r.x);
    int endIndex = gc.getAdapter().getInvX(r.x + r.width);
    
    for (int i = startIndex; i<= endIndex; i++)
      {
	renderPoint(g, i);
      }
  }



  /**
   * set the HOLLOW or SOLID mode */
  public void setMode(int mode)
  {
    itsMode = mode;
  }

  
  //--- Fields

  TableGraphicContext gc;
  int itsMode;

  TopLayer itsTopLayer;
  public final static int HOLLOW = 0;
  public final static int SOLID = 1;
}

