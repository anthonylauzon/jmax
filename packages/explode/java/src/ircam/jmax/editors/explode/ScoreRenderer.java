package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.image.*;
import java.util.*;


/**
 * The main (default) class for a score representation.
 * It provides the music-notation background, 
 */
public class ScoreRenderer implements Renderer, ImageObserver{
  
  Container itsContainer;
  ExplodeDataModel itsExplodeDataModel;
  AdapterProvider itsAdapterProvider;

  Image itsImage;
  boolean imageReady = false;
  public EventRenderer itsEventRenderer;
  
  ScrEvent temp = new ScrEvent();
  public static final int XINTERVAL = 10;
  public static final int YINTERVAL = 3;


  /**
   * Constructor with the graphic container and the ExplodeDataModel (the data base)
   */
  public ScoreRenderer(Container theContainer, ExplodeDataModel theExplodeDataModel, AdapterProvider theAdapterProvider) {
    itsContainer = theContainer;
    itsExplodeDataModel = theExplodeDataModel; 
    itsAdapterProvider = theAdapterProvider;
    itsEventRenderer = new PartitionEventRenderer(itsAdapterProvider);
    init();
  }
  
  /**
   * make the necessary initializations for this renderer
   */
  public void init() {
    itsImage = Toolkit.getDefaultToolkit().getImage("/u/worksta/maggi/explodeTest/images/Portee1.gif");
    itsImage.getWidth(this); //call any method on the image starts loading it
  }


  /** 
   * keeps the "image ready" flag
   */
  public boolean  imageUpdate(Image img,
			      int infoflags,
			      int x,
			      int y,
			      int width,
			      int height) {
    
    if ((infoflags & ALLBITS) != 0) {
      imageReady = true;
      
      itsContainer.repaint();
      return false;
    } 
    
    return true;
    
  }
  
  /**
   * returns its (current?) event renderer
   */
  public EventRenderer getEventRenderer() {
    return itsEventRenderer;
  }

  /**
   * render (a part of) the score
   */
  public void render(Graphics g, int startEvent, int endEvent) {
    
    if (!prepareBackground(g)) return;
    
    for (int i = startEvent; i< endEvent; i++) {

      temp = itsExplodeDataModel.getEventAt(i);
      itsEventRenderer.render(temp, g);
      
    }

  }

  boolean prepareBackground(Graphics g) {
    if (!imageReady) {
      /* received a paint while loading the image... don't paint yet */
      
      g.drawString("PLEASE WAIT.....", 100, 100);
      return false;
      
    }
    else {
      
      if (!g.drawImage(itsImage, 0/*12*/, -1/*20*/, this))
	System.err.println("OH-OH something wrong: incomplete Image  ");
    }
    return true;
  }
  
}


