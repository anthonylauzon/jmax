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

package ircam.jmax.editors.sequence.tools;

import ircam.jmax.editors.sequence.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * The class containing the Tools used by a Sequence editor.
 * This class separates the ToolManager functionalities 
 * of the SequenceToolManager by the knowledge of the tools */
public class SequenceTools implements ToolProvider{

  public SequenceTools()
  {
    String path = null;
    String fs = File.separator;

    try
      {
	path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
      }
    catch(FileNotFoundException e){
	//System.err.println("Couldn't locate sequence images");
	path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
    }

    tools[0] = new ArrowTool(new ImageIcon(path+"arrow.gif"));
    //tools[1] = new AdderTool(new ImageIcon(path+"plus.gif"));
    tools[1] = new AdderTool(new ImageIcon(path+"edit.gif"));
    tools[2] = new DeleteTool(new ImageIcon(path+"delete.gif"));
    tools[3] = new ResizerTool(new ImageIcon(path+"hresize.gif"));
    tools[4] = new VResizerTool(new ImageIcon(path+"vresize.gif"));
  }

  public Enumeration getTools()
  {
    return new ToolEnumeration();
  }

  class ToolEnumeration implements Enumeration 
  {
    ToolEnumeration()
    {
      index = 0;
    }
    
    public boolean hasMoreElements()
    {
      return index < instance.tools.length;
    }
    
    public Object nextElement()
    {
      return instance.tools[index++];
    }
    
    //---
    int index;
  }

  //---
  Tool tools[] = new Tool[5];
  public static SequenceTools instance = new SequenceTools();
}



