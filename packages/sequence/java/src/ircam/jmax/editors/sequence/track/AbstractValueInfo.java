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

package ircam.jmax.editors.sequence.track;

import ircam.jmax.*;
import java.awt.Component;
import ircam.jmax.editors.sequence.SequenceGraphicContext;
import java.awt.datatransfer.*;
import javax.swing.*;
import java.io.*;
import java.util.*;

import ircam.jmax.editors.sequence.*;

/**
 * A convenience implementation of the ValueInfo interface.
 */
public class AbstractValueInfo implements ValueInfo {
    
    public String getName()
    {
	return "unknown";
    }

    public String getPublicName()
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

    public Enumeration getPropertyNames()
    {
	return new EmptyEnumeration();
    }
    public int getPropertyCount()
    {
	return 0; 
    }

    public Class getPropertyType(int index)
    {
	return Integer.class; 
    }

    public DataFlavor getDataFlavor()
    {
	return AbstractValueDataFlavor.getInstance();
    }

    /**
     * A convenience class to implement an empty enumeration */
    public class EmptyEnumeration implements Enumeration {
	public boolean hasMoreElements()
	{
	    return false;
	}
	
	public Object nextElement()
	{
	    return null;
	}
    }
    
  //--- AbstractValueInfo fields
  public static AbstractValueInfo instance = new AbstractValueInfo();
  public static final String fs = File.separator;
  static String path;
  public static ImageIcon GENERIC_ICON;

  static 
  {
      /*
	WARNING:
	Waiting for a method to get the packagePath from the package name
      */
      /*try
	{
	path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
	}
	catch(FileNotFoundException e){
	//System.err.println("Can't locate sequence images");
	path = MaxApplication.getProperty("sequencePackageDir")+File.separator+"images"+File.separator;
	}*/
      path = MaxApplication.getProperty("jmaxRoot")+fs+"packages"+fs+"sequence"+fs+"images"+fs;//??????????????   
      /*************************************************************/
    GENERIC_ICON = new ImageIcon(path+"genericAdder.gif");
  }

}












