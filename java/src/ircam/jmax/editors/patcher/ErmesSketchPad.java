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


/** The graphic workbench for the patcher editor.
 * It handles the interaction of the user with the objects,
 * propagates the mouse events to the objects themselves,
 * allow the selection, moving, erasing, resizing of objects.
 * It keeps track of the toolBar state, it handles the 
 * offscreen and much, much more...
 */

public class ErmesSketchPad extends JPanel implements FtsUpdateGroupListener {

  private DisplayList displayList;

  public DisplayList getDisplayList()
  {
    return displayList;
  }

  private Interaction interaction;


  Interaction getInteraction()
  {
    return interaction;

  }

  private KeyMap keyMap;

  KeyMap getKeyMap()
  {
    return keyMap;
  }

  private long lastUpdateGroupStartTime = 0;
  private long updateGroupStartTime = 0;
  private long lastUpdateGroupEndTime = 0;
  private long updateGroupEndTime = 0;
  private int count = 0;

  public void updateGroupStart() 
  {
    // (fd)
    // Measurement code
    lastUpdateGroupStartTime = updateGroupStartTime;
    updateGroupStartTime = System.currentTimeMillis();
  }
  
  public void updateGroupEnd() 
  {
    Toolkit.getDefaultToolkit().sync();

    // (fd)
    // Measurement code
    lastUpdateGroupEndTime = updateGroupEndTime;
    updateGroupEndTime = System.currentTimeMillis();

    count++;

    if (count % 20 == 0)
      {
	double p = (100.0 * (lastUpdateGroupEndTime - lastUpdateGroupStartTime)) / (updateGroupStartTime - lastUpdateGroupStartTime);

	//System.err.println( "update load " + p + "%");
      }
  }

  ErmesSketchWindow itsSketchWindow;
  FtsObject itsPatcher;
  FtsPatcherData itsPatcherData;

  private final static int SKETCH_WIDTH = 1200;
  private final static int SKETCH_HEIGHT = 1200;
  static Dimension preferredSize = new Dimension(SKETCH_WIDTH, SKETCH_HEIGHT);

  public ErmesObjInOutPop itsInPop = null;
  public ErmesObjInOutPop itsOutPop = null;
  private ErmesObjEditField itsEditField = null;

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

  final void doNothing() 
  {
    interaction.doNothing();
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
    ErmesObject object;
    ErmesConnection connection;

    numberOfPaste += 1;

    resetFocus();

    ErmesSelection.patcherSelection.setOwner(this); 

    if (! ErmesSelection.patcherSelection.isEmpty())
      ErmesSelection.patcherSelection.deselectAll();

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

	object = ErmesObject.makeErmesObject( this, fo);
	displayList.addObject( object);
	ErmesSelection.patcherSelection.select( object);
	object.redraw();
      }

    ErmesObject fromObj, toObj;
    
    for ( Enumeration e2 = connectionVector.elements(); e2.hasMoreElements();) 
      {
	fc = (FtsConnection)e2.nextElement();

	connection = new ErmesConnection( this, 
					  displayList.getErmesObjectFor(fc.getFrom()),
					  fc.getFromOutlet(), 
					  displayList.getErmesObjectFor(fc.getTo()),
					  fc.getToInlet(),
					  fc);

	displayList.addConnection( connection);

	ErmesSelection.patcherSelection.select( connection);
	connection.redraw();
      }

    interaction.startSelection();
  }
  
  void InitFromFtsContainer( FtsPatcherData theContainerObject)
  {
    FtsPatcherData aFtsPatcherData = theContainerObject;

    Object[] objects = aFtsPatcherData.getObjects().getObjectArray();
    int osize = aFtsPatcherData.getObjects().size();

    for ( int i = 0; i < osize; i++)
      {
	ErmesObject object = ErmesObject.makeErmesObject( this, (FtsObject)objects[i]);
	displayList.addObject( object);
      }
		
    // chiama tanti AddConnection...

    MaxVector connectionVector = aFtsPatcherData.getConnections();	//usefull?

    Object[] connections = aFtsPatcherData.getConnections().getObjectArray();
    int csize = aFtsPatcherData.getConnections().size();

    for ( int i = 0; i < csize; i++)
      {
	ErmesConnection connection;
	FtsConnection fc = (FtsConnection)connections[i];

	connection = new ErmesConnection( this, 
					  displayList.getErmesObjectFor(fc.getFrom()),
					  fc.getFromOutlet(), 
					  displayList.getErmesObjectFor(fc.getTo()),
					  fc.getToInlet(),
					  fc);

	displayList.addConnection(connection);
      }
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
    interaction = new Interaction(this, displayList);
    keyMap = new KeyMap(this);

    // Next two temporary (mdc)

    RepaintManager.currentManager(this).setDoubleBufferingEnabled(false);
    setDoubleBuffered(false);

    setOpaque(true);

    Fts.getServer().addUpdateGroupListener( this);

    setLayout( null);

    itsEditField = new ErmesObjEditField( this);
    add( itsEditField);

    itsEditField.setVisible( false);
    itsEditField.setLocation( -200,-200);
    
    setBackground( Settings.sharedInstance().getEditBackgroundColor());
    addMouseMotionListener( interaction); 
    addMouseListener( interaction);
    addKeyListener( interaction);

    InitFromFtsContainer( itsPatcherData);

    displayList.getAndFixContainerSize(preferredSize);
    setPreferredSize(preferredSize);

    PrepareInChoice(); 
    PrepareOutChoice();

    requestDefaultFocus(); // ???
  }
	

  /* To be called to fix the sketchpad size after some changes (move
     and paste); it also assure the selection is in the scrolled area. */

  public void fixSize()
  {
    boolean redraw;

    redraw = displayList.getAndFixContainerSize(preferredSize);
    setPreferredSize(preferredSize);

    itsSketchWindow.validate();

    if (redraw)
      redraw();
  }
  
  static final private Dimension minSize = new Dimension(30, 20);

  public Dimension getMinimumSize() 
  {
    return minSize;
  }
	
  void makeObject( String description, int x, int y)
  {
    FtsObject fo;
    ErmesObject object;

    try
      {
	fo = Fts.makeFtsObject( itsPatcher, description);

	fo.setX( x);
	fo.setY( y);

	object = ErmesObject.makeErmesObject( this, fo);
	displayList.addObject( object);

	if (object instanceof ErmesObjEditable)
	  ((ErmesObjEditable)object).startEditing();

	object.redraw();
      }
    catch ( FtsException ftse)
      {
	System.err.println( "ErmesSketchPad:mousePressed: INTERNAL ERROR: FTS Instantiation Error: " + ftse);
	ftse.printStackTrace();
      }
  }

  final public ErmesObjEditField getEditField()
  {
    return itsEditField;
  }
  
  public void showObject( Object obj)
  {
    // Should select or highlight obj if it is an FtsObject

    if (obj instanceof FtsObject) 
      {
	ErmesObject object = displayList.getErmesObjectFor((FtsObject) obj);

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
	  }
      }
  }


  // public Dimension getPreferredSize() 
  // {
  // return preferredSize;
  // }
  
  void PrepareInChoice() 
  {
    itsInPop = new ErmesObjInOutPop( itsSketchWindow, itsPatcher.getNumberOfInlets() + 4);
    add( itsInPop);
  }

  final void RedefineInChoice()
  {
    itsInPop.Redefine(itsPatcher.getNumberOfInlets() + 4);
  }

  void PrepareOutChoice()
  {
    itsOutPop = new ErmesObjInOutPop( itsSketchWindow, itsPatcher.getNumberOfOutlets() + 4);
    add( itsOutPop);
  }
  
  void RedefineOutChoice()
  {
    itsOutPop.Redefine( itsPatcher.getNumberOfOutlets() + 4);
  }

  public void paintComponent( Graphics g)
  {
    displayList.paint(g);
  }		

  final public void redraw()
  {
    repaint();
  }
       
  final void setToolBar( ErmesToolBar theToolBar)
  {
    interaction.setToolBar(theToolBar);
  }

  //--------------------------------------------------------
  //	cleanAll
  //	Make the cleanup needed before closing the 
  //    sketchpad
  //--------------------------------------------------------

  void cleanAll()
  {
    if (ErmesSelection.patcherSelection.ownedBy(this))
      ErmesSelection.patcherSelection.deselectAll();

    displayList.disposeAllObjects();

    Fts.getServer().removeUpdateGroupListener( this);
    removeMouseMotionListener( interaction); 
    removeMouseListener( interaction);
    removeKeyListener( interaction);

    remove( itsInPop);
    remove( itsOutPop);
    remove( itsEditField);

    // Clean up to help the gc, and found the bugs.

    itsSketchWindow = null;// should not be needed, here to get the grabber !!

    itsPatcher = null;
    itsInPop = null;
    itsOutPop = null;
    itsEditField = null;
    anOldPastedObject = null;
  }

  //
  // This function handles the Focus change and updates the menus.
  //

  void resetFocus()
  {
    if (getEditField() != null && getEditField().HasFocus())
      {
	getEditField().transferFocus();
      }

    if (interaction.isEditingObject())
      {
	getEditField().LostFocus();
	requestFocus();
	interaction.doNothing();
      }
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

    if (locked)
      {
	if (interaction.isEditingObject())
	  {
	    itsEditField.LostFocus();
	    requestFocus();
	    interaction.doNothing();
	  }

	setBackground( Settings.sharedInstance().getLockBackgroundColor());

	if (ErmesSelection.patcherSelection.ownedBy(this))
	  ErmesSelection.patcherSelection.deselectAll();
      }
    else
      setBackground( Settings.sharedInstance().getEditBackgroundColor());
  }

  final public boolean isLocked()
  {
    return locked;
  }

  /* Temporary support for text cut/pasting; will work only between objects,
     not in the general case; the general case need to use the real JDK clipboard
     support, and will be done when the patcher editor will be based on the toolkit.
     */
     
  void pasteText(String text)
  {
    if (interaction.isEditingObject())
      itsEditField.insert(text,  itsEditField.getCaretPosition());
  }

  boolean canCopyText()
  {
    return interaction.isEditingObject();
  }

  boolean canPasteText()
  {
    return interaction.isEditingObject();
  }

  String getSelectedText()
  {
    if (interaction.isEditingObject())
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

  // Key Client; should go in the interaction

  public void setKeyEventClient( KeyEventClient keyEventClient)
  {
    interaction.setKeyEventClient(keyEventClient);
  }

  // Selection change handling

  void selectionChanged()
  {
    itsSketchWindow.selectionChanged();
  }
}
