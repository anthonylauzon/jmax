package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;

/** Implement the patcher editor File Menu */

public class HelpMenu extends JMenu
{
  ErmesSketchWindow editor;

  public HelpMenu(ErmesSketchWindow window)
  {
    super("Help");
    setHorizontalTextPosition(AbstractButton.LEFT);

    JMenuItem item;
    editor = window;

    item = new JMenuItem( "Help Patch", KeyEvent.VK_H);
    add( item);
    item.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad))
	    {
	      if (! ErmesSelection.patcherSelection.openHelpPatches())
		new ErrorDialog( editor, "Sorry, no help for object ");
	    }
	}
    });

    // Adding the summaries 

    Enumeration en = FtsHelpPatchTable.getSummaries(); 

    while (en.hasMoreElements())
       {
	 final String str = (String) en.nextElement();

	item = new JMenuItem( str + " summary");
	add( item);
	item.addActionListener( new ActionListener() {
	  public  void actionPerformed( ActionEvent e)
	    {
	      FtsHelpPatchTable.openHelpSummary(editor.getFts(), str);
	    }
	});
       }
  
}
}
