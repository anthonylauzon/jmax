package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

/** Implement the patcher editor File Menu */

class AlignMenu extends JMenu
{
  ErmesSketchWindow editor;

  AlignMenu(ErmesSketchWindow window)
  {
    super("Edit");

    JMenuItem item;
    editor = window;

    item = new JMenuItem( "Top");
    add( item);
    item.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
	    ErmesSelection.patcherSelection.alignTop();
	}
    });

    item = new JMenuItem( "Left");
    add( item);
    item.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
	    ErmesSelection.patcherSelection.alignLeft();
	}
    });

    item = new JMenuItem( "Bottom");
    add( item);
    item.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
	    ErmesSelection.patcherSelection.alignBottom();
	}
    });

    item = new JMenuItem( "Right");
    add( item);
    item.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
	    ErmesSelection.patcherSelection.alignRight();
	}
    });
  }
}
