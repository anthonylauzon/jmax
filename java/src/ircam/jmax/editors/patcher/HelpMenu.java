package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

/** Implement the patcher editor File Menu */

class HelpMenu extends JMenu
{
  ErmesSketchWindow editor;

  HelpMenu(ErmesSketchWindow window)
  {
    super("Help");

    JMenuItem item;
    editor = window;

    item = new JMenuItem( "Help Patch", KeyEvent.VK_H);
    add( item);
    item.addActionListener( new ActionListener() {
      public  void actionPerformed( ActionEvent e)
	{
	  editor.Help();
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
	      FtsHelpPatchTable.openHelpSummary( str);
	    }
	});
       }
  
}
}
