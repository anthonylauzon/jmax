package ircam.jmax.editors.ermes;


import java.awt.*;


/**
 * The graphic outlet on the bottom of the objects.
 */
public class ErmesObjOutlet extends ErmesObjInOutlet{

  int itsOutletNum;
  public ErmesConnectionSet itsConnectionSet;
  static int debug_count = 1;
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjOutlet(int theOutletNum, ErmesObject theOwner, int x_coord, int y_coord){
    
    super(theOwner, x_coord, y_coord);
    Rectangle aRect = itsOwner.Bounds();
    itsOutletNum = theOutletNum;
    itsConnectionSet = new ErmesConnectionSet(this);
  }
	
  //--------------------------------------------------------
  //	Paint
  //--------------------------------------------------------
  public void Paint(Graphics g){
    if(!itsOwner.itsSketchPad.itsGraphicsOn) return;
    debug_count += 1;
    if (debug_count % ErmesSketchPad.DEBUG_COUNT == 0) {
      /* System.out.println("uscito in ErmesObjOutlet.paint"); */
    }
    g.setColor(Color.black);	
    
    if(itsOwner.itsSketchPad.itsRunMode) g.fillRect(itsX+1, itsY, preferredSize().width-2, 2);
    else{
      if(selected){
	g.drawOval(itsX+1, itsY+4, preferredSize().width-2,preferredSize().height-4);
	g.drawLine(itsX+3, itsY, itsX+3, itsY+4);
      }
      else 
	if (connected) g.fillRect(itsX+1, itsY, preferredSize().width-2, 2);
	else{	// not selected, not connected
	  g.fillOval(itsX+1, itsY+1, preferredSize().width-2,preferredSize().height-4);
	  g.drawLine(itsX+3, itsY, itsX+3, itsY+1);
	}
    }
  }
  
  //--------------------------------------------------------
  //	IsInlet
  //--------------------------------------------------------
  public boolean IsInlet(){
    return false;
  }
	
  //--------------------------------------------------------
  //	GetOutletNum
  //--------------------------------------------------------
  public int GetOutletNum(){
    return itsOutletNum;
  }
  
  //--------------------------------------------------------
  //	GetConnectionSet
  //--------------------------------------------------------
  public ErmesConnectionSet GetConnectionSet(){
    return itsConnectionSet;
  }
  
  //--------------------------------------------------------
  //	GetAnchorPoint
  //--------------------------------------------------------
  public Point GetAnchorPoint(){
    return new Point(itsX+3, itsY+2);
  }
}





