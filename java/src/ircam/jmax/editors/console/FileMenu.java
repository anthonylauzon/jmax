//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.console;

import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.toolkit.actions.*;

//import ircam.jmax.editors.console.*;

/** Implement the console editor File Menu */

public class FileMenu extends DefaultFileMenu
{
    public FileMenu()
    {
	super();
	insert(new PrintAction(), "Print", Toolkit.getDefaultToolkit().getMenuShortcutKeyMask(), KeyEvent.VK_P, 3);
	
	remove(5);
	remove(4);
	setDefaultNumEntries(6);
    }

    public class PrintAction extends EditorAction
    {
	public void doAction(EditorContainer container)
	{
	    PrinterJob printJob = PrinterJob.getPrinterJob();
	    PageFormat format = printJob.pageDialog(printJob.defaultPage());    
	    printJob.setPrintable((ConsoleWindow)container, format);
	    
	    if (printJob.printDialog()) {
		try {
		    printJob.print();
		} catch (Exception ex) {
		    ex.printStackTrace();
		}
	    }
	}
    } 
}

