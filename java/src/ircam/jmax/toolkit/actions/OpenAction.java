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

package ircam.jmax.toolkit.actions;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.project.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

public class OpenAction extends EditorAction
{
  Frame frame;
  private File preset_file;
  
  public OpenAction()
  {
    preset_file = null;
  }

  public OpenAction(File file)
  {
     preset_file = file;
  }
  
  public void doAction(EditorContainer container)
  {
    File file;
    String fileName;

    if (preset_file == null)
      file = MaxFileChooser.chooseFileToOpen(container.getFrame());
    else
      file = preset_file;
	
    frame = container.getFrame();
    
    if (file != null)
      {
	fileName = file.getAbsolutePath();
	
	RecentFileHistory recentFileHistory = JMaxApplication.getRecentFileHistory();
	recentFileHistory.addFile(file);
	    
	try
	  {	
	    JMaxApplication.load(fileName);
	  }
	catch(IOException e)
	  {
	    System.err.println("[OpenAction]: I/O error loading file "+fileName);
	  }
      }
  }
}




