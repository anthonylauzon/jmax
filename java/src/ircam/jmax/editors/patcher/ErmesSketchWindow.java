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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.menus.*;

//
// The window that contains the sketchpad. It knows the ftspatcher it is editing.
// It handles all the sketch menus, it knows how to load from a ftspatcher.
//
public class ErmesSketchWindow extends JFrame implements ComponentListener, WindowListener
{
  public ErmesSketchPad itsSketchPad;
  JScrollPane  itsScrollerView;
  ErmesToolBar itsToolBar;
  JLabel       itsMessageLabel;
  public FtsObject itsPatcher;
  public FtsPatcherData itsPatcherData;

  private FileMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private JMenu itsWindowsMenu;
  private JMenu itsToolsMenu;
  private TextMenu itsTextMenu;
  private JMenu itsHelpMenu;

  private Fts fts;

  public MaxDocument itsDocument;

  public void showObject( Object obj)
  {
    itsSketchPad.showObject( obj);
  }

  /****************************************************************************/
  /*                                                                          */
  /*           CONSTRUCTOR                                                      */
  /*                                                                          */
  /****************************************************************************/

  public ErmesSketchWindow( FtsPatcherData patcherData) 
  {
    super("");

    fts = patcherData.getFts();

    MaxWindowManager.getWindowManager().addWindow(this);

    // Create the ClipboardManager
    PatcherClipboardManager.createManager();
    
    // Initialize state
    itsDocument = patcherData.getDocument();
    itsPatcher = patcherData.getContainerObject();
    itsPatcherData = patcherData;

    itsPatcherData.setPatcherListener(new ErmesPatcherListener(this));

    // Make the title
    makeTitle();

    // Make the content
    itsSketchPad = new ErmesSketchPad(fts, this, itsPatcherData);
    itsToolBar = new ErmesToolBar( itsSketchPad);

    itsScrollerView = new JScrollPane();
    itsScrollerView.setViewportView( itsSketchPad); 
    itsScrollerView.getHorizontalScrollBar().setUnitIncrement( 10);
    itsScrollerView.getVerticalScrollBar().setUnitIncrement( 10);
    itsScrollerView.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    itsScrollerView.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

    // Build The Menus and Menu Bar
    JMenuBar mb = new JMenuBar();

    // Build the file menu
    itsFileMenu = new FileMenu();
    mb.add(itsFileMenu); 

    // Build the edit menu
    itsEditMenu = new EditMenu(this); 
    mb.add(itsEditMenu); 

    // Build the text menu
    itsTextMenu = new TextMenu(this);
    mb.add( itsTextMenu);

    // New Tool menu 
    itsToolsMenu = new ircam.jmax.toolkit.MaxToolsJMenu("Tools"); 
    mb.add(itsToolsMenu);

    // New Window Manager based Menu
    itsWindowsMenu = new ircam.jmax.toolkit.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    // Add some separation between help and the others.
    mb.add(Box.createHorizontalGlue());

    // Build up the help Menu 
    itsHelpMenu = new HelpMenu(this);
    mb.add( itsHelpMenu);

    setJMenuBar(mb);
    
    // inform the Sketch of the ToolBar to 
    itsSketchPad.setToolBar( itsToolBar);

    itsMessageLabel = new JLabel("   ");

    getContentPane().add( itsToolBar, BorderLayout.NORTH);
    getContentPane().add( itsScrollerView, BorderLayout.CENTER);
    getContentPane().add( itsMessageLabel, BorderLayout.SOUTH);

    // Compute its Initial Size
    InitFromContainer( itsPatcher); 

    validate();

    addComponentListener( this); 
    addWindowListener(this); 
    
    // To set the initial state: set to edit mode only if the
    // initialMode property of a patcher is set and it is set
    // to something different than "run" (usually, "edit" :)
    if (itsPatcherData.getRecursiveEditMode() == FtsPatcherData.EDIT_MODE)
      setLocked( false);
    else
      setLocked( true);

    // Fix the sketch size if needed ????
    itsSketchPad.fixSize();

    // Finally, activate the updates
    itsPatcherData.startUpdates();

    // Make it visible, at the end
    setVisible( true);
  }

  private final void makeTitle()
  {
    String name;

    if (itsDocument.isRootData(itsPatcherData))
      name = itsDocument.getName();
    else if (itsPatcher instanceof FtsPatcherObject)
      {
	name = "patcher " + itsPatcher.getDescription();
      }
    else
      name = "template " + itsPatcher.getClassName();

    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle(name));

    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  int horizontalOffset()
  {
    return 40;
  }
  
  int verticalOffset() 
  {
    return 130;
  }
  
  private void InitFromContainer( FtsObject patcher)
  {
    int x;
    int y;
    int width;
    int height;

    //Double check the existence of the window properties. If there aren't, use defaults
      
    x = itsPatcherData.getWindowX();
    y = itsPatcherData.getWindowY();
    width = itsPatcherData.getWindowWidth();
    height = itsPatcherData.getWindowHeight();

    if (width <= 0)
      width = 480;

    if (height <= 0)
      height = 500;

    setBounds( x, y, width + horizontalOffset(), height + verticalOffset());
  }

  public Fts getFts()
  {
    return fts;
  }

  public ErmesSketchPad getSketchPad(){
    return itsSketchPad;
  }
  public MaxDocument getDocument(){
    return itsDocument;
  }
  public FtsPatcherData getPatcherData(){
    return itsPatcherData;
  }
  /****************************************************************************/
  /*                                                                          */
  /*           MENU ACTIONS                                                   */
  /*                                                                          */
  /****************************************************************************/

  public void Close(boolean doCancel)
  {
    if (! itsDocument.isRootData(itsPatcherData))
      {
	itsPatcherData.stopUpdates();
	Mda.dispose(itsPatcherData); // experimental
      }
    else 
      {
	if(PatcherSaveManager.SaveClosing(this, doCancel))
	  itsDocument.dispose();
	// Just call dispose on the document
	// Mda will indirectly call Destroy, and will close all the other editors
      }
  }

  // Method to close the editor (do not touch the patcher)
  public void Destroy()
  {
    setVisible(false);

    itsPatcherData.resetPatcherListener();
    removeComponentListener( this);
    removeWindowListener(this);
    itsSketchPad.cleanAll();

    itsSketchPad = null;
    itsPatcher = null;
    itsPatcherData = null;
    itsDocument = null;

    itsScrollerView = null;

    dispose();
  }

  /****************************************************************************/
  /*                                                                          */
  /*           LOCK MODE HANDLING                                             */
  /*                                                                          */
  /****************************************************************************/

  public void setLocked( boolean locked)
  {
    // Store the mode in a non persistent, property of 
    // the patch, so that subpatcher can use it as their initial mode

    if (locked)
      itsPatcherData.setEditMode(FtsPatcherData.RUN_MODE);
    else
      itsPatcherData.setEditMode(FtsPatcherData.EDIT_MODE);

    itsSketchPad.setLocked( locked); 

    itsToolBar.setLocked( locked);

    itsSketchPad.setKeyEventClient( null); //when changing mode, always remove key listeners

    itsSketchPad.requestFocus();
  }

  final public boolean isLocked()
  {
    return itsSketchPad.isLocked();
  }

  /****************************************************************************/
  /*                                                                          */
  /*           LISTENERS                                                      */
  /*                                                                          */
  /****************************************************************************/
  // Window Listener Interface 
  public void windowClosing(WindowEvent e)
  {
    Close(false);
  }

  public void windowOpened(WindowEvent e)
  {
  }

  public void windowClosed(WindowEvent e)
  {
  }

  public void windowIconified( WindowEvent e)
  {
    // Do the test because the awt can call this before itsPatcher is ready
    if (itsPatcherData != null)
      itsPatcherData.stopUpdates();
  }       

  public void windowDeiconified( WindowEvent e)
  {
    // Do the test because the awt can call this before itsPatcher is ready
    if (itsPatcherData != null)
      itsPatcherData.startUpdates();
  }       

  public void windowActivated(WindowEvent e)
  {
    itsSketchPad.requestFocus();
  }

  public void windowDeactivated(WindowEvent e)
  {
  }

  // Component Listener Interface
  public void componentResized( ComponentEvent e) 
  {
    if (itsPatcher != null) 
      {
	itsPatcherData.setWindowWidth(getSize().width - horizontalOffset());
	itsPatcherData.setWindowHeight(getSize().height - verticalOffset());
	itsSketchPad.fixSize();
      }
  }

  public void componentMoved( ComponentEvent e) 
  {
    if (itsPatcher != null)
      {
	itsPatcherData.setWindowX(getLocation().x);
	itsPatcherData.setWindowY(getLocation().y);
      }
  }

  public void componentShown( ComponentEvent e) 
  {
  }

  public void componentHidden( ComponentEvent e)
  {
  }

  boolean gotMessage = false;

  public void showMessage(String text)
  {
    itsMessageLabel.setText(text);
    gotMessage = true;
  }

  public void resetMessage()
  {
    if (gotMessage)
      {
	itsMessageLabel.setText("    ");
	gotMessage = false;
      }
  }

  public boolean isMessageReset()
  {
    return (! gotMessage);
  }
}











