package ircam.jmax.editors.ermes;


import java.awt.*;


/**
 * The graphic outlet on the bottom of the objects.
 */
public class ErmesObjOutlet extends ErmesObjInOutlet{

  int itsOutletNum;
  static int debug_count = 1;
  
  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjOutlet(int theOutletNum, ErmesObject theOwner, int x_coord, int y_coord){
    
    super(theOwner, x_coord, y_coord);
    Rectangle aRect = itsOwner.Bounds();
    itsOutletNum = theOutletNum;
  }
	
  //--------------------------------------------------------
  //	Paint
  //--------------------------------------------------------
  public void Paint_specific(Graphics g){
    if(!itsOwner.itsSketchPad.itsGraphicsOn) return;
    g.setColor(Color.black);	
    
    if(itsOwner.itsSketchPad.itsRunMode) g.fillRect(itsX+1, itsY, getPreferredSize().width-2, 2);
    else{
      if(selected){
	g.drawOval(itsX+1, itsY+4, getPreferredSize().width-2,getPreferredSize().height-4);
	g.drawLine(itsX+3, itsY, itsX+3, itsY+4);
      }
      else 
	/*if (connected)*/ g.fillRect(itsX+1, itsY, getPreferredSize().width-2, 2);
      //else{	// not selected, not connected
      //  g.fillOval(itsX+1, itsY+1, getPreferredSize().width-2,getPreferredSize().height-4);
      //  g.drawLine(itsX+3, itsY, itsX+3, itsY+1);
      //}
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
  //	updateAnchorPoint
  //--------------------------------------------------------
  void updateAnchorPoint(){
    itsAnchorPoint.setLocation(itsX+3, itsY+2);
  }
}






