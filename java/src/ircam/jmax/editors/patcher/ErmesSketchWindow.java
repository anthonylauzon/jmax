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
import java.awt.print.*;
import java.awt.event.*;  
import javax.swing.*;
/*import javax.swing.Box;
  import javax.swing.JFrame;
  import javax.swing.JLabel;
  import javax.swing.JMenu;
  import javax.swing.JMenuBar;
  import javax.swing.JScrollPane;
  import javax.swing.JViewport;
  import javax.swing.WindowConstants;*/

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.menus.*;
import ircam.fts.client.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

//
// The window that contains the sketchpad. It knows the ftspatcher it is editing.
// It handles all the sketch menus, it knows how to load from a ftspatcher.
//
public class ErmesSketchWindow extends JFrame implements ComponentListener, WindowListener, EditorContainer, MessageDisplayer
{
  public ErmesSketchPad itsSketchPad;
  JScrollPane  itsScrollerView;
  ToolBar itsToolBar;
  JLabel       itsMessageLabel;
  FtsPatcherObject itsPatcher;

  private FileMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private JMenu itsWindowsMenu;
  private TextMenu itsTextMenu;
  private JMenu itsHelpMenu;
  private JMenuBar mb; 

  static public void initModule()
  {
    RepaintManager.setCurrentManager( new MaxRepaintManager());
    ToolsPanel.registerToolsPanel();
  }

  // ------ editorContainer interface ---------------

  public Editor getEditor(){
    return itsSketchPad;
  }
  public Frame getFrame(){
    return this;
  }
  public Point getContainerLocation(){
    return getLocation();
  }
  public Rectangle getViewRectangle(){
    JViewport port = itsScrollerView.getViewport();
    return port.getViewRect();
  }

  // ------- messageDisplayer interface -------------\

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
  
  /****************************************************************************/
  /*                                                                          */
  /*           CONSTRUCTOR                                                    */
  /*                                                                          */
  /****************************************************************************/

  public ErmesSketchWindow(FtsPatcherObject patcherObj) 
  {
    super("");

    itsPatcher = patcherObj;

    PatcherClipboardManager.createManager();

    // Make the content
    itsSketchPad = new ErmesSketchPad(this, itsPatcher);

    itsToolBar = new ToolBar( itsSketchPad);

    // Make the title
    makeTitle();

    itsScrollerView = new JScrollPane();
    itsScrollerView.setViewportView( itsSketchPad); 
    itsScrollerView.getHorizontalScrollBar().setUnitIncrement( 10);
    itsScrollerView.getVerticalScrollBar().setUnitIncrement( 10);
    itsScrollerView.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    itsScrollerView.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

    // Build The Menus and Menu Bar
    makeMenuBar();

    // inform the Sketch of the ToolBar to 
    itsSketchPad.setToolBar( itsToolBar);

    itsMessageLabel = new JLabel("   ");
    itsMessageLabel.setPreferredSize(new Dimension( patcherObj.getWindowWidth(), 15));

    getContentPane().add( itsToolBar, BorderLayout.NORTH);
    getContentPane().add( itsScrollerView, BorderLayout.CENTER);
    getContentPane().add( itsMessageLabel, BorderLayout.SOUTH);

    // Compute its Initial Size
    setPatcherBounds(itsPatcher); 

    validate();

    addComponentListener( this); 
    addWindowListener(this); 
    
    itsSketchPad.InitLock();
    itsSketchPad.setMessageDisplayer(this);
    
    // Finally, activate the updates
    itsPatcher.startUpdates();

    if(itsSketchPad.getFtsPatcher() instanceof FtsPatcherObject)
	((FtsPatcherObject)itsSketchPad.getFtsPatcher()).setEditorFrame(this);

    setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
  }

  private final void makeTitle()
  {
    setTitle( MaxWindowManager.getWindowManager().makeUniqueWindowTitle( itsSketchPad.getTitle()));
  }

  public void setTitle(String fileName)
  {
    String title;
    
    int id = fileName.lastIndexOf('/');
    
    if( id != -1)
      title = fileName.substring( id + 1);
    else
      title = fileName;

    if(!getTitle().equals( title))
      {
	super.setTitle( MaxWindowManager.getWindowManager().makeUniqueWindowTitle( title));
	MaxWindowManager.getWindowManager().windowChanged( this);
      }
  }

  public void updateTitle()
  {
      makeTitle(); 
  }

  private final void makeMenuBar(){
    
    mb = new JMenuBar();

    // Build the file menu
    itsFileMenu = new FileMenu(itsSketchPad); 
    mb.add(itsFileMenu); 


    // Build the edit menu
    itsEditMenu = new EditMenu(itsSketchPad); 
    mb.add(itsEditMenu); 

    // Build the text menu
    itsTextMenu = new TextMenu(itsSketchPad);
    mb.add( itsTextMenu);

    // Build the Project menu
    mb.add( new DefaultProjectMenu());

    // New Window Manager based Menu
    itsWindowsMenu = new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    // Add some separation between help and the others.
    mb.add(Box.createHorizontalGlue());

    // Build up the help Menu 
    itsHelpMenu = new HelpMenu();
    mb.add( itsHelpMenu);

    setJMenuBar(mb);
  }


  int horizontalOffset()
  {
    return 40;
  }
  
  int verticalOffset() 
  {
    return 130;
  }
  
  public void setPatcherBounds(FtsPatcherObject patcherObj)
  {
    int x;
    int y;
    int width;
    int height;

    //Double check the existence of the window properties. If there aren't, use defaults
    x = patcherObj.getWindowX();
    y = patcherObj.getWindowY();
    width = ScaleTransform.getInstance().scaleX(patcherObj.getWindowWidth());
    height = ScaleTransform.getInstance().scaleY(patcherObj.getWindowHeight());

    if (width <= 0)
      width = ScaleTransform.getInstance().scaleX(480);

    if (height <= 0)
      height =  ScaleTransform.getInstance().scaleY(500);

    setBounds( x, y, width + horizontalOffset(), height + verticalOffset());
  }

  public void fontBaseChanged(int newFontBase)
  {
      int x;
      int y;
      int width;
      int height;

      width = ScaleTransform.getInstance().scaleX(itsPatcher.getWindowWidth());
      height = ScaleTransform.getInstance().scaleY(itsPatcher.getWindowHeight());
  
      setSize( width + horizontalOffset(), height + verticalOffset());
  }

  public void scale(float scaleX, float scaleY)
  {
     int x;
     int y;
     int width;
     int height;

     itsSketchPad.scale(scaleX, scaleY);

     width =  Math.round((getSize().width-horizontalOffset())*scaleX);
     height = Math.round((getSize().height-verticalOffset())*scaleY);
  
     setSize( width + horizontalOffset(), height + verticalOffset()); 
  }

  public void Destroy()
  {
    Hide();

    removeComponentListener( this);
    removeWindowListener(this);

    if(itsSketchPad.getFtsPatcher() instanceof FtsPatcherObject)
	((FtsPatcherObject)itsSketchPad.getFtsPatcher()).setEditorFrame(null);

    itsSketchPad.cleanAll();

    itsSketchPad = null;
    itsScrollerView = null;

    dispose();
  }
  public void Hide()
  {
    setVisible(false);
    MaxWindowManager.getWindowManager().removeWindow(this);
  }

  public void setCursor(Cursor cursor)
  {
    super.setCursor(cursor);
    itsSketchPad.setCursor(cursor);
  }

  public ErmesSketchPad getSketchPad()
  {
    return itsSketchPad;
  }

  /****************************************************************************/
  /*                                                                          */
  /*           LISTENERS                                                      */
  /*                                                                          */
  /****************************************************************************/
  // Window Listener Interface 
  public void windowClosing(WindowEvent e)
  {
    itsSketchPad.close(false);    
  }

  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}

  public void windowIconified( WindowEvent e)
  {
    itsSketchPad.stopUpdates();
  }       

  public void windowDeiconified( WindowEvent e)
  {
    itsSketchPad.startUpdates();
  }       

  public void windowActivated(WindowEvent e)
  {
    itsSketchPad.requestFocus();
  }

  public void windowDeactivated(WindowEvent e){}

  // Component Listener Interface
  public void componentResized( ComponentEvent e) 
  {
    /* not loading patch or opening subpatcher */
    if( itsSketchPad.getFtsPatcher().getWindowWidth() > 0)
      itsSketchPad.resizeToWindow( getSize().width - horizontalOffset(), getSize().height - verticalOffset());
  }

  public void componentMoved( ComponentEvent e) 
  {
    itsSketchPad.relocateToWindow(getLocation().x, getLocation().y);
  }

  public void componentShown( ComponentEvent e){}

  public void componentHidden( ComponentEvent e){}
}











