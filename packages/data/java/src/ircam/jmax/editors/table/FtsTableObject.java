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

package ircam.jmax.editors.table;

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
public class FtsTableObject extends FtsObjectWithEditor implements TableDataModel
{

  /**
   * constructor.
   */
    public FtsTableObject(Fts fts, FtsObject parent, String variableName, String classname, int nArgs, FtsAtom args[])
    {
	super(fts, parent, variableName, classname, classname + " " + FtsParse.unparseArguments(nArgs, args));

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
	if(getEditorFrame() == null)
	    setEditorFrame( new Tabler(this));

	showEditor();
    }
    /**
     * Fts callback: destroy the editor associated with this FtsSequenceObject.
     */
  public void destroyEditor(int nArgs, FtsAtom args[])
  {
      disposeEditor();
  }

  public void setSize(int nArgs, FtsAtom args[])
  {
    if(size != args[0].getInt())
    {
      size = args[0].getInt();
    
      int[] temp =  new int[size];
      if(lastIndex > size-1) lastIndex = size-1; 
      for(int i = 0; i< lastIndex; i++)
	temp[i] = visibles[i];

      visibles = temp;

      notifySizeChanged(size);
    }
  }

  private int lastIndex = 0;
  public void setVisibles(int nArgs , FtsAtom args[])
  {
    int i = 0;
    size = args[0].getInt();    
    visibleSize = args[1].getInt();    
    visibles = new int[size];

    if (isInGroup()) 
	for(i = 0; i<nArgs-2; i++)
        {
	  postEdit(new UndoableValueSet(this, i, visibles[i]));
	  visibles[i] = args[i+2].getInt();
	}
    else
	for(i = 0; i<nArgs-2; i++)
	    visibles[i] = args[i+2].getInt();
    
    lastIndex = i;
    
    notifySizeChanged(size);

    if((size <= lastIndex)||(visibleSize <= lastIndex))
	notifySet();

    setDirty();
  }
  public int getLastUpdatedIndex()
  {
    return lastIndex;
  }

  public void appendVisibles(int nArgs , FtsAtom args[])
  {
    int startIndex = args[0].getInt();
    int i=0;

    if (isInGroup()) 
	for(i = 0; ((i<nArgs-1)&&(startIndex+i<size)); i++)
	{      
	  postEdit(new UndoableValueSet(this, startIndex+i, visibles[startIndex+i]));
	  visibles[startIndex+i] = args[i+1].getInt();
	}
    else
	for(i = 0; ((i<nArgs-1)&&(startIndex+i<size)); i++)
	  visibles[startIndex+i] = args[i+1].getInt();
    
    if(startIndex+i > lastIndex)
	lastIndex = startIndex+i;
    notifySet();
    setDirty();
  }  
  
  public void startEdit(int nArgs , FtsAtom args[])
  {
    beginUpdate();
  }
  public void endEdit(int nArgs , FtsAtom args[])
  {
    endUpdate();
  }
    void printVisibles()
    {
	System.err.println("printvisibles ");
	for(int i=0;i<visibleSize-9;i+=10)
	    {
		System.err.println(" "+visibles[i]+" "+visibles[i+1]+" "+visibles[i+2]+" "+visibles[i+3]+" "+visibles[i+4]+
				   " "+visibles[i+5]+" "+visibles[i+6]+" "+visibles[i+7]+" "+visibles[i+8]+" "+visibles[i+9]);
	    }
    }

  public void setPixels(int nArgs , FtsAtom args[])
  {
      int i = 0;      
      pixelsSize = args[0].getInt();    
      pixels = new int[pixelsSize];

      /*if (isInGroup()) 
	for(i = 0; i<nArgs-1; i++)
	pixels[i] = args[i+1].getInt();
	else*/
      for(i = 0; i<nArgs-1; i++)
	  pixels[i] = args[i+1].getInt();

      if(pixelsSize <= nArgs-1)
	  notifySet();
  }
    
  public void appendPixels(int nArgs , FtsAtom args[])
  {
    int startIndex = args[0].getInt();
    int i=0;

    for(i = 0; i<nArgs-1; i++)
	pixels[startIndex+i] = args[i+1].getInt();

    if(pixelsSize <= startIndex+nArgs-1)
	notifySet();
  }

  public void addPixels(int nArgs , FtsAtom args[])
  {
    int startIndex = args[0].getInt();
    int i=0;
    int newp = nArgs-1;
    int[] temp = new int[pixelsSize];    

    if(startIndex==0)
    {
	for(i = 0; i < newp; i++)
	    temp[i] = args[i+1].getInt();
	for(i = newp; i< pixelsSize; i++)
	    temp[i] = pixels[i-newp];
    }
    else
    {
	for(i = 0; i<pixelsSize-newp; i++)
	  temp[i] = pixels[i+newp];
	for(i = 1; i<= newp; i++)
	    temp[pixelsSize-newp-1+i] = args[i].getInt();
    }
    pixels = temp;
    notifySet();
  }

  /*
  ** Requests to the server
  */
    public void requestSetValue(int index, int value)
    {
      sendArgs[0].setInt(index); 
      sendArgs[1].setInt(value); 
      sendMessage(FtsObject.systemInlet, "set_from_client", 2, sendArgs);
    }
    int countSetRequests = 0;
    public void requestSetValues(int[] values, int startIndex, int size)
    {
	int send;
	int current = 0;
	int num = size;

	while(num > 0)
	    {
		sendArgs[0].setInt(startIndex+current); 
		send = (num > NUM_ARGS-1)? NUM_ARGS-1 : num;
		
		for(int i=0; i < send; i++)
		    sendArgs[i+1].setInt(values[current+i]); 

		sendMessage(FtsObject.systemInlet, "set_from_client", send+1, sendArgs);
		
		current += send;
		num -= send;
	    }
    }

    public void requestSetVisibleWindow(int size, int startIndex, float zoom, int sizePixels)
    {
      sendArgs[0].setInt(size+10); 
      sendArgs[1].setInt(startIndex); 
      sendArgs[2].setFloat(zoom); 
      sendArgs[3].setInt(sizePixels); 

      sendMessage(FtsObject.systemInlet, "set_visible_window", 4, sendArgs);
    }
    public void requestEndEdit()
    {
      sendMessage(FtsObject.systemInlet, "end_edit", 0, null);
    }

    private boolean firstTime = false;
    public void requestGetValues()
    {
	sendMessage(FtsObject.systemInlet, "get_from_client", 0, null);
	firstTime = true;
    } 
    public void requestGetValues(int first, int last)
    { 
	if(!firstTime) requestGetValues();
	else
	    {
		sendArgs[0].setInt(first); 
		sendArgs[1].setInt(last); 
		sendMessage(FtsObject.systemInlet, "get_from_client", 2, sendArgs);
	    }
    }
    public void requestGetPixels(int deltax, int deltap)
    { 
	if(deltax==0)	    
	    sendMessage(FtsObject.systemInlet, "get_pixels_from_client", 0, null);
	else
	    {
		sendArgs[0].setInt(deltax); 
		sendArgs[1].setInt(deltap); 
		sendMessage(FtsObject.systemInlet, "get_pixels_from_client", 2, sendArgs);
	    }
	firstTime = false;
    }
    boolean thereIsAcopy = false;
    public boolean thereIsACopy()
    {
	return thereIsAcopy;
    }
    public void requestCopy(int startIndex, int size)
    {
	sendArgs[0].setInt(startIndex); 
	sendArgs[1].setInt(size); 
	sendMessage(FtsObject.systemInlet, "copy_from_client", 2, sendArgs);
	thereIsAcopy = true;
    }
    public void requestCut(int startIndex, int size, int vsize, int pixsize)
    {
	sendArgs[0].setInt(vsize); 
	sendArgs[1].setInt(pixsize); 
	sendArgs[2].setInt(startIndex); 
	sendArgs[3].setInt(size); 
	sendMessage(FtsObject.systemInlet, "cut_from_client", 4, sendArgs);
	thereIsAcopy = true;
	
	clearAllUndoRedo();
    }
    public void requestPaste(int startIndex, int size)
    {
	sendArgs[0].setInt(startIndex);
	sendArgs[1].setInt(size);  
	sendMessage(FtsObject.systemInlet, "paste_from_client", 2, sendArgs);
		
	clearAllUndoRedo();
    }
    public void requestInsert(int startIndex, int vsize, int pixsize)
    {
	sendArgs[0].setInt(vsize); 
	sendArgs[1].setInt(pixsize); 
	sendArgs[2].setInt(startIndex); 
	sendMessage(FtsObject.systemInlet, "insert_from_client", 3, sendArgs);
    	
	clearAllUndoRedo();
    }

    private int[] visibles;
    private int visibleSize = 0;
    private int[] pixels;
    private int pixelsSize = 0;

    public int getVisibleSize()
    {
	return visibleSize;
    }
    public int getVisibleValue(int index)
    {
	return visibles[index];
    }
    public int getPixelsSize()
    {
	return pixelsSize;
    }
    public int getPixel(int index)
    {
	if(index >= pixelsSize) return 0;
	else
	    return pixels[index];
    }

    public void closeEditor()
    {
	sendMessage(FtsObject.systemInlet, "close_editor", 0, null);
    }

    private int[] values;
    public int getSize()
    {
      return size;
    }
    public int getVerticalSize(){return Math.abs(max()-min());}
    public int max()
    {
	if(values == null) return 0;

	int max = values[0];
	for (int i = 0; i < getSize(); i++)
	    {
		if (values[i] > max) max = values[i];
	    }
	return max;
    }
    public int min()
    {
	if(values == null) return 0;

	int min = values[0];
	for (int i = 0; i < getSize(); i++)
	    {
		if (values[i] < min) min = values[i];
	    }	
	return min;
    }
    public int[] getValues()
    {
	return values;
    }
    public int getValue(int index)
    {
	return values[index];
    }
    /* public void setValue(int index, int value)
       {
       if (index < 0 || index >= getSize()) return;
       if (values[index] != value)
       {
       if (isInGroup()) 
       postEdit(new UndoableValueSet(this, index, values[index]));
       values[index] = value;
		
       //itsData.changed(index);
       }
       }
       public void setValues(int vals[], int startIndex, int lenght)
       {
       if (startIndex > getSize()) return;
       if (startIndex < 0) startIndex = 0;
       if (startIndex + lenght > getSize()) lenght = getSize()-startIndex;
       
       
       for (int i = 0; i < lenght; i++)
       {
       if (values[startIndex + i] != vals[i])
       {
	  
       if (isInGroup()) 
       postEdit(new UndoableValueSet(this, startIndex+i, values[startIndex+i]));    
       values[startIndex+i] = vals[i];
       
       //itsData.changed(startIndex+i);
       }		
       }
       }*/

    public void interpolate(int start, int end, int startValue, int endValue)
    {
	float coeff;
	if (startValue != endValue) 
	    coeff = ((float)(startValue - endValue))/(end - start);
	else coeff = 0;

	prepareBuffer(end-start+1);
	  
	if (end >= getSize()) 
	    end = getSize()-1;
	
	for (int i = start; i < end; i+=1)
	    {
		buffer[i-start] = (int)(startValue-Math.abs(i-start)*coeff + 0.5);
	    }
	
	requestSetValues(buffer, start, end-start);
    }
    /**
     * Utility private function to allocate a buffer used during the interpolate operations.
     * The computation is done in a private vector that is stored in one shot. */  
    static int buffer[];
    private static void prepareBuffer(int lenght)
    {
	if (buffer == null || buffer.length < lenght)
	    buffer = new int[lenght];
    }
    
  /**
   * Require to be notified when database change
   */
  public void addListener(TableDataListener theListener) 
  {
    listeners.addElement(theListener);
  }

  /**
   * Remove the listener
   */
  public void removeListener(TableDataListener theListener) 
  {
    listeners.removeElement(theListener);
  }

 /**
  * utility to notify the data base change to all the listeners
  */

  private void notifySizeChanged(int size)
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	((TableDataListener) e.nextElement()).sizeChanged(size);
  }

  private void notifySet()
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	((TableDataListener) e.nextElement()).tableSetted();
  }

  private void notifyClear()
  {
    for (Enumeration e = listeners.elements(); e.hasMoreElements();) 
	((TableDataListener) e.nextElement()).tableCleared();
  }
    
  ///////////////////////////////////////////////////////////
  private Vector points = new Vector();
  MaxVector listeners = new MaxVector();
  
  public final static int NUM_ARGS = /*128*/256;
  public static FtsAtom[] sendArgs = new FtsAtom[NUM_ARGS];
  
  private int size = 0;

  static
  {
    for(int i=0; i<NUM_ARGS; i++)
      sendArgs[i]= new FtsAtom();
  }
}











