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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 


package ircam.jmax.editors.sequence.track;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import java.io.*;
import javax.swing.*;

/**
 * The EventValue object that represents a Cue event. Is used during score-recognition */
public class CueValue extends IntegerValue
{
    public CueValue()
    {
    }

    public ValueInfo getValueInfo()
    {
	return info;
    }

    static class CueValueInfo extends AbstractValueInfo {
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return CUE_NAME;
	    
	}

	public ImageIcon getIcon()
	{
	    return CUE_ICON;
	}

	public ValueEditor newValueEditor()
	{
	    return CueEditor.getInstance();
	} 

	public Object newInstance()
	{
	    return new CueValue();
	}


    }



    /**
     * Returns its specialized renderer (an AmbitusEventRenderer) */
    public ObjectRenderer getRenderer()
    {
	return CueEventRenderer.getRenderer();
    }
  

    //--- Fields
  public static final String fs = File.separator;
  public static final String CUE_NAME = "Cue";
  static String path;
  public static ImageIcon CUE_ICON;
  public static CueValueInfo info = new CueValueInfo();

  static 
  {
    try
      {
	path  = MaxApplication.getPackageHandler().locatePackage("sequence").getPath()+fs+"images"+fs;
	CUE_ICON = new ImageIcon(path+"cue.gif");
      }
    catch(FileNotFoundException e){
      System.err.println("Couldn't locate sequence images");
    }
  }
}

