//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher;


import java.awt.*; 
import java.awt.print.*;
import java.awt.event.*;
// import java.awt.AWTEvent.*;
import java.awt.AWTEvent;
import java.util.*;
import java.lang.*;
import java.io.*;
import java.awt.datatransfer.*;

//import javax.swing.*; 
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.RepaintManager;
import javax.swing.SwingUtilities;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.interactions.*;
import ircam.fts.client.*;

import ircam.jmax.toolkit.*;

/** The graphic workbench for the patcher editor.
 * It handles the interaction of the user with the objects,
 * propagates the mouse events to the objects themselves,
 * allow the selection, moving, erasing, resizing of objects.
 * It keeps track of the toolBar state, it handles the 
 * offscreen and much, much more...
 */

public class ErmesSketchPad extends JComponent implements  Editor, Printable, FtsUpdateGroupListener, ClipboardOwner
{
  public static FtsUpdateGroup updateGroup;
  static
  {
    try
      {
	updateGroup = new FtsUpdateGroup();
	updateGroup.start();
      }
    catch(IOException e)
      {
	System.err.println("[ErmesSketchPad]: Error in FtsUpdateGroup creation!");
	e.printStackTrace();
      }
  }

  private boolean somethingToUpdate = false;
  private Rectangle invalid = new Rectangle();
  
  public void paintAtUpdateEnd(GraphicObject object, int x, int y, int w, int h)
  {
    displayList.addToUpdate(object);

    if(!somethingToUpdate){
      somethingToUpdate = true;
      invalid.setBounds(x, y, w, h);
    }
    else
      SwingUtilities.computeUnion(x, y, w, h, invalid);
  }
  public void resetUpdate(){
    displayList.resetUpdateObjects();
    somethingToUpdate = false;
  }
  public Rectangle getUpdateRect(){
    return invalid;
  }

  public void updateGroupStart()
  {
    resetUpdate();
  }

  public void updateGroupEnd()
  {
    Rectangle rect = getEditorContainer().getViewRectangle();
    Graphics gr;

    if (isLocked()){
      gr = getGraphics();
      if(gr!=null)
	{
	  ((Graphics2D)gr).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
	  SwingUtilities.computeIntersection(rect.x, rect.y, rect.width, rect.height, invalid);
	  gr.setClip(invalid);
	  displayList.updatePaint(gr);
	}
    }    
    else
      repaint(invalid);
  }

  // ------------------------------------------------

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

  private EditorContainer itsEditorContainer;

  public EditorContainer getEditorContainer(){
    return itsEditorContainer;
  }

  private FtsPatcherObject itsPatcher;

  public FtsPatcherObject getFtsPatcher()
  {
    return itsPatcher;
  }

  public boolean isARootPatcher()
  {
    return (getFtsPatcher().getParent() == JMaxApplication.getRootPatcher());
  }

  public boolean isASubPatcher()
  {
    return !itsPatcher.isARootPatcher();
  }
  public boolean isATemplate()
  {
      return (itsPatcher instanceof FtsTemplateObject);
  }

  // ---------------------------------------------------------------------
  // font handling
  // --------------------------------------------------------------------

  private String defaultFontName;
  private int defaultFontStyle;
  private int defaultFontSize;
  
  public final String getDefaultFontName() 
  {
    return defaultFontName;
  }

  public final int getDefaultFontSize() 
  {
    return defaultFontSize;
  }
  public final int getDefaultFontStyle() 
  {
    return defaultFontStyle;
  }
  public final void setDefaultFontName(String v) 
  {
    defaultFontName = v;
  }

  public final void setDefaultFontSize(int v) 
  {
    defaultFontSize = v;
  }
  public final void setDefaultFontStyle(int s) 
  {
    defaultFontStyle = s;
  }
  public final void changeDefaultFontStyle(String style, boolean selected) 
  {
    if(style.equals("Bold"))
      {
	if((defaultFontStyle == Font.BOLD || defaultFontStyle == Font.BOLD+Font.ITALIC)&&(!selected))
	  defaultFontStyle -= Font.BOLD;
	else
	  if((defaultFontStyle != Font.BOLD && defaultFontStyle != Font.BOLD+Font.ITALIC)&&(selected))
	    defaultFontStyle += Font.BOLD;
      }
    else
      if(style.equals("Italic"))
	{
	  if((defaultFontStyle == Font.ITALIC || defaultFontStyle == Font.BOLD+Font.ITALIC)&&(!selected))
	    defaultFontStyle -= Font.ITALIC;
	  else
	    if((defaultFontStyle != Font.ITALIC && defaultFontStyle != Font.BOLD+Font.ITALIC)&&(selected))
	      defaultFontStyle += Font.ITALIC;
	}
  }

  private boolean automaticFitToText = false;
  public void setAutomaticFitToText(boolean fit){
    automaticFitToText = fit;
  }
  public boolean isAutomaticFitToText(){
    return automaticFitToText;
  }

  // ---------------------------------------------------------
  // cut/copy/paste variables and methods
  // ---------------------------------------------------------
  private int incrementalPasteOffsetX;
  private int incrementalPasteOffsetY;
  private int numberOfPaste = 0;
  private GraphicObject anOldPastedObject = null;
  private int startPasteX = -1;
  private int startPasteY = -1;
  private int lastCopyCount;
  private MaxVector pastedObjects     = new MaxVector();
  private MaxVector pastedConnections = new MaxVector();

  int getPasteNumber(){
    return ++numberOfPaste;
  }
  void resetPaste(int n)
  {
    numberOfPaste = n;
  }  
  void setOldPastedObject(GraphicObject obj){
    anOldPastedObject = obj;
  }
  void setStartPasteXY(int x, int y){
    startPasteX = x;
    startPasteY = y;
  }
  GraphicObject getOldPastedObject(){
    return anOldPastedObject;
  }
  int getStartPasteX(){
    return startPasteX;
  }
  int getStartPasteY(){
    return startPasteY;
  }
  public void addPastedObject(GraphicObject obj)
  {
    pastedObjects.addElement( obj);
    if( undoing) addUndoRedoObject( obj);
  }

  public void addPastedConnection(GraphicConnection c)
  {
    pastedConnections.addElement( c);
  }

  void setIncrementalPasteOffsets(int offsetX, int offsetY){
    incrementalPasteOffsetX = offsetX;
    incrementalPasteOffsetY = offsetY;
  }
  int getPasteOffsetX(){
    return incrementalPasteOffsetX;
  }
  int getPasteOffsetY(){
    return incrementalPasteOffsetY;
  }
  int getPasteDX()
  {
    return numberOfPaste*incrementalPasteOffsetX;
  }
  int getPasteDY()
  {
    return numberOfPaste*incrementalPasteOffsetY;
  }
  void setLastCopyCount(int count){
    lastCopyCount = count;
  }
  int getLastCopyCount(){
    return lastCopyCount;
  }

  private boolean pasting = false;
  public void startPaste()
  {
    pasting = true;
  }

  public void endPaste()
  {
    pasting = false;

    ErmesSelection.patcherSelection.deselectAll();
    for(Enumeration e = pastedObjects.elements(); e.hasMoreElements();)
      ErmesSelection.patcherSelection.select( (GraphicObject)e.nextElement());
    for(Enumeration e = pastedConnections.elements(); e.hasMoreElements();)
      ErmesSelection.patcherSelection.select( (GraphicConnection)e.nextElement());
    
    setOldPastedObject(  ErmesSelection.patcherSelection.getSingleton());    
    if(( startPasteX == -1)||( startPasteY == -1))
      setStartPasteXY( anOldPastedObject.getX(), anOldPastedObject.getY());

    pastedObjects.removeAllElements();
    pastedConnections.removeAllElements();
    
    fixSize();

    // (fd) if only one object pasted, then edit it with all text selected
    if (ErmesSelection.patcherSelection.isSingleton())
      {
	GraphicObject obj = (GraphicObject)ErmesSelection.patcherSelection.getSingleton();
      
	if ((obj instanceof Editable) && !undoing)
	  {
	    ErmesSelection.patcherSelection.deselectAll();
      
	    textEditObject((Editable)obj);
	    SwingUtilities.invokeLater(new Runnable(){
		public void run(){
		  getEditField().selectAll();
		}
	      });
	  }
      }

    undoing = false;

    repaint();
  }

  //--------------------------------------------------------
  //	CONSTRUCTOR
  //--------------------------------------------------------
  ErmesSketchPad(EditorContainer container, FtsPatcherObject thePatcher) 
  {
    String s;

    // Setting the local variables
    itsEditorContainer = container;

    itsPatcher      = thePatcher;

    // Get the defaultFontName and Size
    defaultFontName = JMaxApplication.getProperty("jmaxDefaultFont");

    if (defaultFontName == null)
      defaultFontName = ircam.jmax.Platform.FONT_NAME;

    s = JMaxApplication.getProperty("jmaxDefaultFontSize");
    
    if (s == null)
	defaultFontSize = ircam.jmax.Platform.FONT_SIZE;
    else
      defaultFontSize = Integer.parseInt(s);


    defaultFontStyle = ircam.jmax.Platform.FONT_STYLE;

    // Install the display List object

    displayList = new DisplayList(this);

    engine      = new InteractionEngine(this);
    keyMap      = new KeyMap(this);

    // Next two temporary (mdc)

    setOpaque(true);
    setLayout(null);

    itsEditField = new EditField( this);
    add( itsEditField);

    fixSize();

    KeyEventsManager.addProducer(this);

    updateGroup.add( this);

    initUndoStuff();
  }
  
  private float sx, sy;
  public void scale(float scaleX, float scaleY)
  {
      sx = scaleX;
      sy = scaleY;
      getDisplayList().applyToObjects(new ObjectAction() {
	      public void processObject(GraphicObject object)
	      {
		  object.redraw();
		  object.redrawConnections();
		  object.scale(sx, sy);
		  object.redraw();
		  object.redrawConnections();
	      }});
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

  // ----------------------------------------------------------------
  // make the title for the container (window)
  // ----------------------------------------------------------------

  String getTitle(){
    String name;

    if (itsPatcher.isARootPatcher())
      {
	name = itsPatcher.getName();
	if(name == null) name = "untitled";
      }
      else if(itsPatcher instanceof FtsTemplateObject)
	  name = "template " + itsPatcher.getDescription();
      else if (itsPatcher instanceof FtsPatcherObject)
	  name = "patcher " + itsPatcher.getDescription();
      else 
	  name = "template " + itsPatcher.getDescription();
    return name;
  }

  // ----------------------------------------------------------------
  // scrolling support
  // ----------------------------------------------------------------

  public boolean pointIsVisible(Point point, int margin)
  {
    Rectangle r = itsEditorContainer.getViewRectangle();

    return ((point.x > r.x + margin) && (point.x < r.x + r.width - margin) &&
	    (point.y > r.y + margin) && (point.y < r.y + r.height - margin));
  }


  public void scrollBy(int dx, int dy)
  {
    Rectangle r = itsEditorContainer.getViewRectangle();

    r.x = r.x + dx;
    r.y = r.y + dy;

    scrollRectToVisible(r);

    revalidate();
    redraw();
  }

  public Point whereItIs(Point point, Point direction, int margin)
  {
    Rectangle r = itsEditorContainer.getViewRectangle();

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
    Rectangle visibleRectangle = itsEditorContainer.getViewRectangle();

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
	
  // ---------------------------------------------------------------
  // create a graphic Object
  // ---------------------------------------------------------------

  public void addNewObject(GraphicObject object, boolean doEdit)
  {
    displayList.add( object);
    if((object.getLayer() == -1)||( pasting))
      displayList.reassignLayers();

    object.updateInOutlets();

    if( !multiAdd)
      ErmesSelection.patcherSelection.deselectAll();

    if( fromToolbar)
      {
	ErmesSelection.patcherSelection.select( object);
	fromToolbar = false;
      }
    redraw();

    if (doEdit && newObjectEdit && (!multiAdd) && (object instanceof Editable) && !undoing)
      {
	// The EditField is not really ready until the control
	// is returned back to the event loop; this is why we invoke textEditObject 
	// with an invoke later command.
	
	final Editable obj  = (Editable)object;

	SwingUtilities.invokeLater(new Runnable() {
	    public void run()
	    { textEditObject((Editable)obj);}});
      }
    
    multiAdd = false;
  }

  public void objectRedefined(FtsGraphicObject fObj)
  {
    GraphicObject obj = displayList.getGraphicObjectFor( fObj);
    if(( obj instanceof Editable)&&(getTextEditedObject() == obj))
      stopTextEditing();

    obj.redefined();
    
    ErmesSelection.patcherSelection.deselectAll();
    repaint();
  }
  
  boolean connFromClient = false;
  public void addingConnection()
  {
    connFromClient = true;
  }

  public GraphicConnection addNewConnection(FtsConnection fc)
  {
    GraphicConnection connection = new GraphicConnection(this,displayList.getGraphicObjectFor(fc.getFrom()),
							 fc.getFromOutlet(), 
							 displayList.getGraphicObjectFor(fc.getTo()),
							 fc.getToInlet(), fc.getType(), fc);
    displayList.add(connection);
    connection.updateDimensions();
    if( connFromClient)
      {
	ErmesSelection.patcherSelection.select( connection);
	connFromClient = false;
      }    
    connection.redraw();
    
    return connection;
  }

  /***************************************************************/
  /***************************************************************/
  
  //debug utility 
  private void printObjectsDescription()
  {
    Object[] objects = itsPatcher.getObjects().getObjectArray();
    int osize = itsPatcher.getObjects().size();
    
    for ( int i = 0; i < osize; i++)
      System.err.println("obj "+(FtsGraphicObject)objects[i]+" "+((FtsGraphicObject)objects[i]).getDescription());
  }

  // -----------------------------------------------------------------------
  // Handling of the object text editing
  // -----------------------------------------------------------------------
  private Editable editedObject = null;
  private EditField itsEditField;

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
      {
	itsEditField.endEdit();
	resetUndoRedo();
      }
    editedObject = null;
  }

  final public void abortTextEditing()
  {
    if (editedObject != null)
      itsEditField.abortEdit();

    editedObject = null;
  }

  //////////////////////////////////////////////////
  public void showObject( Object obj)
  {
    // Should select or highlight obj if it is an FtsObject

    if (obj instanceof FtsGraphicObject) 
      {
	if(isLocked())
	    setLocked(false);

	GraphicObject object = displayList.getGraphicObjectFor((FtsGraphicObject) obj);

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

  public void paintComponent(Graphics g)
  {
    ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
    displayList.paint(g);
  }
  
  public int print(Graphics g, PageFormat pf, int pi) throws PrinterException 
  {
    Paper paper = pf.getPaper();
    Point sketchPos = SwingUtilities.convertPoint(this, getLocation(), (ErmesSketchWindow)itsEditorContainer);
    double onsetX = pf.getImageableX()+sketchPos.x;
    double onsetY = pf.getImageableY()+sketchPos.y;
    double width = pf.getImageableWidth();
    double height = pf.getImageableHeight();
    double scaleX = width / (getSize().width);
    double scaleY = height / (getSize().height + 31);
    double scale = (scaleX < scaleY)? scaleX: scaleY;
    
    if(scale < 1.0)
      ((Graphics2D)g).scale(scale, scale);
    
    ((Graphics2D)g).translate(onsetX, onsetY);
    
    if (pi >= 1) {
      return Printable.NO_SUCH_PAGE;
    }
    displayList.paint((Graphics2D) g);
    return Printable.PAGE_EXISTS;
  }

  final public void redraw()
  {
    repaint();
  }

  // ------------------------------------------------------------------------
  // toolbar support
  // ------------------------------------------------------------------------
  private ToolBar toolBar;

  final void setToolBar( ToolBar toolBar)
  {
    this.toolBar = toolBar;
  }

  final public ToolBar getToolBar( )
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
    itsPatcher.resetPatcherListener();
    
    engine.dispose();

    if (ErmesSelection.patcherSelection.ownedBy(this))
      ErmesSelection.patcherSelection.deselectAll();

    displayList.disposeAllObjects();

    remove( itsEditField);

    // Clean up to help the gc, and found the bugs.
    itsEditorContainer = null;// should not be needed, here to get the grabber !!

    itsPatcher = null;
    itsEditField = null;
    editedObject = null;
    anOldPastedObject = null;
    toolBar = null;
    editedObject = null;
    itsEditorContainer = null;
    displayList = null;

    pastedObjects.removeAllElements();
    pastedConnections.removeAllElements();

    updateGroup.remove( this);
  
    ftsUndoClipboard.dispose();
    ftsUndoClipboard = null;
    undoObjects.removeAllElements();
    
    removeSelection.ftsSelection.dispose();
    removeSelection = null;
    itsMessageDisplayer = null;
  
    if( keyEventClient != null)
      {
	removeKeyListener( keyEventClient);
	keyEventClient = null;
      }
    highlightedInletObject = null;
    highlightedOutletObject = null;
    connectingObject = null;
  }


  //--------------------------------------------------------
  //	Close 
  //--------------------------------------------------------

  public void close(boolean doCancel)
  {
    if ( itsPatcher.isARootPatcher())
      {
	if( PatcherSaveManager.saveClosing(getEditorContainer(), doCancel))
	  {
	    itsPatcher.stopUpdates();		      
	    itsPatcher.requestDestroyEditor();
	    ((FtsPatcherObject)itsPatcher.getParent()).requestDeleteObject( itsPatcher);
	    ((ErmesSketchWindow)itsEditorContainer).Destroy();
	  }
      }
    else
      {
	itsPatcher.stopUpdates();	
	itsPatcher.requestDestroyEditor();
	itsPatcher.resetPatcherListener();
	((ErmesSketchWindow)itsEditorContainer).Destroy();
      }

    KeyEventsManager.removeProducer(this);
  }

  public void save()
  {
    PatcherSaveManager.save( itsEditorContainer);
  }

  public void saveAs()
  {
    PatcherSaveManager.saveAs( itsEditorContainer);
  }

  public void print()
  {
    PatcherPrintManager.Print( itsEditorContainer);
  }
  
  // -----------------------------------------------------------------------
  // The waiting/stopWaiting service
  // -----------------------------------------------------------------------

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

  // ------------------------------------------------------------------------
  // Mode handling
  // ------------------------------------------------------------------------
  private boolean locked = false;

  // To set the initial state: set to edit mode only if the
  // initialMode property of a patcher is set and it is set
  // to something different than "run" (usually, "edit" :)
  void InitLock(){
    setLocked( false);
  }

  public void setLocked( boolean locked)
  {
    toolBar.setLocked(locked);

    this.locked = locked;
    // Store the mode in a non persistent, property of 
    // the patch, so that subpatcher can use it as their initial mode
    if (locked)
      {
	itsPatcher.setEditMode(FtsPatcherObject.RUN_MODE);
	setRunModeInteraction();

	if (isTextEditingObject())
	  stopTextEditing();

	if (ErmesSelection.patcherSelection.ownedBy(this))
	  ErmesSelection.patcherSelection.deselectAll();
      }
    else
    {
      setEditModeInteraction();
      itsPatcher.setEditMode(FtsPatcherObject.EDIT_MODE);
    }
    redraw();
    
    setKeyEventClient( null); //when changing mode, always remove key listeners
    requestFocus();
  }

  final public boolean isLocked()
  {
    return locked;
  }

  // ------------------------------------------------------------------------
  //Temporary support for text cut/pasting; will work only between objects,
  //not in the general case; the general case need to use the real JDK clipboard
  //support, and will be done when the patcher editor will be based on the toolkit.
  // ------------------------------------------------------------------------
     
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

  // ------------------------------------------------------------------------
  // Annotations; should be done better ..
  // ------------------------------------------------------------------------
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

  // ------------------------------------------------------------------------
  // Support for keeping a unique supplementary keylistener
  // in the sketchpad; used currently by the number box, the other
  // are Swing components and use the focus.
  // ------------------------------------------------------------------------

  private KeyEventClient keyEventClient;

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

  // ------------------------------------------------------------------------
  // popoup
  // ------------------------------------------------------------------------
  public void showAddPopUp(Point p)
  {
    AddPopUp.popup(this, p.x, p.y);
  }


  // ------------------------------------------------------------------------
  // Support for the new Interaction Model
  // ------------------------------------------------------------------------
  private InteractionEngine engine;

  public void setRunModeInteraction()
  {
    setCursor(Cursor.getDefaultCursor());
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

  public void resetAdding(){
    setCursor(Cursor.getDefaultCursor());
    toolBar.reset();
    resetMessage();
    setEditModeInteraction();
  }

  private String newObjectDescription = null;
  private boolean newObjectEdit;

  public void setAddModeInteraction(String description, String message, boolean edit)
  {
    newObjectDescription = description;
    newObjectEdit = edit;
    showMessage(message);

    stopTextEditing();

    engine.setTopInteraction(Interactions.addModeInteraction);    
  }

  boolean multiAdd    = false;
  boolean fromToolbar = false;
  public void makeAddModeObject(int x, int y, boolean edit)
  {
    resetUndoRedo();

    multiAdd = !edit;
    fromToolbar = true;
    itsPatcher.requestAddObject(newObjectDescription, x, y, edit);
  }

  public  InteractionEngine getEngine()
  {
    return engine;
  }

  public void endInteraction()
  {
    engine.popInteraction();
  }

  // ------------------------------------------------------------------------
  // Support for the inlet/outlet highligthing;
  // to avoid adding to much memory to every object,
  // we store here the id of the higlighted inlet and outlet
  // ------------------------------------------------------------------------

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
    resetHighlightedInlet();

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

  private int highlightedOutlet;
  private GraphicObject  highlightedOutletObject = null;

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
    resetHighlightedOutlet();

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

  // ------------------------------------------------------------------------
  // current object during a connection drawing (used in getSensibleArea in GraphicObject
  // ------------------------------------------------------------------------
  private GraphicObject connectingObject = null;
  public void setConnectingObject(GraphicObject obj){
    connectingObject = obj;
  }
  public GraphicObject getConnectingObject(){
    return connectingObject;
  }
  // -----------------------------------------------------------------
  // Messages 
  // -----------------------------------------------------------------
  private MessageDisplayer itsMessageDisplayer;
  public void setMessageDisplayer(MessageDisplayer displayer){
      itsMessageDisplayer = displayer;
  }
  public MessageDisplayer getMessageDisplayer(){
    return itsMessageDisplayer;
  }

  public void showMessage(String text)
  {
    itsMessageDisplayer.showMessage(text);
  }

  public void resetMessage()
  {
    itsMessageDisplayer.resetMessage();
  }

  public boolean isMessageReset()
  {
    return itsMessageDisplayer.isMessageReset();
  }

  ////////////////////////////////////
  // called at window resize and move
  ///////////////////////////////////

  public void resizeToWindow(int width, int height)
  {
    if (itsPatcher != null) {
      itsPatcher.setWindowWidth(Math.round(ScaleTransform.getInstance( this).invScaleX(width)));
      itsPatcher.setWindowHeight(Math.round(ScaleTransform.getInstance( this).invScaleY(height)));
      fixSize();
    }
  }
  public void setCurrentWindowSize(int width, int height)
  {
    if (itsPatcher != null) {
      itsPatcher.setWW(Math.round(ScaleTransform.getInstance( this).invScaleX(width)));
      itsPatcher.setWH(Math.round(ScaleTransform.getInstance( this).invScaleY(height)));
      fixSize();
    }
  }
  public void relocateToWindow(int x, int y){
    if (itsPatcher != null) {
	itsPatcher.setWindowX(x);
	itsPatcher.setWindowY(y);
    }
  }

  void stopUpdates(){
    // Do the test because the awt can call this before itsPatcher is ready
    if (itsPatcher != null)
      itsPatcher.stopUpdates();
  }
  void startUpdates(){
    // Do the test because the awt can call this before itsPatcher is ready
    if (itsPatcher != null)
      itsPatcher.startUpdates();
  }

  public void setDirty(boolean dirty)
  {
    getToolBar().setDirty(dirty);
  }

  
  // -----------------------------------------------------------------
  // Undo/Redo
  // -----------------------------------------------------------------
  Vector undoObjects;
  boolean canUndo = false;
  boolean canRedo = false;
  boolean undoing = false;
  boolean undoed = false;
  boolean isRemoveUndo = false;
  boolean isUpdateUndo = false;
  FtsClipboard ftsUndoClipboard;
  ErmesSelection removeSelection;
  String undoType = "";

  void initUndoStuff()
  {
    removeSelection = new ErmesSelection();
    undoObjects = new Vector();

    try
      {
	ftsUndoClipboard = new FtsClipboard();	
      }
    catch(IOException e)
      {
	System.err.println("[ErmesSketchPad]: Error in FtsClipboard creation!");
	e.printStackTrace();
      }
  }

  public void setUndo( String type, boolean remove, boolean update)
  {
    isUpdateUndo = (undoType.equals( type) && update && !undoed);
    isRemoveUndo = remove;
    undoType = type;

    if( !isRemoveUndo)
      {
	if( !isUpdateUndo)
	  {
	    undoObjects.removeAllElements();
	    for( Enumeration e = ErmesSelection.patcherSelection.getSelectedObjects(); e.hasMoreElements(); )
	      addUndoRedoObject( (GraphicObject)e.nextElement());
	    
	    canUndo = (undoObjects.size() > 0);
	    undoed = false;
	  }	
      }
    else
      {
	removeSelection.deselectAll();
	for( Enumeration e = ErmesSelection.patcherSelection.getSelectedObjects(); e.hasMoreElements(); )
	  addUndoRedoObject( (GraphicObject)e.nextElement());
	
	if( removeSelection.getSelectedObjectsSize() > 0)
	  {
	    ftsUndoClipboard.copy( removeSelection.getFtsSelection());
	    canUndo = true;
	  }
	else canUndo = false;  
      }
    canRedo = false;
  }

  public void setUndo( String type, GraphicObject obj, boolean remove, boolean update)
  {
    isUpdateUndo = (undoType.equals( type) && update && !undoed);
    isRemoveUndo = remove;
    undoType = type;

    if( !remove)
      {
	if( !isUpdateUndo)
	  {
	    undoObjects.removeAllElements();
	    addUndoRedoObject( obj);
	    canUndo = true;
	    undoed = false;
	  }	
      }
    else
      {
	removeSelection.deselectAll();
	addUndoRedoObject( obj);	
	ftsUndoClipboard.copy( removeSelection.getFtsSelection());
	canUndo = true;
      }
    canRedo = false;
  }

  public void setRedo()
  {
    canRedo = true;
    canUndo = false;

    if( !isRemoveUndo)
      for( Enumeration e = undoObjects.elements(); e.hasMoreElements();)
	((GraphicObject)e.nextElement()).setRedo();
  }

  void addUndoRedoObject( GraphicObject obj)
  {
    if( isRemoveUndo)
      {
	removeSelection.add( obj);
	// select all connections for this obj
	for( Enumeration e = displayList.getConnectionsFor( obj); e.hasMoreElements();)
	  removeSelection.add( (GraphicConnection)e.nextElement());
      }
    else
      if( !undoObjects.contains( obj))
	{
	  obj.setUndo();
	  undoObjects.addElement( obj);
	}
   }

  public void undo()
  {
    setRedo();

    undoing = true;
    
    if( isRemoveUndo)
      getFtsPatcher().requestPaste( ftsUndoClipboard, 0, 0);
    else
      {
	for( Enumeration e = undoObjects.elements(); e.hasMoreElements();)
	  ((GraphicObject)e.nextElement()).undo();
	
	redraw();
      }
    undoed = true;
  }

  public void redo()
  {
    if( isRemoveUndo)
      {
	if( removeSelection.getSelectedObjectsSize() > 0)
	  {
	    getFtsPatcher().requestDeleteObjects( removeSelection.getSelectedObjects());	
	    for ( Enumeration e = removeSelection.getSelectedObjects(); e.hasMoreElements(); ) 
	      ((DisplayObject) e.nextElement()).delete();
	    removeSelection.deselectAll();
	    getDisplayList().reassignLayers();
	  }
      }
    else
      for( Enumeration e = undoObjects.elements(); e.hasMoreElements();)
	((GraphicObject)e.nextElement()).redo();

    redraw();

    canRedo = false;
    canUndo = true;
  }

  public void resetUndoRedo()
  {
    removeSelection.deselectAll();
    undoObjects.removeAllElements();
    canRedo = false;
    canUndo = false;
    undoing = false;
    undoType = "";
  }

  public boolean canUndo()
  {
    return canUndo;
  }  

  public boolean canRedo()
  {
    return canRedo;
  } 

  public String getUndoType()
  {
    return undoType;
  }

  /* ClipboardOwner interface */
  public void lostOwnership( Clipboard c, Transferable t)
  {
  }
}







