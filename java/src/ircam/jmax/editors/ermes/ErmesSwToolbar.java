//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.ermes;

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

public class ErmesSwToolbar extends JPanel implements MouseListener {

  ErmesSketchPad itsSketchPad;

  private boolean staySelected = false;
  boolean pressed = false;

  CardLayout cardLayout;
  JPanel cards;
  JToggleButton lockEditButton;
  JButton upButton;

  JToggleButton itsLastPressed;

  public ErmesSwToolbar( ErmesSketchPad theSketchPad)
  {
    itsSketchPad = theSketchPad;

    setDoubleBuffered( false);

    setLayout( new BorderLayout());    

    lockEditButton = new JToggleButton( Icons.get( "_lock_mode_"));
    lockEditButton.setDoubleBuffered( false);
    lockEditButton.setMargin( new Insets(0,0,0,0));
    lockEditButton.setSelectedIcon( Icons.get( "_edit_mode_"));
    lockEditButton.setFocusPainted( false);
    lockEditButton.addItemListener( new ItemListener() {
      public void itemStateChanged(ItemEvent e)
	{
	  if (e.getStateChange() == ItemEvent.SELECTED)
	    itsSketchPad.itsSketchWindow.setLocked( false);
	  else
	    itsSketchPad.itsSketchWindow.setLocked( true);
	}
    });

    add( lockEditButton, BorderLayout.WEST);

    if ((itsSketchPad.itsPatcher.getParent() != null) &&
	(itsSketchPad.itsPatcher.getParent() != Fts.getRootObject()))
      {
	upButton = new JButton(Icons.get( "_up_"));
	upButton.setDoubleBuffered( false);
	upButton.setMargin( new Insets(0,0,0,0));
	upButton.addActionListener( new ActionListener() {
	  public void actionPerformed( ActionEvent e)
	    {
	      itsSketchPad.waiting();
	      Fts.editPropertyValue(itsSketchPad.itsPatcher.getParent(), itsSketchPad.itsPatcher, 
				    new MaxDataEditorReadyListener()
				    {
				      public void editorReady(MaxDataEditor editor)
					{itsSketchPad.stopWaiting();}
				    });
	    }});

	add( upButton, BorderLayout.EAST);
      }

    cards = new JPanel();
    cards.setBorder( new EmptyBorder( 0, 0, 0, 0));

    cardLayout = new CardLayout();    
    cards.setLayout( cardLayout);    

    add( cards, BorderLayout.CENTER);

    JPanel empty = new JPanel();
    cards.add( "lock", empty);
    cards.setBorder( new EmptyBorder( 0, 0, 0, 0));

    JPanel toolbar = new JPanel();
    toolbar.setBorder( new EmptyBorder( 0, 0, 0, 0));
    toolbar.setLayout( new BoxLayout( toolbar, BoxLayout.X_AXIS));
    insertButtons( toolbar);

    cards.add( "edit", toolbar);
  }

  void SelectAButton(ErmesSwToggleButton theButton)
  {
    theButton.setSelected(true);
    Deselect();
    itsLastPressed = theButton; 
    pressed = true;
  }

  //
  // The action taken when a toolBar button is pressed...
  //
  public void mousePressed(MouseEvent e)
  {
    ErmesSwToggleButton aTButton = (ErmesSwToggleButton) e.getSource();
    
    if (e.getClickCount() > 1)
      return;

    if (aTButton == itsLastPressed)
      {
	itsSketchPad.DoNothing();
	staySelected = false;
	pressed = false;
      }
    else
      { 
	Deselect(); //deselect the last
	staySelected = false;
	itsLastPressed = aTButton;
	pressed = true;

	itsSketchPad.startAdd( aTButton.getNewObjectDescription());
      }
  }

  public void mouseClicked( MouseEvent e)
  {
    if ( e.getClickCount() > 1)
      {
	pressed = true;

	ErmesSwToggleButton aTButton = (ErmesSwToggleButton) e.getSource();

	//aTButton.setSelected(true);
	if (itsLastPressed != aTButton)
	  {
	    itsLastPressed.setSelected( false);
	    itsLastPressed = aTButton;   
	  }

	itsSketchPad.startAdd( aTButton.getNewObjectDescription());
	aTButton.setSelected( true);
	staySelected = true;
      }
  }
  
  public void mouseReleased(MouseEvent e)
  {
    if ( !itsLastPressed.contains( e.getX(), e.getY()))
      {
	itsLastPressed = null;
	pressed = false;
	itsSketchPad.DoNothing();
      }
  }

  public void mouseEntered(MouseEvent e)
  {
  }

  public void mouseExited(MouseEvent e)
  {
  }

  public void Deselect()
  {
    if ( itsLastPressed != null)
      itsLastPressed.setSelected( false);
    itsLastPressed = null;
    pressed = false;
  }

  protected void setLocked( boolean locked)
  {
    if ( pressed)
      {
	resetStaySelected();
      }

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

  protected boolean staySelected()
  {
    return staySelected;
  }

  protected void resetStaySelected()
  {
    Deselect();
    itsSketchPad.DoNothing();
    staySelected = false;
    pressed = false;
  }

  private void addButton( Container toolbar, String descr, String iconName)
  {
    ErmesSwToggleButton aToggleButton = new ErmesSwToggleButton( descr, Icons.get( iconName) );
    toolbar.add( aToggleButton);
    aToggleButton.addMouseListener( this);
  }

  private void insertButtons( Container toolbar)
  {
    addButton( toolbar, "", "_object_");
    addButton( toolbar, "messbox", "_message_box_");
    addButton( toolbar,"jpatcher", "_patcher_");
    addButton( toolbar, "inlet -1", "_inlet_");
    addButton( toolbar, "outlet -1","_outlet_");
    addButton( toolbar, "comment", "_comment_");
    addButton( toolbar, "button",  "_button_");
    addButton( toolbar, "toggle",  "_toggle_");
    addButton( toolbar, "slider",  "_slider_");
    addButton( toolbar, "intbox",   "_intbox_");
    addButton( toolbar, "floatbox", "_floatbox_");
  }
}
