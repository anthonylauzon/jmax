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

    lockEditButton = new JToggleButton( Icons.get( "_lock_mode_"));
    lockEditButton.setDoubleBuffered( false);
    lockEditButton.setMargin( new Insets(0,0,0,0));
    lockEditButton.setSelectedIcon( Icons.get( "_edit_mode_"));
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
	(sketch.itsPatcher.getParent() != sketch.getFts().getRootObject()))
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
	sketch.setAddModeInteraction(selectedButton.getDescription(), selectedButton.getMessage(), true);
      }
    else
      {
	if (button == selectedButton)
	  selectedButton = null;
      }
  }

  public void reset()
  {
    if (selectedButton != null)
      selectedButton.setSelected(false);
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
    toolBar.add( new ErmesToolButton(this, descr, Icons.get(iconName), message));
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
  }
}
