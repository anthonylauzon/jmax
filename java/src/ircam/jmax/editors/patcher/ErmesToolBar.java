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
import java.util.*;
import java.io.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.plaf.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.objects.*;

public class ErmesToolBar extends JPanel implements /*MaxDocumentListener,*/ ComponentListener{

  ErmesSketchPad sketch;

  private boolean staySelected = false;
  boolean pressed = false;

  JPanel toolBar;
  CardLayout cardLayout;
  JPanel cards;
  JToggleButton lockEditButton;
  JButton upButton;
  JButton toSaveButton;

  ButtonGroup bGroup;
  JToggleButton noneButton;
  ErmesToolButton selectedButton = null;
  int numButtons = 0;

  public ErmesToolBar( ErmesSketchPad theSketchPad)
  {
    sketch = theSketchPad;

    //sketch.getDocument().addListener(this);

    setDoubleBuffered( false);

    setLayout( new BorderLayout());    

    noneButton = new JToggleButton();
    bGroup = new ButtonGroup();
    bGroup.add( noneButton);

    lockEditButton = new JToggleButton( SystemIcons.get( "_lock_mode_"));
    lockEditButton.setDoubleBuffered( false);
    lockEditButton.setMargin( new Insets(0,0,0,0));
    lockEditButton.setSelectedIcon( SystemIcons.get( "_edit_mode_"));
    lockEditButton.setFocusPainted( false);
    lockEditButton.addItemListener( new ItemListener() {
	public void itemStateChanged(ItemEvent e)
	{
	  if (e.getStateChange() == ItemEvent.SELECTED)
	    sketch.setLocked( false);
	  else
	    sketch.setLocked( true);
	}
      });

    add( lockEditButton, BorderLayout.WEST);
    numButtons++;

    //widgets panel
    JPanel widgets = new JPanel();
    widgets.setBorder( new EmptyBorder( 0, 0, 0, 0));
    widgets.setLayout( new BoxLayout( widgets, BoxLayout.X_AXIS));
    
    //to save button
    toSaveButton = new JButton(SystemIcons.get( "_to_save_"));
    toSaveButton.setDoubleBuffered( false);
    toSaveButton.setMargin( new Insets(0,0,0,0));
    toSaveButton.addActionListener( new ActionListener() {
	public void actionPerformed( ActionEvent e)
	{
	  PatcherSaveManager.save(sketch.getEditorContainer());
	}
      });
    toSaveButton.setToolTipText("save file");
    toSaveButton.setEnabled(false);
    toSaveButton.setVisible(false);

    widgets.add( toSaveButton);
    /////////////////////

    if ((sketch.getFtsPatcher().getParent() != null) &&
	(!sketch.isARootPatcher()))
      {
	upButton = new JButton(SystemIcons.get( "_up_"));
	upButton.setDoubleBuffered( false);
	upButton.setMargin( new Insets(0,0,0,0));
	upButton.setToolTipText("show parent patcher");
	upButton.addActionListener( new ActionListener() {
	    public void actionPerformed( ActionEvent e)
	    {
	      sketch.waiting();
	      ((FtsPatcherObject)sketch.getFtsPatcher().getParent()).requestOpenEditor();
	      sketch.getFtsPatcher().requestStopWaiting(null);
	    }});

	widgets.add( upButton);
      }
    add( widgets, BorderLayout.EAST);

    cards = new JPanel();
    cards.setBorder( new EmptyBorder( 0, 0, 0, 0));

    cardLayout = new CardLayout();    
    cards.setLayout( cardLayout);    

    add( cards, BorderLayout.CENTER);

    JPanel empty = new JPanel();

    cards.add( "lock", empty);
    cards.setBorder( new EmptyBorder( 0, 0, 0, 0));
 
    toolBar = new JPanel();
    toolBar.setBorder( new EmptyBorder( 0, 0, 0, 0));
    toolBar.setLayout( new BoxLayout( toolBar, BoxLayout.X_AXIS));
    insertButtons();

    cards.add( "edit", toolBar);

    bGroup = new ButtonGroup();

    addComponentListener(this);
  }

  void buttonSelected(ErmesToolButton button)
  {
    if(selectedButton!=null)
      selectedButton.reset();
    selectedButton = button;
    sketch.setAddModeInteraction(selectedButton.getDescription(), selectedButton.getMessage(), true);
  }

  public void reset()
  {
    if(selectedButton!=null){
      selectedButton.reset();
      selectedButton = null;
    }
    noneButton.setSelected(true);
  }

  protected void setLocked( boolean locked)
  {
    if ( locked)
      {
	if (lockEditButton.isSelected())
	  lockEditButton.setSelected( false);
      }
    else
      {
	if ( !lockEditButton.isSelected())
	  lockEditButton.setSelected( true);
      }

    cardLayout.show( cards, locked ? "lock" : "edit");
  }

  private void addButton( String descr, String iconName, String cursorName, String message)
  {
    JToggleButton button = new ErmesToolButton(this, descr, SystemIcons.get(iconName), cursorName, message);
    toolBar.add( button);
    if(!AddPopUp.initDone)
      AddPopUp.addAbbreviation(iconName, descr, message, true);

    bGroup.add( button);
    numButtons++;
  }

  private void addButton(String className, String pname)
  {
    /*
      WARNING:
      Waiting for a method to get the packagePath from the package name
    */
    /*String path;
      try{
      path = MaxApplication.getPackageHandler().locatePackage(pname).getPath();	 
      }
      catch(FileNotFoundException e){
      path = MaxApplication.getProperty(pname+"PackageDir");    
      }*/
    String path = MaxApplication.getProperty("jmaxRoot")+"/packages/"+pname;//??????????????????	 
    /*************************************************************/

    if(path != null) path = path+File.separator+"images"+File.separator;

    if(SystemIcons.get(className) == null)
      SystemIcons.loadIcon(className, path+className+".gif");

    ImageIcon icon = SystemIcons.get(className);
    JToggleButton button = new ErmesToolButton(this, className, icon, path+className+"_cursor.gif",
					       "Adding New "+className+" Object");
    toolBar.add( button);
    if(!AddPopUp.initDone)
      AddPopUp.addAbbreviation(className, icon, true);
	
    bGroup.add( button);
    numButtons++;
  }

  private void insertButtons()
  {
    String path = MaxApplication.getProperty("jmaxRoot")+File.separator+"images"+File.separator;

    addButton( "", "_object_", path+"cursor_standard.gif", "Adding New Object");
    addButton( "jpatcher", "_patcher_", path+"cursor_patcher.gif", "Adding New jpatcher");
    addButton( "inlet -1", "_inlet_", path+"cursor_in.gif", "Adding New Inlet");
    addButton( "outlet -1","_outlet_", path+"cursor_out.gif", "Adding New Outlet");

    String cname;
    for(Enumeration en = ObjectCreatorManager.getClassNames(); en.hasMoreElements();)
      {
	cname = (String)en.nextElement();
	addButton(cname, ObjectCreatorManager.getPackageName(cname));
      }
    AddPopUp.initDone();//????
  }

  // Component Listener Interface
  Vector removedButtons = new Vector();
  final static int BUTTON_WIDTH = 21;
  MoreObjectsButton moreButton = null;
  public void componentResized( ComponentEvent e) 
  {
    int toolbarwidth = getSize().width - 2*BUTTON_WIDTH-10;
    int visibleButtons = ((toolbarwidth/BUTTON_WIDTH)> numButtons)? numButtons: toolbarwidth/BUTTON_WIDTH;
    int numPopButtons = numButtons - visibleButtons;

    if(numPopButtons == 1) 
      {
	numPopButtons = 0;
	visibleButtons++;
      }
    if(numPopButtons==removedButtons.size()) return;

    if(numPopButtons > removedButtons.size())
      {
	ErmesToolButton button;
	int where = numButtons - removedButtons.size();
	int toRemove = numPopButtons-removedButtons.size();

	for(int i = 0; i < toRemove; i++)
	  {
	    if(where-2 >= 0)
	      {
		button = (ErmesToolButton)(toolBar.getComponent(where-2));
		toolBar.remove(where-2);
		removedButtons.add(button);
		where--;	   
	      }   
	  }
	if(moreButton!=null)
	  moreButton.update(removedButtons.elements(), removedButtons.size());
	else
	  {
	    moreButton = new MoreObjectsButton(removedButtons.elements());
	    toolBar.add(moreButton);
	  }
      }
    else
      {
	ErmesToolButton button;
	int where = removedButtons.size()-numPopButtons;
	toolBar.remove(moreButton);
	  
	for(int i = 0; i<where; i++)
	  {
	    button = (ErmesToolButton)(removedButtons.lastElement());
	    toolBar.add(button);
	    removedButtons.remove(button);
	  }
	if(removedButtons.size()>0)
	  {
	    toolBar.add(moreButton);
	    moreButton.update(removedButtons.elements(), removedButtons.size());
	  }
	else
	  moreButton=null;
      }
    toolBar.validate();
  }
  public void componentMoved( ComponentEvent e){}
  public void componentShown( ComponentEvent e){}
  public void componentHidden( ComponentEvent e){}
}







