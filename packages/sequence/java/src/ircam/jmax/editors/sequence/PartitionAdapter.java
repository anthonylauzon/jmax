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

/**
 * the standard, partition-like adapter for a TrackEvent
 * in the Sequence database.
 * The x-coordinates corresponds to time, y and label to pitch, lenght to duration.
 */
public class PartitionAdapter extends Adapter {
  
  /**
   * constructor.
   * It creates and assigns its mappers (X, Y, Lenght, Heigth, Label), 
   * and set the initial values for the zoom, transpose and invertion fields.
   */
  public PartitionAdapter(Geometry g) 
  {
      super(g);
      XMapper = TimeMapper.getMapper();
      YMapper = PitchMapper.getMapper();
      LenghtMapper = DurationMapper.getMapper();
      HeigthMapper = AmbitusMapper.getMapper();
      LabelMapper = PitchMapper.getMapper();
      
      itsName = "Standard Adapter";
  }


  /**
   * inherited from Adapter.
   * it returns the X value of the event,
   * making the needed cordinate conversions.
   */
  public int getX(TrackEvent e) 
  {
    int temp = super.getX(e);
    
    if (geometry.getXInvertion()) temp = -temp;

    return (int) ((temp+geometry.getXTransposition())*geometry.getXZoom()) + ScoreBackground.KEYEND;
  }


  /**
   * inherited from Adapter.
   * Returns the time associated with the value of an X coordinate after
   * the coordinate conversion.
   */
  public int getInvX(int x) 
  {

    if (geometry.getXInvertion()) return (int) (geometry.getXTransposition() -(x-ScoreBackground.KEYEND)/geometry.getXZoom());

    else return (int) ((x-ScoreBackground.KEYEND)/geometry.getXZoom() - geometry.getXTransposition());
    
  }


  /**
   * set the time of the event associated with the graphic X
   */
  public void setX(TrackEvent e, int x) 
  {  
    super.setX(e, getInvX(x));
  }


  /**
   * inherited from Adapter.
   * it returns the Y value of the event,
   * making the needed cordinate conversions.
   */
  public int getY(TrackEvent e) 
  {  
    int temp = super.getY(e);
    
    if (geometry.getYInvertion()) temp = -temp;
    
    return (int) ((temp+geometry.getYTransposition())*geometry.getYZoom());  
  }


  /**
   * inherited from Adapter.
   * Returns the parameter associated with the value of an Y coordinate
   */  
  public int getInvY(int y) 
  {
    int temp;

    if (geometry.getYInvertion()) temp = (int) (geometry.getYTransposition() -y/geometry.getYZoom());
    else temp = (int) (y/geometry.getYZoom() - geometry.getYTransposition());
  
    return temp;
  }


  /**
   * set the parameter of the event associated with the graphic y
   */
  public void setY(TrackEvent e, int y) 
  {
    super.setY(e, getInvY(y));
  }


  /**
   * inherited from Adapter.
   * returns the lenght value of the event,
   * making the needed cordinate conversions (zooming).
   */
  public int getLenght(TrackEvent e) 
  {
    return (int) (super.getLenght(e)*geometry.getXZoom());
  }


  /**
   * set the duration of the event associated with the graphic lenght l.
   */
  public  void setLenght(TrackEvent e, int l) 
  {
    super.setLenght(e, (int) (l/geometry.getXZoom()));
  }

    /**
     * returns the heigth of this event */
    public int getHeigth(TrackEvent e) 
    {
	if (HeigthMapper != null) return 3*(2*HeigthMapper.get(e)+1);
	else return NOTE_DEFAULT_HEIGTH;
    }
    
    public void setHeigth(TrackEvent e, int heigth)
    {
	HeigthMapper.set(e, (heigth-3)/6);
    }

  /**
   * From the MappingListener interface.
   * Reassign the mappers according to the change, and ask
   * a repaint of the graphic.
   */
  public void mappingChanged(String graphicName, String scoreName) 
  {

    if (graphicName.equals("y")) 
      {
	if (scoreName.equals("pitch"))
	  YMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	  YMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	  YMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	  YMapper = ChannelMapper.getMapper();
      }
    else if (graphicName.equals("lenght")) 
      {
	if (scoreName.equals("pitch"))
	  LenghtMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	  LenghtMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	  LenghtMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	  LenghtMapper = ChannelMapper.getMapper();
      }
    else if (graphicName.equals("label")) 
      {
	if (scoreName.equals("pitch"))
	  LabelMapper = PitchMapper.getMapper();
	else if (scoreName.equals("duration"))
	  LabelMapper = DurationMapper.getMapper();
	else if (scoreName.equals("velocity"))
	  LabelMapper = VelocityMapper.getMapper();
	else if (scoreName.equals("channel"))
	  LabelMapper = ChannelMapper.getMapper();
      }

  }
  //------------- Fields


    public static final int NOTE_DEFAULT_HEIGTH = 3;
}






