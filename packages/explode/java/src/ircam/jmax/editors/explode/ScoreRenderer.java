package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.image.*;
import java.util.*;
import java.io.File;
import ircam.jmax.MaxApplication;

/**
 * The main class for a score representation.
 * It provides the support for piano-roll editing,
 * using a background layer and a foreground.
 * The grid is rendered in the ScoreBackground
 * The events are painted by the ScoreForeground.
 */
public class ScoreRenderer extends AbstractRenderer{
  
  /**
   * Constructor.
   */
  public ScoreRenderer(ExplodeGraphicContext theGc) 
  {  
    gc = theGc;
    gc.setAdapter(new PartitionAdapter(gc));
    {//-- prepares the parameters for the adapter
      gc.getAdapter().setXZoom(20);
      gc.getAdapter().setYZoom(300);
      gc.getAdapter().setYInvertion(true);
      gc.getAdapter().setYTransposition(136);
    }

    tempList = new Vector();

    itsLayers = new Vector();

    itsForegroundLayer = new ScoreForeground(gc);

    itsLayers.addElement(new ScoreBackground(gc));
    //itsLayers.addElement(new TestBackground(gc));
    itsLayers.addElement(itsForegroundLayer);
  }
  


  /**
   * render all the objects in the given rectangle */
  public void render(Graphics g, Rectangle r)
  {
    Layer aLayer;

    for (int i = 0; i< itsLayers.size(); i++)
      {
	aLayer = (Layer) itsLayers.elementAt(i);
	aLayer.render(g, r, i);
      }
  }

  /**
   * returns its (current) event renderer
   */
  public ObjectRenderer getObjectRenderer() 
  {
    return itsForegroundLayer.getObjectRenderer();
  }


  
  /**
   * returns the events whose graphic representation contains
   * the given point.
   */
  public Enumeration objectsContaining(int x, int y) 
  {  
    ScrEvent aScrEvent;

    tempList.removeAllElements();

    int startTime = gc.getAdapter().getInvX(0);
    int endTime = gc.getAdapter().getInvX(gc.getGraphicDestination().getSize().width);

    for (int i=gc.getDataModel().indexOfFirstEventEndingAfter(startTime); i<=gc.getDataModel().indexOfLastEventStartingBefore(endTime); i++) 
      {      
	aScrEvent = gc.getDataModel().getEventAt(i);

	if (getObjectRenderer().contains(aScrEvent, x, y))
	  tempList.addElement(aScrEvent);
      }

    return tempList.elements();
  }

  /**
   * returns the first event containg the given point */
  public Object firstObjectContaining(int x, int y)
  {
    ScrEvent aScrEvent;

    int startTime = gc.getAdapter().getInvX(0);
    int endTime = gc.getAdapter().getInvX(gc.getGraphicDestination().getSize().width);

    for (int i=gc.getDataModel().indexOfFirstEventEndingAfter(startTime); i<=gc.getDataModel().indexOfLastEventStartingBefore(endTime); i++) 
      
      {      
	aScrEvent = gc.getDataModel().getEventAt(i);

	if (getObjectRenderer().contains(aScrEvent, x, y))
	  return aScrEvent;
      }

    return null;
  }

  /**
   * returns an enumeration of all the events whose graphic representation
   * intersects the given rectangle.
   */
  public Enumeration objectsIntersecting(int x, int y, int w, int h) 
  {
    ScrEvent aScrEvent;

    tempList.removeAllElements();
    int startTime = gc.getAdapter().getInvX(x);
    int endTime = gc.getAdapter().getInvX(x+w);

    for (int i=gc.getDataModel().indexOfFirstEventEndingAfter(startTime); i<=gc.getDataModel().indexOfLastEventStartingBefore(endTime); i++) 
      {
	aScrEvent = gc.getDataModel().getEventAt(i);

	if (getObjectRenderer().touches(aScrEvent, x, y, w, h))
	  {
	    tempList.addElement(aScrEvent);
	  }
      }
    return tempList.elements();
  }


  //------------------  Fields
  ExplodeGraphicContext gc;

  ExplodeDataModel itsExplodeDataModel;

  ScoreForeground itsForegroundLayer;
  
  public static final int XINTERVAL = 10;
  public static final int YINTERVAL = 3;

  private Vector tempList;
}



