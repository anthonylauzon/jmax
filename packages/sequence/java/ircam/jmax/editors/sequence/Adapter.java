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

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.toolkit.*;

import java.util.*;
import java.awt.Frame;

/**
 * the converter between Score and graphic events. 
 * It implements a mapping between the score parameters (pitch, duration...)
 * and graphic parameters (x, y, width...).
 */
abstract public class Adapter implements MappingListener{

    /**
     * Builds an adapter with a given geometry object*/
    public Adapter(Geometry g)
    {
	setGeometry(g);
    }

    /**
     * Returns the geometry object for this adapter */
    public Geometry getGeometry()
    {
	return geometry;
    }

    /**
     * Sets the Geometry object to be used by this adapter */
    public void setGeometry(Geometry geometry)
    {
	this.geometry = geometry;
    }

    /**
     * Get the X value for the given event
     */
    public int getX(Event e) 
    {
	if (XMapper != null) return (int) XMapper.get(e);
	else return 0;
    }


    /**
     * Get the Y value for the given event
     */
    public int getY(Event e) 
    {
	if (YMapper != null) return YMapper.get(e);
	else return 0;
    }

    /**
     * Returns the value of the X coordinate in the given graphic context.
     * e_m_ will be in the future the only one? */
    public double getX(Event e, GraphicContext gc)
    {
	return getX(e);
    }


    /**
     * Returns the value of the X coordinate in the given graphic context.
     * e_m_ will be in the future the only one? */
    public int getY(Event e, GraphicContext gc)
    {
	return getY(e);
    }

  /**
   * get the value of the parameter associated with the graphic X
   */
    abstract public double getInvX(int x);
  
    abstract public int getInvWidth(int w);

  /**
   * get the value of the parameter associated with the graphic Y
   */
  abstract public int getInvY(int y);


  /**
   * set the parameter of the event associated with the graphic X
   */
    public void setX(Event e, int x) 
    {
	XMapper.set(e, getInvX(x));
    }


  /**
   * set the parameter of the event associated with the Y
   */
    public void setY(Event e, int y) 
    {
	YMapper.set(e, y);
    }
  

  /**
   * get the Lenght value for this event
   */
    public int getLenght(Event e) 
    {
	if (LenghtMapper != null) return (int) LenghtMapper.get(e);
	else return 0;
    }
  /**
   * sets the parameter of the event associated with the Lenght
   */
    public void setLenght(Event e, int lenght) 
    {
	LenghtMapper.set(e, lenght);
    }

    public void setHeigth(Event e, int heigth)
    {
    }

    public int getHeigth(Event e)
    {
	return NOTE_DEFAULT_HEIGTH;
    }
 
  /**
   * get the label value (int) for this event
   */
    public String getLabel(Event e) 
  {
    if (LabelMapper != null) return LabelMapper.get(e);
    else return "";
  }

  /**
   * sets the parameter of the event associated with the Label
   */
  public void setLabel(Event e, String label) 
  {
    LabelMapper.set(e, label);
  }

  /**
   * edit this adapter (settings dialog)
   */
  public void edit(Frame theFrame) 
  {
      //SettingsDialog asd = SettingsDialog.createSettingsDialog(this, theFrame);
      //asd.pack();
      //asd.setVisible(true);
  }


  /**
   * return the Adapter's name
   */
  public String getName() 
  {
    return itsName;
  } 

  public boolean isVerticalZoomable()
  {
      return false;
  }
  public boolean isVerticalScrollable()
  {
      return false;
  }
  public boolean isHorizontalMovementBounded()
  {
      return false;
  }
  public boolean isHorizontalMovementAllowed()
  {
      return true;
  }
  public boolean isDrawable()
  {
      return false;
  }
  /**
   * MappingListener interface
   */
  public void mappingChanged(String graphicName, String scoreName) {}


  //--- Fields
  public DoubleMapper XMapper;
  public Mapper YMapper;
  //public Mapper LenghtMapper;
  public DoubleMapper LenghtMapper;//*@*//
  public Mapper HeigthMapper;
  public StringMapper LabelMapper;

  Geometry geometry; //package private
  String itsName;

  public final static int NOTE_DEFAULT_HEIGTH = 3;
}








