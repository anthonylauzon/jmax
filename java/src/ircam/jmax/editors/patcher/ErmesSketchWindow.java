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

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.menus.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import ircam.jmax.script.ScriptMenu;
//
// The window that contains the sketchpad. It knows the ftspatcher it is editing.
// It handles all the sketch menus, it knows how to load from a ftspatcher.
//
public class ErmesSketchWindow extends JFrame implements ComponentListener, WindowListener, EditorContainer, MessageDisplayer
{
  public ErmesSketchPad itsSketchPad;
  JScrollPane  itsScrollerView;
  ErmesToolBar itsToolBar;
  JLabel       itsMessageLabel;
  FtsPatcherData itsPatcherData;

  private FileMenu itsFileMenu;
  private EditMenu itsEditMenu;	
  private ScriptMenu itsScriptMenu;	
  private JMenu itsWindowsMenu;
  private TextMenu itsTextMenu;
  private JMenu itsHelpMenu;

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
  
    public static void touch(Fts fts)
    {	
      MaxDocument doc = Mda.getDocumentTypeByName( "patcher").newDocument(fts);
      FtsPatcherData data = (FtsPatcherData)(doc).getRootData();
      ErmesSketchWindow win = new ErmesSketchWindow(data);
      MaxWindowManager.getWindowManager().removeWindow(win);
      win.itsSketchPad.Close(false);
      win.dispose();
    }

  // -----------------------------------------------

  /****************************************************************************/
  /*                                                                          */
  /*           CONSTRUCTOR                                                      */
  /*                                                                          */
  /****************************************************************************/

  public ErmesSketchWindow( FtsPatcherData patcherData) 
  {
    super("");

    itsPatcherData = patcherData;

    MaxWindowManager.getWindowManager().addWindow(this);

    // Create the ClipboardManager
    PatcherClipboardManager.createManager();
    
    // Make the content
    itsSketchPad = new ErmesSketchPad(this, patcherData);

    itsToolBar = new ErmesToolBar( itsSketchPad);

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

    getContentPane().add( itsToolBar, BorderLayout.NORTH);
    getContentPane().add( itsScrollerView, BorderLayout.CENTER);
    getContentPane().add( itsMessageLabel, BorderLayout.SOUTH);

    // Compute its Initial Size
    InitFromContainer( patcherData); 

    validate();

    addComponentListener( this); 
    addWindowListener(this); 
    
    itsSketchPad.InitLock();
    itsSketchPad.setMessageDisplayer(this);
    
    patcherData.getFts().addUpdateGroupListener(itsSketchPad);//bug fix 11/5 

    // Finally, activate the updates
    patcherData.startUpdates();

    if(itsSketchPad.getFtsPatcher() instanceof FtsPatcherObject)
	((FtsPatcherObject)itsSketchPad.getFtsPatcher()).setEditorFrame(this);

    // Make it visible, at the end
    //setVisible( true);
  }

  private final void makeTitle()
  {
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle(itsSketchPad.getTitle()));
    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  private final void makeMenuBar(){
    
    JMenuBar mb = new JMenuBar();

    // Build the file menu
    itsFileMenu = new FileMenu(itsSketchPad); 
    mb.add(itsFileMenu); 


    // Build the edit menu
    itsEditMenu = new EditMenu(itsSketchPad); 
    mb.add(itsEditMenu); 

    // Build the text menu
    itsTextMenu = new TextMenu(itsSketchPad);
    mb.add( itsTextMenu);

    // Build the script menu
    //itsScriptMenu = new ScriptMenu();
    //mb.add( itsScriptMenu);

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
  
  private void InitFromContainer( FtsPatcherData patcherData)
  {
    int x;
    int y;
    int width;
    int height;

    //Double check the existence of the window properties. If there aren't, use defaults
      
    x = patcherData.getWindowX();
    y = patcherData.getWindowY();
    width = ScaleTransform.getInstance().scaleX(patcherData.getWindowWidth());
    height = ScaleTransform.getInstance().scaleY(patcherData.getWindowHeight());

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

      width = ScaleTransform.getInstance().scaleX(itsPatcherData.getWindowWidth());
      height = ScaleTransform.getInstance().scaleY(itsPatcherData.getWindowHeight());
  
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
    
    setVisible(false);

    removeComponentListener( this);
    removeWindowListener(this);

    if(itsSketchPad.getFtsPatcher() instanceof FtsPatcherObject)
	((FtsPatcherObject)itsSketchPad.getFtsPatcher()).setEditorFrame(null);

    itsSketchPad.cleanAll();

    itsSketchPad = null;
    itsScrollerView = null;

    dispose();
  }

  /****************************************************************************/
  /*                                                                          */
  /*           LISTENERS                                                      */
  /*                                                                          */
  /****************************************************************************/
  // Window Listener Interface 
  public void windowClosing(WindowEvent e)
  {
    itsSketchPad.Close(false);    
  }

  public void windowOpened(WindowEvent e)
  {
  }

  public void windowClosed(WindowEvent e)
  {
  }

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

  public void windowDeactivated(WindowEvent e)
  {
  }

  // Component Listener Interface
  public void componentResized( ComponentEvent e) 
  {
    itsSketchPad.resizeToWindow(getSize().width - horizontalOffset(), getSize().height - verticalOffset());
  }

  public void componentMoved( ComponentEvent e) 
  {
    itsSketchPad.relocateToWindow(getLocation().x, getLocation().y);
  }

  public void componentShown( ComponentEvent e) 
  {
  }

  public void componentHidden( ComponentEvent e)
  {
  }
}











