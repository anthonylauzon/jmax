
package ircam.jmax.editors.sequence;

import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import java.io.File;
import java.util.*;
import javax.swing.*;


/**
 * The class containing the Tools used by a Sequence editor.
 * This class separates the ToolManager functionalities 
 * of the SequenceToolManager by the knowledge of the tools */
public class SequenceTools implements ToolProvider{

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
	    return index < tools.length;
	}

	public Object nextElement()
	{
	    return tools[index++];
	}

	//---
	int index;
    }

    //---
    static String fs = File.separator;
    static String path = MaxApplication.getProperty("sequencePackageDir")+fs+"images" +fs;
    
    
    static Tool tools[] ={ new ArrowTool(new ImageIcon(path+"selecter.gif")),
			   new AdderTool(new ImageIcon(path+"genericAdder.gif")),
			   new DeleteTool(new ImageIcon(path+"eraser.gif")),
			   new ResizerTool(new ImageIcon(path+"resizer.gif")),
			   new VResizerTool(new ImageIcon(path+"vresizer.gif"))};
    public static SequenceTools instance = new SequenceTools();
}
