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

package ircam.jmax.editors.patcher;

import java.awt.*;
// import javax.swing.*;
import javax.swing.RepaintManager;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import java.awt.print.*;

public class PatcherPrintManager
{
  static private PatcherPrintManager printManager = new PatcherPrintManager();

  /****************************************************************************/
  /*                                                                          */
  /*           ACTIONS                                                        */
  /*                                                                          */
  /****************************************************************************/
  
  static public void Print(EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad)container.getEditor();

    RepaintManager.currentManager(sketch).setDoubleBufferingEnabled(false);

    PrinterJob printJob = PrinterJob.getPrinterJob();
    PageFormat format = printJob.pageDialog(printJob.defaultPage());    
    printJob.setPrintable(sketch, format);

    if (printJob.printDialog()) {
	try {
	    printJob.print();
	} catch (Exception ex) {
	    ex.printStackTrace();
	}
    }
    RepaintManager.currentManager(sketch).setDoubleBufferingEnabled(true);
  }
}








