package ircam.jmax.editors.patcher;


import java.awt.*; 
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.util.*;
import java.lang.*;

import javax.swing.*; 

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;


/** The graphic workbench for the patcher editor.
 * It handles the interaction of the user with the objects,
 * propagates the mouse events to the objects themselves,
 * allow the selection, moving, erasing, resizing of objects.
 * It keeps track of the toolBar state, it handles the 
 * offscreen and much, much more...
 */

public class ErmesSketchPad extends JComponent implements FtsUpdateGroupListener
{
  static boolean syncPaint = false;

  static void setSyncPaint(boolean v)
  {
    syncPaint = v;
  }

  Rectangle invalid = new Rectangle();
  boolean somethingToDraw = false;

  public void updateGroupStart()
  {
    somethingToDraw = false;
  }

  public void updateGroupEnd()
  {
    if (somethingToDraw);
      {
	if (isLocked() && syncPaint)
	  paintImmediately(invalid);
	else
	  repaint(invalid);
      }
  }

  public void paintAtUpdateEnd(int x, int y, int w, int h)
  {
    if (somethingToDraw)
      SwingUtilities.computeUnion(x, y, w, h, invalid);
    else
      {
	somethingToDraw = true;
	invalid.setBounds(x, y, w, h);
      }
  }

  private DisplayList displayList;

  public DisplayList getDisplayList()
  {
    return displayList;
  }

  private KeyMap keyMap;

  KeyMap getKeyMap()
  {
    return keyMap;
  }

  ErmesSketchWindow itsSketchWindow;
  FtsObject itsPatcher;

  public FtsObject getFtsPatcher()
  {
    return itsPatcher;
  }

  FtsPatcherData itsPatcherData;


  // Shouldn't/Couldn't the popup being static, or created on the fly ?

  private EditField itsEditField;

  // FONT HANDLING

  private String defaultFontName;
  private int defaultFontSize;
  
  public final String getDefaultFontName() 
  {
    return defaultFontName;
  }

  public final int getDefaultFontSize() 
  {
    return defaultFontSize;
  }

  public final void setDefaultFontName(String v) 
  {
    defaultFontName = v;
  }

  public final void setDefaultFontSize(int v) 
  {
    defaultFontSize = v;
  }

  final public ErmesSketchWindow getSketchWindow()
  {
    return itsSketchWindow;
  }
  
  private int incrementalPasteOffsetX;
  private int incrementalPasteOffsetY;
  int numberOfPaste = 0;
  private FtsObject anOldPastedObject = null;
  
  void resetPaste(int n)
  {
    numberOfPaste = n;
  }

  // note: the following function is a reduced version of InitFromFtsContainer.
  // better organization urges

  void PasteObjects( MaxVector objectVector, MaxVector connectionVector) 
  {
    FtsObject	fo;
    FtsConnection fc;
    GraphicObject object;
    GraphicConnection connection;

    numberOfPaste += 1;

    if (isTextEditingObject())
      stopTextEditing();

    ErmesSelection.patcherSelection.setOwner(this); 

    if (! ErmesSelection.patcherSelection.isEmpty())
      {
	ErmesSelection.patcherSelection.redraw(); 
	ErmesSelection.patcherSelection.deselectAll();
      }

    fo = (FtsObject)objectVector.elementAt( 0);

    if (numberOfPaste == 0) 
      {
	incrementalPasteOffsetX = 0;
	incrementalPasteOffsetY = 0;
      }
    else if (numberOfPaste == 1) 
      {
	anOldPastedObject = fo;

	incrementalPasteOffsetX = 20;
	incrementalPasteOffsetY = 20;
      }
    else if (numberOfPaste == 2) 
      {
	incrementalPasteOffsetX = (anOldPastedObject.getX() - fo.getX());
	incrementalPasteOffsetY = (anOldPastedObject.getY() - fo.getY());
      }

    for ( Enumeration e = objectVector.elements(); e.hasMoreElements();) 
      {
	fo = (FtsObject)e.nextElement();

	int newPosX = fo.getX() + numberOfPaste*incrementalPasteOffsetX;
	int newPosY = fo.getY() + numberOfPaste*incrementalPasteOffsetY;

	fo.setX( newPosX);
	fo.setY( newPosY);

	object = GraphicObject.makeGraphicObject( this, fo);
	displayList.add( object);
	ErmesSelection.patcherSelection.select( object);
	object.redraw();
      }

    GraphicObject fromObj, toObj;
    
    for ( Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();) 
      {
	fc = (FtsConnection)e2.nextElement();

	connection = new GraphicConnection( this, 
					  displayList.getGraphicObjectFor(fc.getFrom()),
					  fc.getFromOutlet(), 
					  displayList.getGraphicObjectFor(fc.getTo()),
					  fc.getToInlet(),
					  fc);

	displayList.add( connection);

	ErmesSelection.patcherSelection.select( connection);
	connection.updateDimensions();
	connection.redraw();
      }

    displayList.reassignLayers();
    displayList.sortDisplayList();
  }
  
  void InitFromFtsContainer( FtsPatcherData theContainerObject)
  {
    FtsPatcherData aFtsPatcherData = theContainerObject;
    Object[] objects = aFtsPatcherData.getObjects().getObjectArray();
    int osize = aFtsPatcherData.getObjects().size();
    boolean doLayers = false;

    for ( int i = 0; i < osize; i++)
      {
	GraphicObject object = GraphicObject.makeGraphicObject( this, (FtsObject)objects[i]);
	displayList.add( object);

	if (object.getLayer() < 0)
	  doLayers = true;
      }
		

    MaxVector connectionVector = aFtsPatcherData.getConnections();
    Object[] connections = aFtsPatcherData.getConnections().getObjectArray();
    int csize = aFtsPatcherData.getConnections().size();

    for ( int i = 0; i < csize; i++)
      {
	GraphicConnection connection;
	FtsConnection fc = (FtsConnection)connections[i];

	connection = new GraphicConnection( this, 
					  displayList.getGraphicObjectFor(fc.getFrom()),
					  fc.getFromOutlet(), 
					  displayList.getGraphicObjectFor(fc.getTo()),
					  fc.getToInlet(),
					  fc);
	displayList.add(connection);
	connection.updateDimensions();
      }

    if (doLayers)
      displayList.reassignLayers();

    displayList.sortDisplayList();
  }

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------

  ErmesSketchPad( ErmesSketchWindow theSketchWindow, FtsPatcherData thePatcherData) 
  {
    super();

    String s;

    // Setting the local variables

    itsSketchWindow = theSketchWindow;
    itsPatcherData  = thePatcherData;
    itsPatcher      = thePatcherData.getContainerObject(); // ???

    Fts.getServer().addUpdateGroupListener( this);

    // Get the defaultFontName and Size

    defaultFontName = MaxApplication.getProperty("jmaxDefaultFont");

    if (defaultFontName == null)
      defaultFontName = ircam.jmax.utils.Platform.FONT_NAME;

    s = MaxApplication.getProperty("jmaxDefaultFontSize");
    
    if (s == null)
      defaultFontSize = ircam.jmax.utils.Platform.FONT_SIZE;
    else
      defaultFontSize = Integer.parseInt(s);

    // Install the display List object

    displayList = new DisplayList(this);
    engine      = new InteractionEngine(this);
    keyMap = new KeyMap(this, this.getSketchWindow());

    // Next two temporary (mdc)

    if (MaxApplication.getProperty("dg") != null)
      {
	RepaintManager.currentManager(this).setDoubleBufferingEnabled(false);
	setDoubleBuffered(false);
	setDebugGraphicsOptions(DebugGraphics.FLASH_OPTION);
      }
    else if (MaxApplication.getProperty("nodb") != null)
      {
	RepaintManager.currentManager(this).setDoubleBufferingEnabled(false);
	setDoubleBuffered(false);
      }

    setOpaque(true);
    setLayout( null);

    itsEditField = new EditField( this);
    add( itsEditField);

    InitFromFtsContainer( itsPatcherData);

    fixSize();

    requestDefaultFocus(); // ???
  }
	

  /* To be called to fix the sketchpad size after some changes (move
     and paste); it also assure the selection is in the scrolled area. */

  static Rectangle totalBounds = new Rectangle();
  static Dimension preferredSize = new Dimension();

  public void fixSize()
  {
    boolean redraw = false;

    totalBounds.x = 0;
    totalBounds.y = 0;
    totalBounds.width = 0;
    totalBounds.height = 0;

    displayList.getBounds(totalBounds);

    if ((totalBounds.x < 0) || (totalBounds.y < 0))
      {
	int dx, dy;

	if (totalBounds.x < 0)
	  dx = (-1) * totalBounds.x;
	else
	  dx = 0;

	if (totalBounds.y < 0)
	  dy = (-1) * totalBounds.y;
	else
	  dy = 0;
	
	displayList.moveAllBy(dx, dy);

	redraw = true;
      }

    // If the objects bounds are bigger than the current sketch bounds
    // resize the sketch to be 5 points bigger than this size

    if ((totalBounds.width > getWidth()) || (totalBounds.height > getHeight()))
      {
	preferredSize.height = totalBounds.height + 5;
	preferredSize.width  = totalBounds.width  + 5;

	// Test to avoid windows too small 

	if (preferredSize.height < 20)
	  preferredSize.height = 20;

	if (preferredSize.width < 20)
	  preferredSize.width = 20;

	setPreferredSize(preferredSize);
      }

    if (redraw)
      redraw();

    revalidate(); // ????
  }


  public boolean pointIsVisible(Point point, int margin)
  {
    Rectangle r = itsSketchWindow.itsScrollerView.getViewport().getViewRect();

    return ((point.x > r.x + margin) && (point.x < r.x + r.width - margin) &&
	    (point.y > r.y + margin) && (point.y < r.y + r.height - margin));
  }


  public void scrollBy(int dx, int dy)
  {
    Rectangle r = itsSketchWindow.itsScrollerView.getViewport().getViewRect();

    r.x = r.x + dx;
    r.y = r.y + dy;

    scrollRectToVisible(r);

    revalidate(); // ???
    redraw();
  }

  public Point whereItIs(Point point, Point direction, int margin)
  {
    Rectangle r = itsSketchWindow.itsScrollerView.getViewport().getViewRect();

    direction.x = 0;
    direction.y = 0;

    // Vertical dimension

    if (point.x <= r.x + margin)
      direction.x = -1;
    else if (point.x >= r.x + r.width - margin)
      direction.x = 1;

    if (point.y <= r.y + margin)
      direction.y = -1;
    else if (point.y >= r.y + r.height - margin)
      direction.y = 1;

    return direction;
  }

  // if the selection is outside the current visible 
  // area, rescroll to show it

  void showSelection()
  {
    Rectangle selectionBounds = ErmesSelection.patcherSelection.getBounds();
    Rectangle visibleRectangle = itsSketchWindow.itsScrollerView.getViewport().getViewRect();

    if (selectionBounds != null)
      {
	if (! SwingUtilities.isRectangleContainingRectangle(visibleRectangle, selectionBounds))
	  {
	    scrollRectToVisible(selectionBounds);
	    revalidate(); // ???
	  }
      }
  }


  static final private Dimension minSize = new Dimension(30, 20);

  public Dimension getMinimumSize() 
  {
    return minSize;
  }
	
  GraphicObject makeObject( String description, int x, int y)
  {
    FtsObject fo;
    GraphicObject object = null;

    try
      {
	fo = Fts.makeFtsObject( itsPatcher, description);

	fo.setX( x);
	fo.setY( y);

	object = GraphicObject.makeGraphicObject( this, fo);
	displayList.add( object);
	displayList.reassignLayers();

	object.redraw();
      }
    catch ( FtsException ftse)
      {
	System.err.println( "ErmesSketchPad:mousePressed: INTERNAL ERROR: FTS Instantiation Error: " + ftse);
	ftse.printStackTrace();
      }

    return object;
  }

  /* Handling of the object text editing */

  Editable editedObject = null;

  final public EditField getEditField()
  {
    return itsEditField;
  }

  final public boolean isTextEditingObject()
  {
    return editedObject != null;
  }

  final public Editable getTextEditedObject()
  {
    return editedObject;
  }

  final public void textEditObject(Editable object)
  {
    textEditObject(object, null);
  }

  final public void textEditObject(Editable object, Point p)
  {
    if (editedObject != null)
      stopTextEditing();

    editedObject = object;
    itsEditField.doEdit(object, p);
  }

  final public void stopTextEditing()
  {
    if (editedObject != null)
      itsEditField.endEdit();

    editedObject = null;
  }

  public void showObject( Object obj)
  {
    // Should select or highlight obj if it is an FtsObject

    if (obj instanceof FtsObject) 
      {
	GraphicObject object = displayList.getGraphicObjectFor((FtsObject) obj);

	if (object != null)
	  {
	    ErmesSelection.patcherSelection.setOwner(this); 

	    if (ErmesSelection.patcherSelection.isEmpty())
	      {
		ErmesSelection.patcherSelection.select( object);
		object.redraw();
	      }
	    else
	      {
		ErmesSelection.patcherSelection.redraw(); 
		ErmesSelection.patcherSelection.deselectAll();
		ErmesSelection.patcherSelection.select( object);
		object.redraw();
	      }

	    showSelection();
	  }
      }
  }

  public void paintComponent( Graphics g)
  {
    displayList.paint(g);
  }		

  final public void redraw()
  {
    repaint();
  }


  private ErmesToolBar toolBar;

  final void setToolBar( ErmesToolBar toolBar)
  {
    this.toolBar = toolBar;
  }

  final public ErmesToolBar getToolBar( )
  {
    return toolBar;
  }

  //--------------------------------------------------------
  //	cleanAll
  //	Make the cleanup needed before closing the 
  //    sketchpad
  //--------------------------------------------------------

  void cleanAll()
  {
    Fts.getServer().removeUpdateGroupListener( this);

    engine.dispose();

    if (ErmesSelection.patcherSelection.ownedBy(this))
      ErmesSelection.patcherSelection.deselectAll();

    displayList.disposeAllObjects();

    remove( itsEditField);

    // Clean up to help the gc, and found the bugs.

    itsSketchWindow = null;// should not be needed, here to get the grabber !!

    itsPatcher = null;
    itsEditField = null;
    anOldPastedObject = null;
  }
  
  // The waiting/stopWaiting service

  private int waiting = 0;

  public void waiting()
  {
    if (waiting >= 0)
      setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

    waiting++;
  }

  public void stopWaiting()
  {
    waiting--;

    if (waiting <= 0)
      setCursor( Cursor.getDefaultCursor());
  }

  // ----------------------------------------------------------------------
  // Mode handling
  // ----------------------------------------------------------------------


  private boolean locked = false;

  void setLocked( boolean locked)
  {
    this.locked = locked;

    if (isLocked())
      setRunModeInteraction();
    else
      setEditModeInteraction();

    if (isLocked())
      {
	if (isTextEditingObject())
	  stopTextEditing();

	if (ErmesSelection.patcherSelection.ownedBy(this))
	  ErmesSelection.patcherSelection.deselectAll();
      }

    redraw();
  }

  final public boolean isLocked()
  {
    return locked;
  }

  /* Temporary support for text cut/pasting; will work only between objects,
     not in the general case; the general case need to use the real JDK clipboard
     support, and will be done when the patcher editor will be based on the toolkit.
     */
     
  void pasteText()
  {
    itsEditField.paste();
    itsEditField.resizeIfNeeded();
  }

  void copyText()
  {
    itsEditField.copy();
  }

  void cutText()
  {
    itsEditField.cut();
    itsEditField.resizeIfNeeded();
  }

  boolean canCopyText()
  {
    return isTextEditingObject();
  }

  boolean canPasteText()
  {
    return isTextEditingObject();
  }

  public String getSelectedText()
  {
     if (isTextEditingObject())
       return itsEditField.getSelectedText();
     else
       return null;
  }

  void deleteSelectedText()
  {
    itsEditField.deleteSelectedText();
  }

  // Annotations; should be done better ..

  private boolean annotating = false;

  public void setAnnotating()
  {
    annotating = true;
  }

  public boolean isAnnotating()
  {
    return annotating;
  }

  public void cleanAnnotations()
  {
    if (annotating)
      {
	annotating = false;
	redraw();
      }
  }

  // Support for keeping a unique supplementary keylistener
  // in the sketchpad; used currently by the number box, the other
  // are Swing components and use the focus.

  KeyEventClient keyEventClient;

  public void setKeyEventClient( KeyEventClient kv)
  {
    if (kv == keyEventClient)
      return;

    if (keyEventClient != null)
      {
	removeKeyListener( keyEventClient);
	this.keyEventClient.keyInputLost();
      }

    keyEventClient = kv;

    if (keyEventClient != null)
      {
	addKeyListener(keyEventClient);
	keyEventClient.keyInputGained();
      }
  }


  // Support for the new Interaction Model

  public void showAddPopUp(Point p)
  {
    AddPopUp.popup(this, p.x, p.y);
  }

  private InteractionEngine engine;

  public void setRunModeInteraction()
  {
    stopTextEditing();
    toolBar.reset();
    resetHighlightedInlet();
    resetHighlightedOutlet();
    engine.setTopInteraction(Interactions.runModeInteraction);
  }

  public void setEditModeInteraction()
  {
    toolBar.reset();
    engine.setTopInteraction(Interactions.editModeInteraction);
  }

  String newObjectDescription = null;
  boolean newObjectEdit;

  public void setAddModeInteraction(String description, String message, boolean edit)
  {
    newObjectDescription = description;
    newObjectEdit = edit;
    showMessage(message);

    stopTextEditing();
    setCursor( Cursor.getPredefinedCursor( Cursor.CROSSHAIR_CURSOR));
    engine.setTopInteraction(Interactions.addModeInteraction);
  }

  public void makeAddModeObject(int x, int y, boolean edit)
  {
    GraphicObject object = makeObject(newObjectDescription, x, y);

    if (object instanceof Standard)
      ((Standard)object).setIgnoreError(true);

    if (edit && newObjectEdit && (object instanceof Editable))
      {
	// The EditField is not really ready until the control
	// is returned back to the event loop; this is why we invoke textEditObject 
	// with an invoke later command.
	
	final Editable obj  = (Editable)object;
	
	SwingUtilities.invokeLater(new Runnable() {
	  public void run()
	    { textEditObject((Editable)obj);}});
      }
  }

  public  InteractionEngine getEngine()
  {
    return engine;
  }

  public void endInteraction()
  {
    engine.popInteraction();
  }

  
  // Support for the inlet/outlet highligthing;
  // to avoid adding to much memory to every object,
  // we store here the id of the higlighted inlet and outlet

  private int highlightedInlet;
  private GraphicObject  highlightedInletObject = null;

  public int getHighlightedInlet()
  {
    return highlightedInlet;
  }

  public GraphicObject getHighlightedInletObject()
  {
    return highlightedInletObject;
  }

  public boolean isHighlightedInlet(GraphicObject object, int inlet)
  {
    return ((highlightedInletObject == object) && (highlightedInlet == inlet));
  }

  public boolean hasHighlightedInlet(GraphicObject object)
  {
    return (highlightedInletObject == object);
  }

  public boolean hasHighlightedInlet()
  {
    return (highlightedInletObject != null);
  }

  public void setHighlightedInlet(GraphicObject object, int inlet)
  {
    highlightedInletObject = object;
    highlightedInlet       = inlet;

    if (highlightedInletObject != null)
      {
	highlightedInletObject.redraw();
	highlightedInletObject.redrawConnections();
      }
  }

  public void resetHighlightedInlet()
  {
    if (highlightedInletObject != null)
      {
	highlightedInletObject.redraw();
	highlightedInletObject.redrawConnections();	
      }
    
    highlightedInletObject = null;
  }

  int highlightedOutlet;
  GraphicObject  highlightedOutletObject = null;

  public int getHighlightedOutlet()
  {
    return highlightedOutlet;
  }

  public GraphicObject getHighlightedOutletObject()
  {
    return highlightedOutletObject;
  }

  public boolean isHighlightedOutlet(GraphicObject object, int outlet)
  {
    return ((highlightedOutletObject == object) && (highlightedOutlet == outlet));
  }

  public boolean hasHighlightedOutlet(GraphicObject object)
  {
    return (highlightedOutletObject == object);
  }

  public boolean hasHighlightedOutlet()
  {
    return (highlightedOutletObject != null);
  }

  public void setHighlightedOutlet(GraphicObject object, int outlet)
  {
    highlightedOutletObject = object;
    highlightedOutlet       = outlet;

    if (highlightedOutletObject != null)
      {
	highlightedOutletObject.redraw();
	highlightedOutletObject.redrawConnections();
      }
  }

  public void resetHighlightedOutlet()
  {
    if (highlightedOutletObject != null)
      {
	highlightedOutletObject.redraw();
	highlightedOutletObject.redrawConnections();
      }

    highlightedOutletObject = null;
  }

  // General Highlighting reset function

  public void resetHighlighted()
  {
    resetHighlightedOutlet();
    resetHighlightedInlet();
  }

  // Messages 

  public void showMessage(String text)
  {
    itsSketchWindow.showMessage(text);
  }

  public void resetMessage()
  {
    itsSketchWindow.resetMessage();
  }
}
