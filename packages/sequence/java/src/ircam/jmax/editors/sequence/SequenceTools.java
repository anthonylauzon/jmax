
package ircam.jmax.editors.sequence;

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
      System.err.println("Couldn't locate sequence images");
    }

    tools[0] = new ArrowTool(new ImageIcon(path+"selecter.gif"));
    tools[1] = new AdderTool(new ImageIcon(path+"genericAdder.gif"));
    tools[2] = new DeleteTool(new ImageIcon(path+"eraser.gif"));
    tools[3] = new ResizerTool(new ImageIcon(path+"resizer.gif"));
    tools[4] = new VResizerTool(new ImageIcon(path+"vresizer.gif"));
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



