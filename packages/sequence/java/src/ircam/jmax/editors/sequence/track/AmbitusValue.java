
package ircam.jmax.editors.sequence.track;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import javax.swing.*;
import java.awt.*;
import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;

/**
 * The EventValue object that represents a midi-like note with "ambitus" information */
public class AmbitusValue extends AbstractEventValue
{
  public AmbitusValue()
  {
    super();
    
    setDefaultProperties();
  }
  
  private void setDefaultProperties()
  {
    for (Enumeration e = getPropertyNames(); e.hasMoreElements();)
      {
	properties.put((String) e.nextElement(), DEFAULT_PROPERTY);
      }
    
    setProperty("duration", new Integer(100));
    setProperty("ambitus", new Integer(0));
  }

  Object duration;
  public void setProperty(String name, Object value)
  {
      if(name.equals("duration"))
	  duration = value;
      else if(name.equals("pitch") && ((Integer)value).intValue() > 127)
	  value = new Integer(127);
	  
      super.setProperty(name, value);
  }
  public Object getProperty(String name)
  {
      if(name.equals("duration"))
	  return duration;
      else
	  return super.getProperty(name);
  }

  public ValueInfo getValueInfo()
  {
    return info;
  }
  
  static class AmbitusValueInfo extends AbstractValueInfo {
    /**
     * Returns the name of this value object */
    public String getName()
    {
      return AMBITUS_NAME;
    }

    public String getPublicName()
    {
      return AMBITUS_PUBLIC_NAME;
    }
    

    public ImageIcon getIcon()
    {
      return AMBITUS_ICON;
    }
    
    /**
     * Create an instance of the associated EventValue */
    public Object newInstance()
    {
      return new AmbitusValue();
    }

    /**
     * Create a new Widget for the associated Value */ 
    public Component newWidget(SequenceGraphicContext gc)
    {
      ScrEventWidget widget = new ScrEventWidget(BoxLayout.Y_AXIS, gc);
      return widget;
    }
   
    public Enumeration getPropertyNames()
      {
	  return new ArrayEnumeration(defNamesArray);
      }


    public int getPropertyCount()
      {
	  return defPropertyCount;
      }

    public DataFlavor getDataFlavor()
      {
	  return AmbitusValueDataFlavor.getInstance();
      }
 
    String defNamesArray[] = {"pitch", "duration"};
    int defPropertyCount = 2;
  }

  public JPopupMenu getPopupMenu()
  {
      //return MidiEventPopupMenu.getInstance();
      return null;
  } 

  /**
   * Returns its specialized renderer (an AmbitusEventRenderer) */
  public ObjectRenderer getRenderer()
  {
    return AmbitusEventRenderer.getRenderer();
  }
  
  public Enumeration getPropertyNames()
  {
    return new ArrayEnumeration(nameArray);
  }
  public int getPropertyCount()
  {
    return propertyCount;
  }

  public int getPropertyType(int index)
  {
      if(index < propertyCount)
	  return propertyTypes[index];
      else return UNKNOWN_TYPE;
  }

    public Object[] getPropertyValues()
    {
	for(int i = 0; i<propertyCount; i++)
	    propertyValuesArray[i] = getProperty(nameArray[i]);

	return propertyValuesArray;
    }

    public void setPropertyValues(int nArgs, Object args[])
    {
	for(int i = 0; i<nArgs; i++)
	    setProperty(nameArray[i], args[i]);
    }

    public boolean samePropertyValues(Object args[])
    {
	return ((((Integer)getProperty("pitch")).intValue() == ((Integer)args[0]).intValue()) &&
	    (((Integer)getProperty("duration")).intValue() == ((Integer)args[1]).intValue())); 
    }

  //--- Fields
    public static final int DEFAULT_MAX_PITCH = 127;
    public static final int DEFAULT_MIN_PITCH = 0;
    public static final String fs = File.separator;
    public static final String AMBITUS_NAME = "noteevt";
    public static final String AMBITUS_PUBLIC_NAME = "note";
    static String path;
    public static ImageIcon AMBITUS_ICON;
    public static AmbitusValueInfo info = new AmbitusValueInfo();

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
    AMBITUS_ICON = new ImageIcon(path+"ambitusAdder.gif");
  }

    static String nameArray[] = {"pitch", "duration"};
    static int propertyTypes[] = {INTEGER_TYPE, INTEGER_TYPE};
    static int propertyCount = 2;
}





