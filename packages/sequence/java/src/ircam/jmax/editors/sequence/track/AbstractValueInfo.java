
package ircam.jmax.editors.sequence.track;

import ircam.jmax.*;
import java.awt.Component;
import ircam.jmax.editors.sequence.SequenceGraphicContext;
import javax.swing.*;
import java.io.*;

/**
 * A convenience implementation of the ValueInfo interface.
 */
public class AbstractValueInfo implements ValueInfo {
    
    public String getName()
    {
	return "unknown";
    }

    public ImageIcon getIcon()
    {
	return GENERIC_ICON;
    }
    
    public Object newInstance()
    {
	return new AbstractEventValue();
    }
    

    public ValueEditor newValueEditor()
    {
	return NullEditor.getInstance();
    }
    
    public Component newWidget(SequenceGraphicContext gc)
    {
	return null;
    }

    public AbstractValueInfo getInstance()
    {
	return instance;
    }

  //--- AbstractValueInfo fields
  public static AbstractValueInfo instance = new AbstractValueInfo();
  public static final String fs = File.separator;
  static String path;
  public static ImageIcon GENERIC_ICON;

  static 
  {
     try
      {
	path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
      }
    catch(FileNotFoundException e){
	//System.err.println("Couldn't locate sequence images");
	path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
    }
    GENERIC_ICON = new ImageIcon(path+"genericAdder.gif");
  }

}


