
package ircam.jmax.editors.sequence.track;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.renderers.*;
import java.io.*;
import javax.swing.*;
import java.util.*;

/**
 * The EventValue object that represents a Logic expression event. Is used during score-recognition */
public class LogicValue extends AbstractEventValue
{
    public LogicValue()
    {
	super();

	properties.put("expression", "");
    }

    /**
     * returns the ValueInfo associated to this class */
    public ValueInfo getValueInfo()
    {
	return info;
    }

    static class LogicValueInfo extends AbstractValueInfo {
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return LOGIC_NAME;
	}


	public ImageIcon getIcon()
	{
	    return LOGIC_ICON;
	}

	public Object newInstance()
	{
	    return new LogicValue();
	}


    }


    /**
     * Returns its specialized renderer */
    public ObjectRenderer getRenderer()
    {
	return LogicEventRenderer.getRenderer();
    }
  
    public ValueEditor getValueEditor()
    {
	return LogicEditor.getInstance();
    } 


    public Enumeration getPropertyNames()
    {
	return new ArrayEnumeration(nameArray);
    }

  //--- Fields
  public static final String fs = File.separator;
  public static final String LOGIC_NAME = "Logic";
  static String path;
  public static ImageIcon LOGIC_ICON;
  static String nameArray[] = {"expression"};
  public static LogicValueInfo info = new LogicValueInfo();
  private String expression = new String(); 

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
    LOGIC_ICON = new ImageIcon(path+"logic.gif");
  }
}

