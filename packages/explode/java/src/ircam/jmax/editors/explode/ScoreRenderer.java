package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.image.*;
import java.util.*;


/**
 * The main class for a score representation.
 * It provides the music-notation background,
 * and uses a PartitionEventRenderer object
 * to draw the single events. 
 */
public class ScoreRenderer implements Renderer, ImageObserver{
  
  /**
   * Constructor.
   */
  public ScoreRenderer(GraphicContext theGc) 
  {  
    gc = theGc;
    
    itsEventRenderer = new PartitionEventRenderer(gc);
    tempList = new Vector();
    
    init();
  }
  

  /**
   * start loading the background image (asynchronously)
   */
  private void init() 
  {    
    itsImage = Toolkit.getDefaultToolkit().getImage("/u/worksta/maggi/projects/max/packages/explode/images/Portee1.gif");
    itsImage.getWidth(this); //call any method on the image starts loading it
  }


  /** 
   * the "image ready" flag keeps track of 
   * the complete loading of the background image
   */
  public boolean  imageUpdate(Image img,
			      int infoflags,
			      int x,
			      int y,
			      int width,
			      int height) 
  {  
    if ((infoflags & ALLBITS) != 0) 
      {
	imageReady = true;
	
	gc.getGraphicDestination().repaint();
	return false;
      } 
    else return true;
  }
  

  /**
   * returns its (current) event renderer
   */
  public EventRenderer getEventRenderer() 
  {
    return itsEventRenderer;
  }


  /**
   * render (a part of) the data in the model
   */
  public void render(Graphics g, int startEvent, int endEvent) 
  {   
    if (!prepareBackground(g)) return;

    for (int i = startEvent; i< endEvent; i++) 
      {
      
	temp = gc.getDataModel().getEventAt(i);
	itsEventRenderer.render(temp, g, gc.getSelection().isInSelection(temp));
	
      }

    firstIndexDisplayed = startEvent;
    lastIndexDisplayed = endEvent;
  }
  

  /**
   * draws the background if the entire image is in memory.
   */
  boolean prepareBackground(Graphics g) 
  {  
    if (!imageReady) 
      {
	/* received a paint while loading the image... don't paint yet */
	
	g.drawString("PLEASE WAIT.....", 100, 100);
	return false;
      }
    else 
      {
	g.setColor(Color.white);
	g.fillRect(0, 0, 1000, 1000);

	if (!g.drawImage(itsImage, 0, -1, this))
	  System.err.println("something wrong: incomplete Image  ");

	return true;
      }
  }
  
  
  /**
   * returns the event whose graphic representation contains
   * the given point.
   */
  public ScrEvent eventContaining(int x, int y) 
  {  
    ScrEvent aScrEvent;

    for (int i=firstIndexDisplayed; i<=lastIndexDisplayed; i++) 
      {      
	aScrEvent = gc.getDataModel().getEventAt(i);

	if (itsEventRenderer.contains(aScrEvent, x, y))
	  return aScrEvent;
      }
    return null;
  }


  /**
   * returns an enumeration of all the events whose graphic representation
   * intersects the given rectangle.
   */
  public Enumeration eventsIntersecting(int x, int y, int w, int h) 
  {
    ScrEvent aScrEvent;

    tempList.removeAllElements();

    for (int i=firstIndexDisplayed; i<=lastIndexDisplayed; i++) 
      {
	aScrEvent = gc.getDataModel().getEventAt(i);
	
	if (itsEventRenderer.touches(aScrEvent, x, y, w, h))
	  tempList.addElement(aScrEvent);
      }
    return tempList.elements();
  }


  //------------------  Fields
  GraphicContext gc;

  Container itsContainer;
  ExplodeDataModel itsExplodeDataModel;
  SelectionHandler itsSelection;

  Image itsImage;
  boolean imageReady = false;
  public EventRenderer itsEventRenderer;
  
  ScrEvent temp = new ScrEvent();
  public static final int XINTERVAL = 10;
  public static final int YINTERVAL = 3;

  int firstIndexDisplayed = 0;
  int lastIndexDisplayed = 0;

  private Vector tempList;

}



