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
import ircam.jmax.editors.sequence.renderers.*;
import java.io.File.*;
import javax.swing.*;
import java.util.*;

/**
 * The EventValue object that represents a Integer event. Is used during score-recognition */
public class IntegerValue extends AbstractEventValue
{
    public IntegerValue()
    {
	super();

	properties.put("integer", new Integer(0));
    }


    public ValueInfo getValueInfo() 
    {
	return info;
    }

    static class IntegerValueInfo extends AbstractValueInfo {
	/**
	 * Returns the name of this value object */
	public String getName()
	{
	    return INTEGER_NAME;
	}

	public Object newInstance()
	{
	    return new IntegerValue();
	}

    }

    /**
     * Returns its specialized renderer (an AmbitusEventRenderer) */
    public ObjectRenderer getRenderer()
    {
	return IntegerEventRenderer.getRenderer();
    }
  
    public Enumeration getPropertyNames()
    {
	return new ArrayEnumeration(nameArray);
    }

    //--- Fields

    public static final String INTEGER_NAME = "Integer";
    public static IntegerValueInfo info = new IntegerValueInfo();

    static String nameArray[] = {"integer"};
}

