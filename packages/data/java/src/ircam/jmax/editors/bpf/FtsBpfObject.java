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

package ircam.jmax.editors.bpf;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

/**
 * A concrete implementation of the SequenceDataModel,
 * this class represents a model of a set of tracks.
 */
public class FtsBpfObject extends FtsObjectWithEditor implements BpfDataModel
{

  /**
   * constructor.
   */
    public FtsBpfObject(Fts fts, FtsObject parent, String variableName, String classname)
    {
	super(fts, parent, variableName, "bpf", "bpf");
	
	listeners = new MaxVector();
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //// MESSAGES called from fts.
    //////////////////////////////////////////////////////////////////////////////////////

    /**
     * Fts callback: open the editor associated with this FtsSequenceObject.
     * If not exist create them else show them.
     */
    public void openEditor(int nArgs, FtsAtom args[])
    {
      if(bpf == null){
	  bpf = new Bpf(this);
	  setEditorFrame(bpf);
      }
      if (! bpf.isVisible())
	  {
	      bpf.setVisible(true);
	      MaxWindowManager.getWindowManager().addWindow(bpf);
	  }   
      bpf.toFront();
    }

    /**
     * Fts callback: destroy the editor associated with this FtsSequenceObject.
     */
    public void destroyEditor(int nArgs, FtsAtom args[])
    {
	if(bpf != null)
	    {
		bpf.dispose();
		bpf = null;
		setEditorFrame(null);
	    }
    }

  public void addPoint(int nArgs , FtsAtom args[])
  {
      int index = args[0].getInt();
      
      addPoint(index, new BpfPoint(args[1].getFloat(), args[2].getFloat()));

      notifyPointAdded(index);

      setDirty();
  }
  
  public void removePoints(int nArgs , FtsAtom args[])
  {
      int[] indxs = new int[nArgs];
      
      for(int i = 0; i<nArgs;i++)
	  indxs[i] = args[i].getInt();

      if(nArgs>1)
	  bubbleSort(indxs);

      for(int i=0; i<nArgs; i++)
	  removePoint(indxs[i]);	

      notifyPointsDeleted(indxs);
      setDirty();
  }
 
    void bubbleSort(int a[])
    {
	boolean flag = true;
	int temp;

	while(flag) {
	    flag = false;
	    for (int i = 0; i<a.length-1; i++)
		if (a[i] < a[i+1])
		    {
			temp = a[i];
			a[i] = a[i+1];
			a[i+1] = temp;
			flag = true;
		    }
	} 
    }

  public void setPoint(int nArgs , FtsAtom args[])
  {
      int oldIndex = args[0].getInt();
      int newIndex = oldIndex;
      float newTime = args[1].getFloat();
      float newValue = args[2].getFloat();

      BpfPoint point = getPointAt(oldIndex);
      point.setValue(newValue);
      if(point.getTime()!=newTime)
	  {
	      point.setTime(newTime);
	      points.removeElementAt(oldIndex);
	      newIndex = getPreviousPointIndex(newTime)+1;
	      points.insertElementAt(point, newIndex);
	  }
      notifyPointChanged(oldIndex, newIndex, newTime, newValue);
      setDirty();
  }

  public void clear(int nArgs , FtsAtom args[])
  {
      removeAllPoints();
  }

  public void set(int nArgs , FtsAtom args[])
    {
	removeAllPoints();
	int j=0;
	for(int i = 0; i<nArgs; i+=2)
	    addPoint(j++, new BpfPoint(args[i].getFloat(), args[i+1].getFloat()));
	
	notifyPointAdded(j);
	setDirty();
    }
  public void append(int nArgs , FtsAtom args[])
    {
	int j=0;
	for(int i = length(); i<length()+nArgs; i+=2)
	    addPoint(j++, new BpfPoint(args[i].getFloat(), args[i+1].getFloat()));
	
	notifyPointAdded(j);
	setDirty();
    }
    
  /*
  ** Requests to the server
  */
  public void requestPointCreation(int index, float time, float value)
  {
    sendArgs[0].setInt(index); 
    sendArgs[1].setFloat(time); 
    sendArgs[2].setFloat(value); 
    sendMessage(FtsObject.systemInlet, "add_point", 3, sendArgs);
  }

    public void requestSetPoint(int index, float time, float value)
    {
	sendArgs[0].setInt(index); 
	sendArgs[1].setFloat(time); 
	sendArgs[2].setFloat(value); 
	sendMessage(FtsObject.systemInlet, "set_point", 3, sendArgs);
    }

    public void requestPointRemove(int index)
    {
	sendArgs[0].setInt(index); 
	sendMessage(FtsObject.systemInlet, "remove_points", 1, sendArgs);
    }

    public void requestPointsRemove(Enumeration en)
    {
	int i = 0;
	for(Enumeration e = en;en.hasMoreElements();)
	    {
		sendArgs[i++].setInt(indexOf((BpfPoint)e.nextElement())); 
	    }
	sendMessage(FtsObject.systemInlet, "remove_points", i, sendArgs);
    }

    public void closeEditor()
    {
	sendMessage(FtsObject.systemInlet, "close_editor", 0, null);
    }

    public void addPoint(int index, BpfPoint pt)
    {
	points.insertElementAt(pt, index);
    }

    public void removePoint(int index)
    {
	points.removeElementAt(index);
    }

    public BpfPoint getPointAt(int index)
    {
	if(index<points.size()&&(index>=0))
	    return (BpfPoint)points.elementAt(index);
	else
	    return null;
    }

    public void removeAllPoints()
    {
	points.removeAllElements();
    }

    public Enumeration getPoints()
    {
	return points.elements();
    }

    public BpfPoint getLastPoint()
    {
	if(points.size()!=0)	    
	    return (BpfPoint) points.lastElement();
	else
	    return null;
    }

    public BpfPoint getPreviousPoint(float time)
      {
	  int i = -1;
	  for(Enumeration e = points.elements(); e.hasMoreElements();)
	    {
		if(((BpfPoint)e.nextElement()).getTime() >= time) 
		    break;
		i++;
	    }
	  if((i<0)||(i >= points.size()))
	      return null; 
	  else
	      return getPointAt(i);
      }

    public int getPreviousPointIndex(float time)
      {
	  int i = -1;
	  for(Enumeration e = points.elements(); e.hasMoreElements();)
	    {
		if(((BpfPoint)e.nextElement()).getTime() > time) 
		    return i;
		i++;
	    }
	return i; 
      }

    public BpfPoint getNextPoint(float time)
    {
	if(length()==0) return null;
	int i = 0;
	for(Enumeration e = points.elements(); e.hasMoreElements();)
	    {
		if(((BpfPoint)e.nextElement()).getTime() > time) 
		    return getPointAt(i);
		i++;
	    }
	return null;
    }

    public BpfPoint getNextPoint(BpfPoint pnt)
    {
	int id = indexOf(pnt);	
	if((id!=-1)&&(id<length()))
	    return getPointAt(id+1);
	else return null;
    }

    public int indexOf(BpfPoint pnt)
    {
	int i = 0;
	for(Enumeration e = points.elements(); e.hasMoreElements();)
	    {
		if((BpfPoint)e.nextElement() == pnt) return i;
		i++;
	    }
	return -1; 
    }

    public int length()
    {
	return points.size();
    }

    public int movePointTo(int oldIndex, float newTime){
	BpfPoint point = getPointAt(oldIndex);
	point.setTime(newTime);
	points.removeElementAt(oldIndex);
	int newIndex = getPreviousPointIndex(newTime)+1;
	points.insertElementAt(point, newIndex);
	return newIndex;
    }
  /**
   * Require to be notified when database change
   */
  public void addBpfListener(BpfDataListener theListener) 
  {
    listeners.addElement(theListener);
  }

  /**
   * Remove the listener
   */
  public void removeBpfListener(BpfDataListener theListener) 
  {
    listeners.removeElement(theListener);
  }

 /**
  * utility to notify the data base change to all the listeners
  */
    
    private void notifyPointAdded(int index)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((BpfDataListener) e.nextElement()).pointAdded(index);
    }

    private void notifyPointsDeleted(int[] oldIndexs)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((BpfDataListener) e.nextElement()).pointsDeleted(oldIndexs);
    }
    
    private void notifyPointChanged(int oldIndex, int newIndex, float newTime, float newValue)
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((BpfDataListener) e.nextElement()).pointChanged(oldIndex, newIndex, newTime, newValue);
    }

    private void notifyClear()
    {
	for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	    ((BpfDataListener) e.nextElement()).cleared();
    }
    ///////////////////////////////////////////////////////////
    public Enumeration intersectionSearch(float start, float end, BpfAdapter adapter)
    {
	return new Intersection(start, end, adapter);
    }

    /**
     * an utility class to implement the intersection with a range */
    class Intersection implements Enumeration {
	Intersection(float start, float end, BpfAdapter ad)
	{
	    endTime = end;
	    startTime = start;
	    adapter = ad;

	    index = 0;
	} 
	
	public boolean hasMoreElements()
	{
	    nextObject = findNext(); 
	    
	    return nextObject != null;
	}
	
	public Object nextElement()
	{
	    
	    return nextObject;
	}
	
	private Object findNext()
	{
	    if (length() == 0) return null;
	    BpfPoint p;

	    while (index < length() && ((BpfPoint)points.elementAt(index)).getTime() <= endTime)
		{
		    p = (BpfPoint)points.elementAt(index++);
		    if (p.getTime() >= startTime ||
			p.getTime()+adapter.getInvLenght(p) >= startTime)
			return p;
		}
	    return null;
	}

	//--- Intersection Fields
	float endTime;
	float startTime;
	int index;
	Object nextObject = null;
	BpfAdapter adapter;
    }    

    ////////////////////////////////////////////////////////
  Bpf bpf = null;  
  
  private Vector points = new Vector();
  MaxVector listeners = new MaxVector();

  public final static int NUM_ARGS = 128;
  public static FtsAtom[] sendArgs = new FtsAtom[NUM_ARGS];

  static
    {
	for(int i=0; i<NUM_ARGS; i++)
	    sendArgs[i]= new FtsAtom();
    }
}











