package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import ircam.jmax.fts.*;
import ircam.jmax.*;

/**
 * The "integer box" graphic object.
 */
class ErmesObjInt extends ErmesObject implements FtsPropertyHandler, KeyEventClient {

  // (fd) {
  int state;
  StringBuffer currentText;
  // } (fd)


  int itsInteger = 0;
  
  static ErmesObjIntegerDialog itsIntegerDialog = null;
  
  int DEFAULT_WIDTH = 40;
  int DEFAULT_HEIGHT = 15;
  int DEFAULT_VISIBLE_DIGIT = 3;
  int itsStartingY, itsFirstY;
  boolean firstClick = true;
  Dimension preferredSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);
  Dimension minimumSize = new Dimension(DEFAULT_WIDTH,DEFAULT_HEIGHT);

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public ErmesObjInt(ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    super(theSketchPad, theFtsObject);

    // (fd) {
    state = 0;
    currentText = new StringBuffer();
    // } (fd)
  }
	
  //--------------------------------------------------------
  // init
  //--------------------------------------------------------

  public void Init()
  {
    super.Init();
    itsFtsObject.watch("value", this);

    itsInteger = ((Integer)itsFtsObject.get("value")).intValue();
    DEFAULT_HEIGHT = itsFontMetrics.getHeight();
    DEFAULT_WIDTH = itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..");

    if(getItsHeight()<DEFAULT_HEIGHT+4) {
      preferredSize.height = DEFAULT_HEIGHT+4;
      resizeBy(0, getPreferredSize().height - getItsHeight());
    }
    if(getItsWidth()<DEFAULT_WIDTH+17){
      preferredSize.width = DEFAULT_WIDTH+17;
      setItsWidth(preferredSize.width);
    }
   }
	
  //--------------------------------------------------------
  // propertyChanged
  // callback function from the associated FtsObject in FTS
  //--------------------------------------------------------
  public void propertyChanged(FtsObject obj, String name, Object value) {
    
    int temp = ((Integer) value).intValue();
    
    if (itsInteger != temp) {
      itsInteger = temp;

      Graphics g = itsSketchPad.getGraphics();
      Paint_specific(g);
      g.dispose();
    }
  }

  public void FromDialogValueChanged(int theInt){
    itsInteger = theInt;

    itsFtsObject.put("value", theInt);
    DoublePaint();
  }
  
  void ResizeToNewFont(Font theFont){
      resizeBy(17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..")-getItsWidth(),itsFontMetrics.getHeight()+4-getItsHeight());
  }
  
  public void ResizeToText(int theDeltaX, int theDeltaY){
    int aWidth = getItsWidth()+theDeltaX;
    int aHeight = getItsHeight()+theDeltaY;
    
    if((aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0"))&&(aHeight<itsFontMetrics.getHeight()+4)) {
      aWidth = getMinimumSize().width;
      aHeight = getMinimumSize().height;
    }else{
      if(aWidth<aHeight/2+17+itsFontMetrics.stringWidth("0"))
	aWidth = aHeight/2+17+itsFontMetrics.stringWidth("0");
      
      if(aHeight<itsFontMetrics.getHeight()+4) aHeight = itsFontMetrics.getHeight()+4;
    }
    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
  }

  public boolean canResizeBy(int theDeltaX, int theDeltaY){
    if((getItsWidth()+theDeltaX < getItsHeight()/2 +17+itsFontMetrics.stringWidth("0"))||(getItsHeight()+theDeltaY<itsFontMetrics.getHeight() + 4))
      return false;
    else return true;
  }
  
  public void RestoreDimensions(){
    int aHeight, aWidth;
    aHeight = itsFontMetrics.getHeight()+4;
    aWidth = 17+itsFontMetrics.stringWidth("0")*DEFAULT_VISIBLE_DIGIT+itsFontMetrics.stringWidth("..");

    resizeBy(aWidth-getItsWidth(), aHeight-getItsHeight());
    // itsSketchPad.repaint(); // BARBOGIO
  }
	
  //--------------------------------------------------------
  //  mouseDown
  //--------------------------------------------------------
  public void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown())
      {
	// (fd) {
	if (!evt.isControlDown()) //"pure" run mode
	{
	  state = 1;
	  itsSketchPad.itsSketchWindow.setKeyEventClient( this);
	  // } (fd)
	}

	itsFirstY = y;

	if (firstClick) 
	  {
	    itsStartingY = itsInteger;
	    firstClick = false;
	  }
	else 
	  itsStartingY = itsInteger;

	itsFtsObject.put("value", new Integer(itsInteger));
	
      }
    else 
      itsSketchPad.ClickOnObject(this, evt, x, y);
  }
  
  public boolean inspectorAlreadyOpen() {
    return (itsIntegerDialog != null && itsIntegerDialog.isVisible());
  }

  public void openInspector() {
    Point aPoint = GetSketchWindow().getLocation();
    if (itsIntegerDialog == null) itsIntegerDialog = new ErmesObjIntegerDialog(MaxWindowManager.getTopFrame());
    itsIntegerDialog.setLocation(aPoint.x + getItsX(),aPoint.y + getItsY());
    itsIntegerDialog.ReInit(String.valueOf(itsInteger), this, itsSketchPad.GetSketchWindow());
    //itsIntegerDialog.setVisible(true);
  }

	
  //--------------------------------------------------------
  //  mouseUp
  //--------------------------------------------------------
  public boolean MouseUp(MouseEvent evt,int x, int y) 
  {
    if(itsSketchPad.itsRunMode || evt.isControlDown())
      {
	Fts.sync();

	DoublePaint();
	return true;
      }
    else
      return super.MouseUp(evt, x, y);
  }
	
  //--------------------------------------------------------
  // mouseDrag
  //--------------------------------------------------------
  public boolean MouseDrag_specific(MouseEvent evt,int x, int y) 
  {
    if ( itsSketchPad.itsRunMode || evt.isControlDown() )
      {
	// (fd)
	if (!evt.isControlDown())
	  state = 2;

	itsInteger = itsStartingY + (itsFirstY - y);
	itsFtsObject.put( "value", new Integer(itsInteger));
	DoublePaint();
	
	return true;
      }
    else
      return false;
  }

  public boolean NeedPropertyHandler(){
    return true;
  }
  
  public boolean isUIController() {
    return true;
  }

  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  public void Paint_specific( Graphics g) 
    {
      int x = getItsX();
      int xp1 = x + 1;
      int y = getItsY();
      int h = getItsHeight();
      int hd2 = h / 2;
      int hm1 = h - 1;
      int w = getItsWidth();
      int wm1 = w - 1;

      if (g == null) 
	return;

    // (fd) Nice ! Simple ! Fast ! ...
    // draw the white area
    //int xWhitePoints[] = {getItsX()+3, getItsX()+getItsWidth()-3, getItsX()+getItsWidth()-3, getItsX()+3, getItsX()+getItsHeight()/2+3};
    //int yWhitePoints[] = {getItsY()+1, getItsY()+1, getItsY()+getItsHeight()-1,getItsY()+getItsHeight()-1, getItsY()+getItsHeight()/2};
    //g.fillPolygon(xWhitePoints, yWhitePoints, 5);

      // (1) Fill the background
      if ( !itsSelected) 
	g.setColor( Color.white);
      else
	g.setColor( itsUISelectedColor);
      g.fillRect( x, y, wm1 , hm1);
    
      // (2) Draw the outline
      g.setColor( Color.black);
      g.drawRect( x, y, wm1, hm1);

      // (3) Draw or fill the triangle
      if ( state != 0 )
	{
	  int xPoints[] = { xp1, xp1 + hd2, xp1};
	  int yPoints[] = { y, y + hd2, y + hm1};
	  g.fillPolygon( xPoints, yPoints, 3);
	}
      else
	{
	  g.drawLine( xp1, y, xp1 + hd2, y + hd2);
	  g.drawLine( xp1 + hd2, y + hd2, xp1, y + hm1);
	}
    
      // (4) Draw the value
      String aString;

      if (state != 3) 
	aString = GetVisibleString(String.valueOf(itsInteger));

      else
	{
	  aString = currentText.toString();
	  g.setColor( Color.red);
	}

      g.setFont( getFont());
      g.drawString( aString, x + hd2 + 5, y + itsFontMetrics.getAscent() + (h - itsFontMetrics.getHeight())/2 + 1);
    
      // (5) Draw the dragbox (?)
      if ( !itsSketchPad.itsRunMode)
	g.fillRect( x + w - DRAG_DIMENSION, y + h - DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
  }
	
  String GetVisibleString(String theString){
    String aString = theString;
    String aString2 = "..";
    int aStringLength = theString.length();
    int aCurrentSpace = getItsWidth()-(getItsHeight()/2+5)-5;
    int aStringWidth = itsFontMetrics.stringWidth(aString);
    if(aStringWidth<aCurrentSpace) return aString;
    while((aCurrentSpace<=aStringWidth)&&(aString.length()>0)){
      aString = aString.substring(0,aString.length()-1);
      aStringWidth = itsFontMetrics.stringWidth(aString);
    }
    if((aStringWidth+itsFontMetrics.stringWidth("..") >= aCurrentSpace)&&(aString.length()>0))
      aString = aString.substring(0,aString.length()-1);
    aString =  aString + aString2;
    return aString;
  }

  //--------------------------------------------------------
  // minimumSize
  //--------------------------------------------------------
  public Dimension getMinimumSize() {
    if(getItsHeight()==0 || getItsWidth() == 0) return minimumSize;
    else return new Dimension(getItsHeight()/2+13+itsFontMetrics.stringWidth("0"),itsFontMetrics.getHeight()+4); 
  }

  //--------------------------------------------------------
  // preferredSize
  //--------------------------------------------------------
  public Dimension getPreferredSize() {
    return preferredSize;
  }

  // (fd) {
  public void keyPressed( KeyEvent e) 
  {
    // DEBUG
    //System.err.println( this.getClass().getName() + "( " + e.getKeyCode() + " )");

    if (e.getKeyCode() == 17) return; //avoid CTRL as valid (independent) key

    state = 3;

    if ( !e.isControlDown() && !e.isMetaDown() && !e.isShiftDown())
      {
	int c = e.getKeyChar();

	if ( c >= '0' && c <= '9')
	  currentText.append( (char)c);
	else if ( c == ircam.jmax.utils.Platform.ENTER_KEY
		  || c == ircam.jmax.utils.Platform.RETURN_KEY )
	  {
	    try
	      {
		int value = Integer.parseInt( currentText.toString());

		itsInteger = value;
		itsFtsObject.put( "value", new Integer( value));
	      }
	    catch ( NumberFormatException exception)
	      {
	      }
	    currentText.setLength(0);
	    state = 0;
	  }
	else if ( ( c == ircam.jmax.utils.Platform.DELETE_KEY)
	     || ( c == ircam.jmax.utils.Platform.BACKSPACE_KEY) )
	  {
	    int l = currentText.length();

	    l = ( l > 0 ) ? l - 1 : 0;

	    currentText.setLength( l);
	  }

	DoublePaint();

      }
  }

  public void keyReleased( KeyEvent e) 
  {
  }

  public void keyTyped( KeyEvent e)
  {
  }

  public void keyInputGained()
    {
      DoublePaint();
    }

  public void keyInputLost()
    {
      state = 0;
      currentText.setLength(0);
      DoublePaint();
    }
  // } (fd)
}

