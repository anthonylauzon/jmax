//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.explode;

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
   * get the X value for this event
   */
  public int getX(ScrEvent e) 
  {
    if (XMapper != null) return XMapper.get(e);
    else return 0;
  }


  /**
   * get the Y value for this event
   */
  public int getY(ScrEvent e) 
  {
    if (YMapper != null) return YMapper.get(e);
    else return 0;
  }


  /**
   * get the value of the parameter associated with the graphic X
   */
  abstract public int getInvX(int x);
  

  /**
   * get the value of the parameter associated with the graphic Y
   */
  abstract public int getInvY(int y);


  /**
   * set the parameter of the event associated with the graphic X
   */
  public void setX(ScrEvent e, int x) 
  {
    XMapper.set(e, x);
  }


  /**
   * set the parameter of the event associated with the Y
   */
  public void setY(ScrEvent e, int y) 
  {
    YMapper.set(e, y);
  }
  

  /**
   * get the Lenght value for this event
   */
  public int getLenght(ScrEvent e) 
  {
    if (LenghtMapper != null) return LenghtMapper.get(e);
    else return 0;
  }

  /**
   * sets the parameter of the event associated with the Lenght
   */
  public void setLenght(ScrEvent e, int lenght) 
  {
    LenghtMapper.set(e, lenght);
  }

  /**
   * get the label value (int) for this event
   */
  public int getLabel(ScrEvent e) 
  {
    if (LabelMapper != null) return LabelMapper.get(e);
    else return 0;
  }

  /**
   * sets the parameter of the event associated with the Label
   */
  public void setLabel(ScrEvent e, int lenght) 
  {
    LabelMapper.set(e, lenght);
  }

  /**
   * set the zoom factor for the x coordinates (percentage)
   */
  public void setXZoom(int factor) 
  {
    xZoomFactor = factor/(float)100;
    notifyZoom(xZoomFactor);
  }


  /**
   * get the zoom factor for the x coordinates (percentage)
   */
  public float getXZoom() 
  {
    return xZoomFactor;
  }

  /**
   * get the zoom factor for the y coordinates (percentage)
   */
  public float getYZoom() 
  {
    return yZoomFactor;
  }

  /**
   * set the zoom factor for the y coordinates (percentage)
   */
  public void setYZoom(int factor) 
  {
    yZoomFactor = factor/100;
  }


  /**
   * set the transposition amount for the x coordinates (pixels)
   */
  public void setXTransposition(int xT) 
  {
    xTranspose = xT;
    notifyTransposition(xT);
  }

  /**
   * the x transposition 
   */
  public int getXTransposition() 
  {
    return xTranspose;
  }

  /**
   * set the transposition amount for the y coordinates (pixels)
   */
  public void setYTransposition(int yT) 
  {
    yTranspose = yT;
  } 


  /**
   * set the inversion flag for the x coordinates
   */
  public void setXInvertion(boolean b) 
  {
    xInvertion = b;
  }


  /**
   * set the inversion flag for the y coordinates
   */
  public void setYInvertion(boolean b) 
  {
    yInvertion = b;
  }


  /**
   * edit this adapter (settings dialog)
   */
  public void edit(Frame theFrame) 
  {
    SettingsDialog asd = SettingsDialog.createSettingsDialog(this, theFrame);
    asd.pack();
    asd.setVisible(true);
  }


  /**
   * return the Adapter's name
   */
  public String getName() 
  {
    return itsName;
  } 

  /**
   * MappingListener interface
   */
  public void mappingChanged(String graphicName, String scoreName) {}


  /**
   * called by objects that will be informed when the zoom factor changes
   */
  public void addZoomListener(ZoomListener listener)
  {
    zoomListeners.addElement(listener);
  }

  /**
   * called by objects that will be informed when the zoom factor changes
   */
  public void removeZoomListener(ZoomListener listener)
  {
    zoomListeners.removeElement(listener);
  }

  private void notifyZoom(float newZoom)
  {
    ZoomListener aListener;

    for (Enumeration e = zoomListeners.elements(); e.hasMoreElements();) 
      {
	aListener = (ZoomListener) e.nextElement();

	aListener.zoomChanged(newZoom);
      }
  }

  /**
   * called by objects that will be informed when the transposition factor changes
   */
  public void addTranspositionListener(TranspositionListener listener)
  {
    transpositionListeners.addElement(listener);
  }

  /**
   * remove the listener
   */
  public void removeTranspositionListener(TranspositionListener listener)
  {
    transpositionListeners.removeElement(listener);
  }

  private void notifyTransposition(int newTransposition)
  {
    TranspositionListener aListener;

    for (Enumeration e = transpositionListeners.elements(); e.hasMoreElements();) 
      {
	aListener = (TranspositionListener) e.nextElement();

	aListener.transpositionChanged(newTransposition);
      }
  }

  //--- Fields
  Mapper XMapper;
  Mapper YMapper;
  Mapper LenghtMapper;
  Mapper LabelMapper;

  float xZoomFactor;
  float yZoomFactor;
  int xTranspose;
  int yTranspose;
  boolean xInvertion;
  boolean yInvertion;

  String itsName;
  Vector zoomListeners = new Vector();
  Vector transpositionListeners = new Vector();
}

