package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

import com.sun.java.swing.*;
import com.sun.java.swing.border.*;
import com.sun.java.swing.plaf.*;
import com.sun.java.swing.plaf.motif.*;

import ircam.jmax.*;
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

  JToggleButton itsLastPressed;

  public ErmesSwToolbar( ErmesSketchPad theSketchPad)
  {
    itsSketchPad = theSketchPad;

    setDoubleBuffered( false);

    setLayout( new BorderLayout());    

    String path = MaxApplication.getProperty( "root" ) + File.separator + "images" + File.separator;
    lockEditButton = new JToggleButton( IconCache.getIcon( path + "tool_lock_mode.gif"));
    lockEditButton.setDoubleBuffered( false);
    lockEditButton.setMargin( new Insets(0,0,0,0));
    lockEditButton.setSelectedIcon( IconCache.getIcon( path + "tool_edit_mode.gif"));
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

  private void addButton( Container toolbar, String descr, String gifFilePath)
  {
    ErmesSwToggleButton aToggleButton = new ErmesSwToggleButton( descr, IconCache.getIcon( gifFilePath) );
    toolbar.add( aToggleButton);
    aToggleButton.addMouseListener( this);
  }

  private void insertButtons( Container toolbar)
  {
    String fs = File.separator;
    String path = MaxApplication.getProperty( "root" ) + fs + "images" + fs;

    addButton( toolbar, "", path + "tool_ext.gif");
    addButton( toolbar, "messbox", path + "tool_mess.gif");
    addButton( toolbar, "jpatcher",  path + "tool_patcher.gif");
    addButton( toolbar, "inlet -1", path + "tool_in.gif");
    addButton( toolbar, "outlet -1", path + "tool_out.gif");
    addButton( toolbar, "comment", path + "tool_text.gif");
    addButton( toolbar, "button", path + "tool_bang.gif");
    addButton( toolbar, "toggle", path + "tool_toggle.gif");
    addButton( toolbar, "slider", path + "tool_slider.gif");
    addButton( toolbar, "intbox", path + "tool_int.gif");
    addButton( toolbar, "floatbox", path + "tool_float.gif");
  }
}
