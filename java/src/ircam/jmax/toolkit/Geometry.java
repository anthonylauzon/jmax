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

package ircam.jmax.toolkit;

import java.util.*;

/**
 * A class that handles the scroll and zoom values for a group of listeners.
 */
public class Geometry
{

    /**
     * Constructs a default Geometry object with standard values for
     * zoom (100%), transposition(0) and inversion fields(false) */
    public Geometry()
    {
	xZoomFactor = 1;
	yZoomFactor = 1;
	xTranspose = 0;
	yTranspose = 0;
	xInvertion = false;
	yInvertion = false;
	
    }

    /**
     * set the zoom factor for the x coordinates (percentage)
     */
    public void setXZoom(int factor) 
    {
	float old = xZoomFactor;
	xZoomFactor = factor/(float)100;
	notifyZoom(xZoomFactor, old);
    }
    
    public void incrXZoom(int delta) 
    {
	float old = xZoomFactor;
	float dlt = (float)delta;
	if(xZoomFactor < 1) dlt = delta*xZoomFactor;

	xZoomFactor = (xZoomFactor*100+dlt)/(float)100;
	if(xZoomFactor<0.01) xZoomFactor = (float)0.01;
	if(xZoomFactor>10) xZoomFactor = (float)10.0;
	notifyZoom(xZoomFactor, old);
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
     * the y transposition 
     */
    public int getYTransposition() 
    {
	return yTranspose;
    }

    /**
     * get the inversion flag for the x coordinates
     */
    public boolean getXInvertion() 
    {
	return xInvertion;
    }


    /**
     * set the inversion flag for the x coordinates
     */
    public void setXInvertion(boolean b) 
    {
	xInvertion = b;
    }
    
    /**
     * get the inversion flag for the y coordinates
     */
    public boolean getYInvertion() 
    {
	return yInvertion;
    }
    
    /**
     * set the inversion flag for the y coordinates
     */
    public void setYInvertion(boolean b) 
    {
	yInvertion = b;
    }
        
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
    
    private void notifyZoom(float newZoom, float oldZoom)
    {
	ZoomListener aListener;
	
	for (Enumeration e = zoomListeners.elements(); e.hasMoreElements();) 
	    {
		aListener = (ZoomListener) e.nextElement();
		
		aListener.zoomChanged(newZoom, oldZoom);
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
    
    /*static*/public int sizeToMsec(Geometry geometry, int size)
    {
	if (geometry.getXInvertion()) return (int) (geometry.getXTransposition() -(size)/geometry.getXZoom());
	
	else return (int) ((size)/geometry.getXZoom() - geometry.getXTransposition());
    
    }
    
    //-------  Geometry fields
    float xZoomFactor;
    float yZoomFactor;
    int xTranspose;
    int yTranspose;
    boolean xInvertion;
    boolean yInvertion;
 
    Vector zoomListeners = new Vector();
    Vector transpositionListeners = new Vector();
   
}
