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

class EditMenu extends JMenu
{
  ErmesSketchWindow editor;
  boolean locked;

  JMenuItem cutItem;
  JMenuItem copyItem;
  JMenuItem pasteItem;
  JMenuItem duplicateItem;
  JMenuItem selectAllItem;
  JMenuItem lockItem;
  
  EditMenu(ErmesSketchWindow window)
  {
    super("Edit");

    JMenuItem item;
    editor = window;

    cutItem = new JMenuItem("Cut", KeyEvent.VK_X);
    add(cutItem);
    cutItem.addActionListener(new ActionListener()
			      {public  void actionPerformed(ActionEvent e)
				{ editor.Cut();}});

    copyItem = new JMenuItem("Copy", KeyEvent.VK_C);
    add(copyItem);
    copyItem.addActionListener(new ActionListener()
			       { public  void actionPerformed(ActionEvent e)
				 { editor.Copy();}});

    pasteItem = new JMenuItem("Paste", KeyEvent.VK_V);
    add(pasteItem);
    pasteItem.addActionListener(new ActionListener()
				{public  void actionPerformed(ActionEvent e)
				  { editor.Paste();}});

    duplicateItem = new JMenuItem("Duplicate", KeyEvent.VK_D);
    add(duplicateItem);
    duplicateItem.addActionListener( new ActionListener()
				     {public  void actionPerformed(ActionEvent e)
				       { editor.Duplicate();}});

    addSeparator();

    selectAllItem = new JMenuItem( "Select All", KeyEvent.VK_A);
    add( selectAllItem);
    selectAllItem.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  if (! editor.isLocked())
	    {
	      ErmesSelection.patcherSelection.setOwner(editor.itsSketchPad); 
	      ErmesSelection.patcherSelection.deselectAll();
	      editor.itsSketchPad.getDisplayList().selectAll();
	      editor.itsSketchPad.redraw();
	    }
	}
    });

    addSeparator();

    item = new JMenuItem( "Find");
    add( item);
    item.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
 	{
	  FindPanel p;
	  p = FindPanel.open();

	  if (Fts.getSelection().getObjects().size() > 0)
	    {
	      p.findFriends((FtsObject) Fts.getSelection().getObjects().elementAt( 0));
	    }
 	}
    });

    item = new JMenuItem( "Find Errors");
    add( item);
    item.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
 	{
	  FindPanel p;
	  p = FindPanel.open();
	  p.findErrors();
 	}
    });

    addSeparator();

    item = new JMenuItem("Inspect", KeyEvent.VK_I);
    add(item);

    item.addActionListener( new ActionListener() {
      public void actionPerformed(ActionEvent e) 
	{
	  editor.inspectAction();
	}
    });

    addSeparator();

    add( new AlignMenu(editor));

    addSeparator();

    lockItem = new JMenuItem( "Lock", KeyEvent.VK_E);
    add( lockItem);
    lockItem.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
 	{
	  editor.setLocked(! editor.isLocked());
 	}
    });

    enableCut(true);
    enableCopy(true);
    enablePaste(true);
    enableDuplicate(true);
  }

  void enableCut(boolean v)
  {
    cutItem.setEnabled(v);
  }

  void enableCopy(boolean v)
  {
    copyItem.setEnabled(v);
  }

  void enablePaste(boolean v)
  {
    pasteItem.setEnabled(v);
  }

  void enableDuplicate(boolean v)
  {
    duplicateItem.setEnabled(v);
  }

  void enableSelectAll(boolean v)
  {
    selectAllItem.setEnabled(v);
  }

  void setLocked(boolean v)
  {
    locked = v;
    lockItem.setLabel(locked ? "Unlock" : "Lock");
    updateMenus();
  }

  void selectionChanged()
  {
    updateMenus();
  }

  private void updateMenus()
  {
    if (locked)
      {
	enableSelectAll(false);
	enableCut(false);
	enableCopy(false);
	enableDuplicate(false);
      }
    else
      {
	if (ErmesSelection.patcherSelection.isEmpty())
	  {
	    enableCut(false);
	    enableCopy(false);
	    enableDuplicate(false);
	  }
	else
	  {
	    enableCut(true);
	    enableCopy(true);
	    enableDuplicate(true);
	  }

      }

    enablePaste(! editor.ftsClipboardIsEmpty());
  }
}
