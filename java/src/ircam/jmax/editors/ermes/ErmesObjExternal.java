package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.mda.*;

//
// The generic "extern" object in ermes. (example: adc1~)
//
public class ErmesObjExternal extends ErmesObjEditableObject implements FtsPropertyHandler {

  public boolean iAmPatcher = false;
  public static final int WHITE_OFFSET = 6;

  private String itsBackupText = new String();

  int isError = -1; // cache of the error property, to speed up paint


  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------
  public ErmesObjExternal( ErmesSketchPad theSketchPad, FtsObject theFtsObject) 
  {
    super(theSketchPad, theFtsObject);
  }

  public boolean AreYouPatcher() 
  {
    return iAmPatcher;
  }

  protected int getWhiteOffset() 
  {
    return WHITE_OFFSET;
  }

  public void Init() 
  {
    // Added by MDC; get the correct String from the object, and then call super
    // It is needed because ErmesObjExternal and ErmesObjMessage use different methods
    // to get the string from the object.

    itsArgs = itsFtsObject.getDescription().trim();

    super.Init();

    if ( itsFtsObject instanceof FtsContainerObject)
      this.YouArePatcher( true);

    ParseText( itsArgs);

    if ((! itsArgs.equals("")) &&(! canResizeBy( 0, 0)))
      RestoreDimensions( false);
  }

  public void propertyChanged(FtsObject obj, String name, Object value) 
  {
    //handle the "error" property, the only one we're listening at
    // call super for the others

    if (name == "error") 
      {
	if ((value != null) && (value instanceof Integer)) 
	  {
	    isError = ((Integer)value).intValue();
	    DoublePaint();
	  }
      } else
	super.propertyChanged(obj, name, value);
  }

  public void YouArePatcher( boolean what) 
  {
    iAmPatcher = what;
  }

  /* Inspector */
  public boolean inspectorAlreadyOpen() 
  {
    return (ErmesPatcherInspector.isOpen() && ErmesPatcherInspector.getInspectedObject() == itsFtsObject);
  }

  public void openInspector() 
  {
    if (iAmPatcher)
      ErmesPatcherInspector.inspect( (FtsContainerObject) itsFtsObject);
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

  public void MouseDown_specific(MouseEvent evt,int x, int y) 
  {
    if ( evt.getClickCount() > 1 ) 
      {
	itsSketchPad.waiting();
	Fts.editPropertyValue(itsFtsObject, "data",
			      new MaxDataEditorReadyListener() {
	  public void editorReady(MaxDataEditor editor)
	    {itsSketchPad.stopWaiting();}
	});
      }
    else if ( !itsSketchPad.itsRunMode) 
      itsSketchPad.ClickOnObject( this, evt, x, y);
  }

  public void startEditing() 
  {
    isError = 0; // to get the edited object green
    super.startEditing();
  }

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

    g.fillRect( getItsX()+1, getItsY()+1, getItsWidth()-2, getItsHeight()-2);
    g.fill3DRect( getItsX()+2, getItsY()+2, getItsWidth()-4, getItsHeight()-4, true);

    //paint white square
    if (! itsSelected)
      g.setColor(Color.white);
    else
      g.setColor(itsLangNormalColor);

    g.fillRect( getItsX() + getWhiteOffset(), 
		getItsY()+2, 
		getItsWidth() - (getWhiteOffset()*2),
                getItsHeight()-2*HEIGHT_DIFF);

    g.setColor(Color.black);
    g.drawRect( getItsX() + 0, getItsY()+0, getItsWidth()-1, getItsHeight()-1);

    g.setColor(Color.black);
    if (!itsSketchPad.itsRunMode)
      g.fillRect( getItsX() + getItsWidth() - DRAG_DIMENSION,
		  getItsY() + getItsHeight() - DRAG_DIMENSION,
		  DRAG_DIMENSION,
		  DRAG_DIMENSION);

    g.setFont( getFont());
    DrawParsedString(g);
  }
}
