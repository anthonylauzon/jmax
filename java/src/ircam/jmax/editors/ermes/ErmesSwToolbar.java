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

    lockEditButton = new JToggleButton( IconCache.getIcon( "tool_lock_mode.gif"));
    lockEditButton.setDoubleBuffered( false);
    lockEditButton.setMargin( new Insets(0,0,0,0));
    lockEditButton.setSelectedIcon( IconCache.getIcon( "tool_edit_mode.gif"));
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
	upButton = new JButton(IconCache.getIcon( "tool_up.gif"));
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

  public void removeNotify()
  {
    itsSketchPad = null;
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
    ErmesSwToggleButton aToggleButton = new ErmesSwToggleButton( descr, IconCache.getIcon( iconName) );
    toolbar.add( aToggleButton);
    aToggleButton.addMouseListener( this);
  }

  private void insertButtons( Container toolbar)
  {
    addButton( toolbar, "", "tool_ext.gif");
    addButton( toolbar, "messbox", "tool_mess.gif");
    addButton( toolbar, "jpatcher", "tool_patcher.gif");
    addButton( toolbar, "inlet -1", "tool_in.gif");
    addButton( toolbar, "outlet -1","tool_out.gif");
    addButton( toolbar, "comment", "tool_text.gif");
    addButton( toolbar, "button", "tool_bang.gif");
    addButton( toolbar, "toggle", "tool_toggle.gif");
    addButton( toolbar, "slider", "tool_slider.gif");
    addButton( toolbar, "intbox", "tool_int.gif");
    addButton( toolbar, "floatbox", "tool_float.gif");
  }
}
