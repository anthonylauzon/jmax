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

package ircam.jmax.toolkit.actions;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;
import ircam.jmax.dialogs.*;
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

    if (preset_file == null)
      file = MaxFileChooser.chooseFileToOpen(container.getFrame());
    else
      file = preset_file;
	
    frame = container.getFrame();

    if (file != null)
      {
	//FtsPatcherObject.fireAtomicAction(true);
	RecentFileHistory recentFileHistory = JMaxApplication.getRecentFileHistory();
	recentFileHistory.addFile(file);

	try
	  {	
	    JMaxApplication.getFtsServer().getRoot().load(file.getAbsolutePath());
	  }
	catch(IOException e)
	  {
	    System.err.println("[OpenAction]: I/O error loading file "+file.getAbsolutePath());
	  }
      }
  }

  /*public static int getFileType(File file)
    {
    int type = -1;
    
    try
    {
    FileReader fr = new FileReader(file);
    char buf[] = new char[6];
    String codeBMax, codeDotPat;
    
    fr.read(buf);
    fr.close();
	    
    codeBMax = new String(buf, 0, 4);
    
    if (codeBMax.equals("bMax") || codeBMax.equals("Mbxa"))
    type = MaxFileChooser.JMAX_FILE_TYPE;
    else 
    {
    codeDotPat = new String(buf, 0, 6);
    if ( codeDotPat.equals("max v2"))
    type = MaxFileChooser.PAT_FILE_TYPE;
    }
    return type;
    }
    catch (FileNotFoundException e)
    {
    return -1;
    }
    catch (IOException e)
    {
    return -1;
    }
    }*/
}




