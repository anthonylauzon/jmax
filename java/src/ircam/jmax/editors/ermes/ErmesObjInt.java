package ircam.jmax.editors.ermes;

import java.awt.*;
import java.util.*;
import ircam.jmax.fts.*;

/**
 * The "integer box" graphic object.
 */
class ErmesObjInt extends ErmesObject {

	ErmesObjInlet itsInlet;
	ErmesObjOutlet itsOutlet;
	int itsInteger = 0;
	static final int TRUST=10;		//how many transmitted values we trust?
	int transmission_buffer[];
	int transmission_index = 0;
	int receiving_index = 0;
	int last_value = 0;
	
	int DEFAULT_WIDTH = 40;
	int DEFAULT_HEIGHT = 20;
	int itsStartingY, itsFirstY;
	boolean firstClick = true;
	Dimension preferredSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
	//--------------------------------------------------------
	// CONSTRUCTOR
    //--------------------------------------------------------
	public ErmesObjInt(){
		super();
		transmission_buffer = new int[TRUST];
		//setLayout(null);
	}
	
	// WARNING!! this Init must set the integer value coming in theString?
	//--------------------------------------------------------
	// init
    //--------------------------------------------------------
	public boolean Init(ErmesSketchPad theSketchPad, int x, int y, String theString) {
		
		DEFAULT_HEIGHT = theSketchPad.getFontMetrics(theSketchPad.sketchFont).getHeight();
		DEFAULT_WIDTH = theSketchPad.getFontMetrics(theSketchPad.sketchFont).stringWidth("0");
		preferredSize.height = DEFAULT_HEIGHT+10;
		preferredSize.width = DEFAULT_WIDTH+30;
		super.Init(theSketchPad, x, y, theString);
		return true;
	}
	
	//--------------------------------------------------------
	// makeFtsObject, redefineFtsObject
    //--------------------------------------------------------

  public void makeFtsObject()
  {
    itsFtsObject = FtsObject.makeFtsObject(itsFtsPatcher, "gint", (Vector) null);    
  }

  public void redefineFtsObject()
  {
    // gint do not redefine themselves
  }
  
	//--------------------------------------------------------
	// FtsValueChanged
	// callback function from the associated FtsObject in FTS
    //--------------------------------------------------------
	protected void FtsValueChanged(Object value) {
		int temp = ((Integer) value).intValue();

		last_value = temp;
		if (receiving_index < transmission_index && temp == transmission_buffer[receiving_index]) {
			//Ok, the buffer is working, the value is the one we sent...
			receiving_index += 1;
			if (receiving_index == transmission_index) {
				receiving_index = 0;
				transmission_index = 0;
			}
			return;
		}
		else {
			// we're receiving other values
			transmission_index = 0;
			receiving_index = 0;
			itsInteger = temp;
			if (ChangeRectInt()) itsSketchPad.repaint();
			else DoublePaint();
		}		
	}
	
	public boolean ChangeRectInt(){
		String aString; 
		boolean aChange = false;
		aString = String.valueOf(itsInteger);
		int lenght = itsFontMetrics.stringWidth(aString);
		itsSketchPad.RemoveElementRgn(this);
		if (lenght >= currentRect.width*2/3-2-5){
			while(lenght >= currentRect.width*2/3-2-5){
				Resize1(currentRect.width+15, currentRect.height);
				aChange = true;
			}
		}
		else {
			while(lenght < currentRect.width*2/3-2-/*5*/20){
				Resize1(currentRect.width-15, currentRect.height);
				aChange = true;
			}
		}
		itsSketchPad.SaveOneElementRgn(this);
		if(aChange) {
			aChange = false;
			return true;
		}
		else return false;
	}
	
	void ResizeToNewFont(Font theFont){
		if(!itsResized){
			Resize(itsFontMetrics.stringWidth(String.valueOf(itsInteger)) + 30 - currentRect.width,
					itsFontMetrics.getHeight() + 10 - currentRect.height);
		}
		else ResizeToText(0,0);
	}
	
	public void ResizeToText(int theDeltaX, int theDeltaY){
		int aWidth = currentRect.width+theDeltaX;
		int aHeight = currentRect.height+theDeltaY;
		if(aWidth<itsFontMetrics.stringWidth(String.valueOf(itsInteger)) + 30) aWidth = itsFontMetrics.stringWidth(String.valueOf(itsInteger)) + 30;
		if(aHeight<itsFontMetrics.getHeight() + 10) aHeight = itsFontMetrics.getHeight() + 10;
		Resize(aWidth-currentRect.width, aHeight-currentRect.height);
	}
	
	public boolean IsResizeTextCompat(int theDeltaX, int theDeltaY){
		if((currentRect.width+theDeltaX < itsFontMetrics.stringWidth(String.valueOf(itsInteger)) + 30)||
		   (currentRect.height+theDeltaY<itsFontMetrics.getHeight() + 10))
		   return false;
		else return true;
	}
	
	public void RestoreDimensions(){
		itsResized = false;
		itsSketchPad.RemoveElementRgn(this);
		Resize(itsFontMetrics.stringWidth(String.valueOf(itsInteger)) + 30 - currentRect.width,
			   itsFontMetrics.getHeight() + 10 - currentRect.height);
		itsSketchPad.SaveOneElementRgn(this);
		itsSketchPad.repaint();
	}
	
	//--------------------------------------------------------
	//  mouseDown
    //--------------------------------------------------------
	public boolean MouseDown_specific(Event evt,int x, int y) {
		if (itsSketchPad.itsRunMode) {
			itsFirstY = y;
			if (firstClick) {
				itsStartingY = itsInteger;
				firstClick = false;
			}
			else itsStartingY = itsInteger;
			itsFtsObject.putProperty("value", new Integer(itsInteger));
			Trust(itsInteger);
		}
		else 
			itsSketchPad.ClickOnObject(this, evt, x, y);
		return true;
	}
	
	void Trust (int theInt) {
		if (transmission_index == TRUST) {
			//we sent more then TRUST values without acknowledge...
			//write a message? FTS, are you there?
			itsInteger = last_value;
			DoublePaint();
		}
		else {
			transmission_buffer[transmission_index++] = theInt;
		}
	}
	
	//--------------------------------------------------------
	//  mouseUp
    //--------------------------------------------------------
	public boolean MouseUp(Event evt,int x, int y) {
		if (itsSketchPad.itsRunMode) {
			itsStartingY = itsInteger;
			firstClick = true;
			
			String aString = String.valueOf(itsInteger);
			int lenght = itsFontMetrics.stringWidth(aString);
			if(!itsResized){
				if(lenght<currentRect.width*2/3-2-20){
					itsSketchPad.RemoveElementRgn(this);
					while(lenght</*currentRect.width-16*/currentRect.width*2/3-2-20){
						Resize1(currentRect.width-10, currentRect.height);
					}
					itsSketchPad.SaveOneElementRgn(this);
					itsSketchPad.repaint();//???????
				}
			}
			return true;
		}
		else return super.MouseUp(evt,x,y);
	}
	
	//--------------------------------------------------------
	// mouseDrag
    //--------------------------------------------------------
	public boolean MouseDrag(Event evt,int x, int y) {

		if(itsSketchPad.itsRunMode){
			itsInteger = itsStartingY+(itsFirstY-y);
			itsFtsObject.putProperty("value", new Integer(itsInteger));
			//((FtsInteger) itsFtsActive).setValue(itsInteger);	ENZOOO
			DoublePaint();
			Trust(itsInteger);
			return true;
		}
		else return false;
	}

	//--------------------------------------------------------
	// ConnectionRequested
    //--------------------------------------------------------
	public boolean ConnectionRequested(ErmesObjInOutlet theRequester){
		if (!theRequester.IsInlet())
			return (itsSketchPad.OutletConnect(this, theRequester));
		else return (itsSketchPad.InletConnect(this, theRequester)); 
	}
	
	//--------------------------------------------------------
	// ConnectionAbort
    //--------------------------------------------------------
	public boolean ConnectionAbort(ErmesObjInOutlet theRequester){
		theRequester.ChangeState(false, theRequester.connected);
		itsSketchPad.ResetConnect();
		return true;
	}
	
	//--------------------------------------------------------
	// paint
    //--------------------------------------------------------
	public void Paint_specific(Graphics g) {
	  if(!itsSelected) g.setColor(itsUINormalColor/*Color.lightGray*/);
	  else g.setColor(itsUISelectedColor/*Color.gray*/);
	  
	  //draw the box
	  g.fillRect(itsX+1,itsY+1, currentRect.width-2,  currentRect.height-2);
	  g.fill3DRect(itsX+2,itsY+2, currentRect.width-4,  currentRect.height-4, true);
		
	  //draw the white area				
	  g.setColor(Color.white);
	  g.fillRect(itsX+4, itsY+4, currentRect.width-8, currentRect.height-8);
		
	  //draw the outline
	  g.setColor(Color.black);
	  g.drawRect(itsX+0, itsY+0, currentRect.width-1, currentRect.height-1);
		
	  //draw the triangle
	  g.drawLine(itsX+4,itsY+4,itsX+currentRect.width/4,itsY+currentRect.height/2);
	  g.drawLine(itsX+currentRect.width/4,itsY+currentRect.height/2,itsX+ 4, itsY+currentRect.height-4);
		
	  //draw the value
	  String aString = String.valueOf(itsInteger);
	  g.setFont(itsFont);
	  g.drawString(aString, itsX+currentRect.width/3+2,itsY+itsFontMetrics.getAscent()+(currentRect.height-itsFontMetrics.getHeight())/2);
	  //draw the dragbox
	  g.setColor(Color.black);
	  g.fillRect(itsX+currentRect.width-DRAG_DIMENSION,itsY+currentRect.height-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
	}
	
    //--------------------------------------------------------
	// minimumSize
    //--------------------------------------------------------
    public Dimension minimumSize() {
        return preferredSize(); 
    }

  	//--------------------------------------------------------
	// preferredSize
    //--------------------------------------------------------
    public Dimension preferredSize() {
        return preferredSize;
    }

}
