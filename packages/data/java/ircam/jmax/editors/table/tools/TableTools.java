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

package ircam.jmax.editors.table.tools;

import ircam.jmax.editors.table.*;

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
public class TableTools implements ToolProvider{

  public TableTools()
  {
    String fs = File.separator;
    String path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"data"+fs+"images"+fs;

    tools[0] = new PencilTool(new ImageIcon(path+"edit.gif"));
    tools[1] = new TableSelecter(new ImageIcon(path+"arrow.gif"));
    tools[2] = new LinerTool(new ImageIcon(path+"liner.gif"));
    tools[3] = new ZoomTool(new ImageIcon(path+"zoomer.gif"));
  }

  public Enumeration getTools()
  {
    return new ToolEnumeration();
  }

  public static Tool getDefaultTool()
  {
      return instance.tools[0];
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
  Tool tools[] = new Tool[4];
  public static TableTools instance = new TableTools();
}



