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

  public SequenceTools( int type)
  {
    String path = null;
    String fs = File.separator;
    /*
      WARNING:
      Waiting for a method to get the packagePath from the package name
    */
    path = JMaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;//??????????????????	 
    /*************************************************************/

    if( type == COMPLETE_TOOLS)
      tools = new Tool[6];
    else if( type == SCOOB_TOOLS)
      tools = new Tool[4];
    else
      tools = new Tool[5];

    tools[0] = new ArrowTool(new ImageIcon(path+"arrow.gif"));
    tools[1] = new AdderTool(new ImageIcon(path+"edit.gif"));
    
    if( type == COMPLETE_TOOLS)
      {
	tools[2] = new ResizerTool(new ImageIcon(path+"resizer.gif"));
	tools[3] = new ZoomTool(new ImageIcon(path+"zoomer.gif"));
	tools[4] = new LinerTool(new ImageIcon(path+"liner.gif"));
	tools[5] = new CombTool(new ImageIcon(path+"comber.gif"));
      }
    else if( type == SCOOB_TOOLS)
      {
	tools[2] = new ResizerTool(new ImageIcon(path+"resizer.gif"));
	tools[3] = new ZoomTool(new ImageIcon(path+"zoomer.gif"));
      }
    else
      {
	tools[2] = new ZoomTool(new ImageIcon(path+"zoomer.gif"));
	tools[3] = new LinerTool(new ImageIcon(path+"liner.gif"));
	tools[4] = new CombTool(new ImageIcon(path+"comber.gif"));
      }
  }

  public Enumeration getTools()
  {
    return new ToolEnumeration( this);
  }

  class ToolEnumeration implements Enumeration 
  {
    ToolEnumeration( SequenceTools sq)
    {
      index = 0;
      sequenceTools = sq;
    }
    
    public boolean hasMoreElements()
    {
      return index < sequenceTools.tools.length;
    }
    
    public Object nextElement()
    {
      return sequenceTools.tools[index++];
    }
    
    //---
    int index;
    SequenceTools sequenceTools;
  }

  //---
  Tool tools[];
  public static final int COMPLETE_TOOLS = 0;
  public static final int SCOOB_TOOLS = 1;
  public static final int NUMBER_TOOLS = 2;

  public static SequenceTools scoobInstance = new SequenceTools( SCOOB_TOOLS);
  public static SequenceTools numberInstance = new SequenceTools( NUMBER_TOOLS);
  public static SequenceTools completeInstance = new SequenceTools( COMPLETE_TOOLS);
}







