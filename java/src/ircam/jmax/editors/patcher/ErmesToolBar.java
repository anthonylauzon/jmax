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

public class ErmesToolBar extends JPanel  {

  ErmesSketchPad sketch;

  private boolean staySelected = false;
  boolean pressed = false;

  JPanel toolBar;
  CardLayout cardLayout;
  JPanel cards;
  JToggleButton lockEditButton;
  JButton upButton;
  ErmesToolButton selectedButton = null;

  public ErmesToolBar( ErmesSketchPad theSketchPad)
  {
    sketch = theSketchPad;

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
	    sketch.itsSketchWindow.setLocked( false);
	  else
	    sketch.itsSketchWindow.setLocked( true);
	}
    });

    add( lockEditButton, BorderLayout.WEST);

    if ((sketch.itsPatcher.getParent() != null) &&
	(sketch.itsPatcher.getParent() != Fts.getRootObject()))
      {
	upButton = new JButton(IconCache.getIcon( "tool_up.gif"));
	upButton.setDoubleBuffered( false);
	upButton.setMargin( new Insets(0,0,0,0));
	upButton.addActionListener( new ActionListener() {
	  public void actionPerformed( ActionEvent e)
	    {
	      sketch.waiting();
	      Fts.editPropertyValue(sketch.itsPatcher.getParent(), sketch.itsPatcher, 
				    new MaxDataEditorReadyListener()
				    {
				      public void editorReady(MaxDataEditor editor)
					{sketch.stopWaiting();}
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

    toolBar = new JPanel();
    toolBar.setBorder( new EmptyBorder( 0, 0, 0, 0));
    toolBar.setLayout( new BoxLayout( toolBar, BoxLayout.X_AXIS));
    insertButtons();

    cards.add( "edit", toolBar);
  }

  void buttonChanged(ErmesToolButton button)
  {
    if (button.isSelected())
      {
	if ((selectedButton != null) && (button != selectedButton))
	  selectedButton.setSelected(false);

	selectedButton = button;
	sketch.setAddModeInteraction();
      }
    else
      {
	if (button == selectedButton)
	  selectedButton = null;
      }
  }

  boolean isActive()
  {
    return selectedButton != null;
  }

  public void makeObject(int x, int y)
  {
    if (selectedButton != null)
      selectedButton.makeObject(sketch, x, y);
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

  private void addButton( String descr, String iconName)
  {
    toolBar.add( new ErmesToolButton(this, descr, IconCache.getIcon(iconName)));
  }

  private void insertButtons()
  {
    addButton( "", "tool_ext.gif");
    addButton( "messbox", "tool_mess.gif");
    addButton( "jpatcher", "tool_patcher.gif");
    addButton( "inlet -1", "tool_in.gif");
    addButton( "outlet -1","tool_out.gif");
    addButton( "comment", "tool_text.gif");
    addButton( "button",  "tool_bang.gif");
    addButton( "toggle",  "tool_toggle.gif");
    addButton( "slider",  "tool_slider.gif");
    addButton( "intbox",   "tool_int.gif");
    addButton( "floatbox", "tool_float.gif");
  }
}
