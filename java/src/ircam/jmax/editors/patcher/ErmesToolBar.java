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
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;

public class ErmesToolBar extends JPanel implements MaxDocumentListener{

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

  public ErmesToolBar( ErmesSketchPad theSketchPad)
  {
    sketch = theSketchPad;

    sketch.getDocument().addListener(this);

    setDoubleBuffered( false);

    setLayout( new BorderLayout());    

    noneButton = new JToggleButton();
    bGroup = new ButtonGroup();
    bGroup.add( noneButton);

    lockEditButton = new JToggleButton( Icons.get( "_lock_mode_"));
    lockEditButton.setDoubleBuffered( false);
    lockEditButton.setMargin( new Insets(0,0,0,0));
    lockEditButton.setSelectedIcon( Icons.get( "_edit_mode_"));
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

    //widgets panel
    JPanel widgets = new JPanel();
    widgets.setBorder( new EmptyBorder( 0, 0, 0, 0));
    widgets.setLayout( new BoxLayout( widgets, BoxLayout.X_AXIS));
    
    //to save button
    toSaveButton = new JButton(Icons.get( "_to_save_"));
    toSaveButton.setDoubleBuffered( false);
    toSaveButton.setMargin( new Insets(0,0,0,0));
    toSaveButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
	{
	  PatcherSaveManager.Save(sketch.getEditorContainer());
	}
    });

    toSaveButton.setEnabled(false);
    toSaveButton.setVisible(false);

    widgets.add( toSaveButton);
    /////////////////////

    if ((sketch.itsPatcher.getParent() != null) &&
	(!sketch.isARootPatcher()))
      {
	upButton = new JButton(Icons.get( "_up_"));
	upButton.setDoubleBuffered( false);
	upButton.setMargin( new Insets(0,0,0,0));
	upButton.addActionListener( new ActionListener() {
	  public void actionPerformed( ActionEvent e)
	    {
	      sketch.waiting();
	      sketch.getFts().editPropertyValue(sketch.itsPatcher.getParent(), sketch.itsPatcher, 
						new MaxDataEditorReadyListener()
						{
						  public void editorReady(MaxDataEditor editor)
						    {sketch.stopWaiting();}
						});
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

  private void addButton( String descr, String iconName, String message)
  {
    JToggleButton button = new ErmesToolButton(this, descr, Icons.get(iconName), message);
    toolBar.add( button);
    if(!AddPopUp.initDone)
      AddPopUp.addAbbreviation(iconName, descr, message, true);

    bGroup.add( button);
  }

  private void insertButtons()
  {
    addButton( "", "_object_", "Adding New Object");
    addButton( "messbox", "_message_box_", "Adding New Message Box");
    addButton( "jpatcher", "_patcher_", "Adding New Patcher");
    addButton( "inlet -1", "_inlet_", "Adding New Inlet");
    addButton( "outlet -1","_outlet_",  "Adding New Outlet");
    addButton( "comment", "_comment_","Adding New Comment");
    addButton( "button",  "_button_", "Adding New Button");
    addButton( "toggle",  "_toggle_", "Adding New Toggle");
    addButton( "slider",  "_slider_", "Adding New Slider");
    addButton( "intbox",   "_intbox_", "Adding New Integer Box");
    addButton( "floatbox", "_floatbox_", "Adding New Float Box");
    AddPopUp.initDone();//????
  }

  //MaxDocumentListener interface
  boolean isSaved = true;
  public void documentChanged(boolean saved)
  {
    if(isSaved!=saved)
      {
	toSaveButton.setEnabled(!saved);
	toSaveButton.setVisible(!saved);
	if(saved) repaint();
	isSaved = saved;
      }
  }
}

