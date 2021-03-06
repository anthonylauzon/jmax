//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.bpf.tools;

import ircam.jmax.editors.bpf.*;

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
public class BpfTools implements ToolProvider{

  public BpfTools()
  {
    if(JMaxApplication.getProperty("ftm") == null)
    {
      String fs = File.separator;
      String path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"data"+fs+"images"+fs;	 

      tools[0] = new ArrowTool(new ImageIcon(path+"data_arrow.gif"));
      tools[1] = new ScrollZoomTool(new ImageIcon(path+"data_scroller.gif"));
    }
    else
    {
      tools[0] = new ArrowTool(JMaxUtilities.loadIconFromResource("images/data_arrow.gif"));
      tools[1] = new ScrollZoomTool(JMaxUtilities.loadIconFromResource("images/data_scroller.gif"));
    }
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
  Tool tools[] = new Tool[2];
  public static BpfTools instance = new BpfTools();
}



