
package ircam.jmax.editors.sequence.track;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.*;
import java.io.*;
import javax.swing.*;

/**
 * The EventValue object that represents a fricative event. Is used during score-recognition */
public class FricativeValue extends AbstractEventValue
{
    public FricativeValue()
    {
	super();
	setDefaultProperties();
    }


    public ValueInfo getValueInfo()
    {
	return info;
    }


    static class FricativeValueInfo extends AbstractValueInfo {    
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return FRICATIVE_NAME;
	}

	public ImageIcon getIcon()
	{
	    return FRICATIVE_ICON;
	}

	public Object newInstance()
	{
	    return new FricativeValue();
	}

	//---

    }

    private void setDefaultProperties()
    {
	properties.put("duration", new Integer(100));
    }

    /**
     * Returns its specialized renderer (a FricativeEventRenderer) */
    public ObjectRenderer getRenderer()
    {
	return FricativeEventRenderer.getRenderer();
    }

  
    //--- Fields

  public static final String fs = File.separator;
  public static final String FRICATIVE_NAME = "Fricative";
  static String path;
  public static ImageIcon FRICATIVE_ICON;
  public static FricativeValueInfo info = new FricativeValueInfo();

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
    FRICATIVE_ICON = new ImageIcon(path+"fricative.gif");
  }
}



