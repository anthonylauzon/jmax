
package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;
import ircam.jmax.MaxApplication;

import java.awt.*;
import java.awt.image.ImageObserver;
import java.io.File;


/**
 * The background layer of a score. It loads the gif file for the 
 * piano roll representation */
public class ScoreBackground implements Layer, ImageObserver {
  
  /** Constructor */
  public ScoreBackground ( ExplodeGraphicContext theGc)
  {
    super();
    
    gc = theGc;
    
    //
    // start loading the background image (asynchronously)
    //

    String fs = File.separator;
    String path = MaxApplication.getProperty("root")+fs+"packages/explode/images"+fs;
    
    if (itsImage == null) 
      {
	itsImage = Toolkit.getDefaultToolkit().getImage(path+"pianoroll2tr.gif");
	itsImage.getWidth(this); //call any method on the image starts loading it
      }
    
  }




  /** 
   * the "image ready" flag keeps track of 
   * the complete loading of the background image
   * (insert a time out test?) */
  public boolean  imageUpdate( Image img,
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
   * Layer interface. Draw the background */
  public void render( Graphics g, int order)
  {
    if (!imageReady) 
      {
	/* received a paint while loading the image... don't paint yet */
	
	g.drawString("PLEASE WAIT.....", 100, 100);
	return;
      }
    else 
      {
	g.setColor(Color.white);
	g.fillRect(0, 0, 1000, 1000);

	if (!g.drawImage(itsImage, 0, 18, this))
	  System.err.println("something wrong: incomplete Image  ");

	// paint the vertical grid
	int MIN_GRID = 6;
	int MAX_GRID = 50;

	int windowTime = (int) (gc.getGraphicDestination().getSize().width / gc.getAdapter().getXZoom());
	int timeStep;

	
	timeStep = findBestTimeStep(windowTime);

	g.setColor(Color.lightGray);

	int xPosition;
	int snappedTime;
	for (int i=gc.getLogicalTime()+timeStep; i<gc.getLogicalTime()+windowTime; i+=timeStep) 
	  {
	    snappedTime = (i/timeStep)*timeStep;
	    xPosition = (int) ((snappedTime-gc.getLogicalTime()) * gc.getAdapter().getXZoom());

	    g.drawLine(xPosition, 20, xPosition, 400);
	    g.drawString(""+snappedTime, xPosition-20, 15);
	  }
      }
  }
  
  /**
   * Layer interface. */
  public void render(Graphics g, Rectangle r, int order)
  {
    render(g, order);
  }

  public ObjectRenderer getObjectRenderer()
  {
    return null; // no events in this layer!
  }
  
  /**
   * utility function: find a "good" time step for vertical subdivisions
   * given a window time size.
   * The politic is to find 
   * 1) at least 5 divisions 
   * 2) if possible, a power of 10  */  
  private int findBestTimeStep(int windowTime) 
  {
    
    // find a good time interval between two grid
    
    int pow = 1;
      
    while (windowTime/pow>0) 
      {
	pow *= 10;
      }

    pow = pow/10;

    if (windowTime/pow < 5) pow = pow/5;
    if (pow == 0) return 1;
    return pow;
  }


  //--- Fields
  ExplodeGraphicContext gc;
  static Image itsImage;
  static boolean imageReady = false;
}


