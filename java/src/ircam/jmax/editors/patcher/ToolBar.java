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

// import javax.swing.*;
import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JToggleButton;

// import javax.swing.border.*;
import javax.swing.border.EmptyBorder;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.objects.*;

public class ToolBar extends JPanel implements ComponentListener{

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
  ToolButton selectedButton = null;
  int numButtons = 0;
  boolean dirty = false;

  public ToolBar( ErmesSketchPad theSketchPad)
  {
    sketch = theSketchPad;

    setDoubleBuffered( false);

    setLayout( new BorderLayout());    

    noneButton = new JToggleButton();
    bGroup = new ButtonGroup();
    bGroup.add( noneButton);

    lockEditButton = new JToggleButton( JMaxIcons.lockMode);
    lockEditButton.setDoubleBuffered( false);
    lockEditButton.setMargin( new Insets(0,0,0,0));
    lockEditButton.setSelectedIcon( JMaxIcons.editMode);
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
    toSaveButton = new JButton( JMaxIcons.toSave);
    toSaveButton.setDoubleBuffered( false);
    toSaveButton.setMargin( new Insets(0,0,0,0));
    toSaveButton.addActionListener( new ActionListener() {
	public void actionPerformed( ActionEvent e)
	{
	  PatcherSaveManager.save(sketch.getEditorContainer());
	}
      });
    toSaveButton.setToolTipText("save file");
    this.dirty = sketch.getFtsPatcher().isDirty();
    toSaveButton.setEnabled(dirty);
    toSaveButton.setVisible(dirty);

    widgets.add( toSaveButton);
    /////////////////////

    if ((sketch.getFtsPatcher().getParent() != null) &&
	(!sketch.isARootPatcher()))
      {
	upButton = new JButton( JMaxIcons.up);
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

  public ErmesSketchPad getSketchPad()
  {
    return sketch;
  }

  void buttonSelected(ToolButton button)
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

  private void addButton( String className)
  {
    JToggleButton button = new ToolButton( this, className);
    toolBar.add( button);
    if(!AddPopUp.initDone)
      AddPopUp.addAbbreviation( className, JMaxClassMap.getIcon( className), true);
	
    bGroup.add( button);
    numButtons++;
  }

  private void insertButtons()
  {
    for ( Iterator i = JMaxClassMap.getClassNames(); i.hasNext(); )
      addButton( (String)i.next());

    AddPopUp.initDone();
  }

  public void setDirty(boolean dirty)
  {
    if(this.dirty != dirty)
      {
	toSaveButton.setEnabled(dirty);
	toSaveButton.setVisible(dirty);
	if(!dirty) repaint();
	this.dirty = dirty;
      }
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
	ToolButton button;
	int where = numButtons - removedButtons.size();
	int toRemove = numPopButtons-removedButtons.size();

	for(int i = 0; i < toRemove; i++)
	  {
	    if(where-2 >= 0)
	      {
		button = (ToolButton)(toolBar.getComponent(where-2));
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
	ToolButton button;
	int where = removedButtons.size()-numPopButtons;
	toolBar.remove(moreButton);
	  
	for(int i = 0; i<where; i++)
	  {
	    button = (ToolButton)(removedButtons.lastElement());
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
