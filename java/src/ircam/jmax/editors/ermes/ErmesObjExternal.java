package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.mda.*;

/**
 * The generic "extern" object in ermes. (example: adc1~) 
 */
public class ErmesObjExternal extends ErmesObjEditableObject implements FtsPropertyHandler{
  int isError = -1;			// cache of the error property, to speed up paint
  public boolean iAmPatcher = false;
  private String itsBackupText = new String();
  public static final int WHITE_OFFSET = 6;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjExternal(){
    super();
  }
	
  public boolean AreYouPatcher() {
    return iAmPatcher;
  }
  
  protected int getWhiteOffset() {
    return WHITE_OFFSET;
  }

  public boolean Init( ErmesSketchPad theSketchPad, FtsObject theFtsObject)
  {
    // Added by MDC; get the correct String from the object, and then call super
    // It is needed because ErmesObjExternal and ErmesObjMessage use different methods
    // to get the string from the object.

    itsArgs = theFtsObject.getDescription().trim();

    super.Init( theSketchPad, theFtsObject);

    if ( theFtsObject instanceof FtsContainerObject)
      this.YouArePatcher( true);

    ParseText( itsArgs);
    if ( !canResizeBy( 0, 0))
      RestoreDimensions( false);

    return true;
  }

  public void propertyChanged(FtsObject obj, String name, Object value) {
    //handle the "error" property, the only one we're listening at
    // call super for the others

    if (name.equals("error"))
      {
	if ((value != null) && (value instanceof Integer))
	  {
	    isError = ((Integer)value).intValue();
	    DoublePaint();
	  }
      }
    else
      super.propertyChanged(obj, name, value);
  }

  public void YouArePatcher(boolean what) {
    iAmPatcher = what;
  }

  /* Inspector */

  public boolean inspectorAlreadyOpen() {
    return (ErmesPatcherInspector.isOpen() && ErmesPatcherInspector.getInspectedObject() == itsFtsObject);
  }

  public void openInspector() {
    if (iAmPatcher)
      ErmesPatcherInspector.inspect((FtsContainerObject) itsFtsObject);
  }

  //--------------------------------------------------------
  // makeFtsObject and redefineFtsObject() 
  //--------------------------------------------------------

  public void makeFtsObject()
  {
    try
      {
	if (itsArgs == null) 
	  itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, "__void");
	else
	  itsFtsObject = Fts.makeFtsObject(itsFtsPatcher, itsArgs);

	itsFtsObject.watch("error", this);
	isError = -1;
      }
    catch (FtsException e)
      {
	System.out.println("Cannot create object: " + itsArgs);
      }

    if (itsFtsObject instanceof FtsContainerObject)
      YouArePatcher(true);
  }

  public void redefineFtsObject()
  {
    try
      {
	itsFtsObject = Fts.redefineFtsObject(itsFtsObject, itsArgs);

	itsFtsObject.watch("ins", this);
	itsFtsObject.watch("outs", this);
	itsFtsObject.watch("error", this);
	isError = -1;
      }
    catch (FtsException e)
      {
	System.out.println("Error in redefining object, action cancelled");
	restoreText();
	ParseText(itsArgs);
      }

    this.YouArePatcher(itsFtsObject instanceof FtsContainerObject);
  }
  
  //--------------------------------------------------------
  // mouseDown
  //--------------------------------------------------------
  
  public boolean MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    if ( evt.getClickCount() > 1 ) 
      {
	MaxData data = null;

	// New implementation: try to get the "data"
	// property, and if not null edit it.

	itsFtsObject.ask("data");
	Fts.sync();
	data = (MaxData) itsFtsObject.get("data");	

	/* HACK !! */

	if ((data != null) && (data instanceof FtsRemoteData))
	  {
	    ((FtsRemoteData) data).setDocument(GetSketchWindow().itsDocument);
	  }

	/* HACK END !! */


	if ((data == null) && (itsFtsObject instanceof FtsObjectWithData))
	  {
	    // Fall back the old obsolete behaviour
	    // Should be substituted by the previous one
	    // need changes to table, qlist and patcher (?)
	    // for this

	    data = ((FtsObjectWithData) itsFtsObject).getData();	   
	  }

	if (data != null)
	  {
	    Cursor temp = itsSketchPad.getCursor();

	    itsSketchPad.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	    try
	      {
		Mda.edit(data);
	      }
	    catch ( MaxDocumentException e)
	      {
		// SHould do something better
		System.err.println( e);
	      }


	    itsSketchPad.setCursor(temp);
	  }
      }
    else if ( !itsSketchPad.itsRunMode) 
      itsSketchPad.ClickOnObject( this, evt, x, y);

    return true;
  }

  public void RestartEditing() {
    super.RestartEditing();
  }
  

  //--------------------------------------------------------
  // paint
  //--------------------------------------------------------
  int paintCount = 0;
  public void Paint_specific(Graphics g)
  {
    if (isError == -1)
      isError = ((Integer)itsFtsObject.get("error")).intValue();

    if (isError == 0)
      {
	if (! itsSelected)
	  g.setColor(itsLangNormalColor);
	else
	  g.setColor(itsLangSelectedColor);
      }
    else
      g.setColor(Color.red);

    g.fillRect(getItsX()+1,getItsY()+1,getItsWidth()-2, getItsHeight()-2);
    g.fill3DRect(getItsX()+2, getItsY()+2, getItsWidth()-4, getItsHeight()-4, true);
    
    //paint white square
    if (! itsSelected)
      g.setColor(Color.white);
    else
      g.setColor(itsLangNormalColor);

    g.fillRect(getItsX()+getWhiteOffset(), getItsY()+2, getItsWidth()-(getWhiteOffset()*2), getItsHeight()-2*HEIGHT_DIFF);

    g.setColor(Color.black);
    g.drawRect(getItsX()+0, getItsY()+0, getItsWidth()-1, getItsHeight()-1);
    
    g.setColor(Color.black);
    if(!itsSketchPad.itsRunMode) 
      g.fillRect(getItsX()+getItsWidth()-DRAG_DIMENSION,getItsY()+getItsHeight()-DRAG_DIMENSION, DRAG_DIMENSION, DRAG_DIMENSION);
    
    g.setFont(getFont());
    DrawParsedString(g);
  }

}





