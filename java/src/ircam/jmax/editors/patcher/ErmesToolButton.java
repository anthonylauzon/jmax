package ircam.jmax.editors.patcher;

import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.editors.patcher.*;

class ErmesToolButton extends JToggleButton 
{
  String description;
  ErmesToolBar  toolBar;

  ErmesToolButton(ErmesToolBar  toolBar, String description, ImageIcon theIcon)
  {
    super( theIcon);

    setDoubleBuffered( false);
    setMargin( new Insets(0,0,0,0));
    this.description = description;
    this.toolBar = toolBar;

    addChangeListener(new ChangeListener()
		      {
			public void stateChanged(ChangeEvent e)
			  {
			    ErmesToolButton.this.toolBar.buttonChanged(ErmesToolButton.this);
			  }
		      });
  }

  void makeObject(ErmesSketchPad sketch, int x, int y)
  {
    sketch.makeObject(description, x, y);
    setSelected(false);
  }
}


