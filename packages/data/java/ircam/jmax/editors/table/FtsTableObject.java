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

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
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
  static
  {
    FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("setSize"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsTableObject)obj).setSize(args.getInt( 0));
	}
      });
      FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("setVisibles"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, FtsArgs args)
	  {
	    ((FtsTableObject)obj).setVisibles( args.getLength(), args.getAtoms());
	  }
	});
      FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("appendVisibles"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, FtsArgs args)
	  {
	    ((FtsTableObject)obj).appendVisibles(  args.getLength(), args.getAtoms());
	  }
      });
      FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("startEdit"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, FtsArgs args)
	  {
	    ((FtsTableObject)obj).startEdit();
	  }
	});
      FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("endEdit"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, FtsArgs args)
	  {
	    ((FtsTableObject)obj).endEdit();
	  }
      });
      FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("setPixels"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, FtsArgs args)
	  {
	    ((FtsTableObject)obj).setPixels( args.getLength(), args.getAtoms());
	  }
	});
      FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("appendPixels"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, FtsArgs args)
	  {
	    ((FtsTableObject)obj).appendPixels( args.getLength(), args.getAtoms());
	  }
	});
      FtsObject.registerMessageHandler( FtsTableObject.class, FtsSymbol.get("addPixels"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, FtsArgs args)
	  {
	    ((FtsTableObject)obj).addPixels( args.getLength(), args.getAtoms());
	  }
      });
  }

  /**
   * constructor.
   */
  public FtsTableObject(FtsServer server, FtsObject parent, int objId, String classname, FtsAtom args[], int offset, int length)
  {
    super(server, parent, objId, classname, args, offset, length);
    
    listeners = new MaxVector();
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //// MESSAGES called from fts.
  //////////////////////////////////////////////////////////////////////////////////////
  
  public void openEditor(int argc, FtsAtom[] argv)
  {
    if(getEditorFrame() == null)
      setEditorFrame( new Tabler(this));
    
    showEditor();
  }
  
  public void destroyEditor()
  {
    disposeEditor();
  }

  public void setSize(int newSize)
  {
    if(size != newSize)
      {
	size = newSize;
	
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
    size = args[0].intValue;    
    visibleSize = args[1].intValue;    
    visibles = new int[size];

    if (isInGroup()) 
	for(i = 0; i<nArgs-2; i++)
        {
	  postEdit(new UndoableValueSet(this, i, visibles[i]));
	  visibles[i] = args[i+2].intValue;
	}
    else
	for(i = 0; i<nArgs-2; i++)
	    visibles[i] = args[i+2].intValue;
    
    lastIndex = i;
    
    notifySizeChanged(size);

    if((size <= lastIndex)||(visibleSize <= lastIndex))
	notifySet();
  }
  public int getLastUpdatedIndex()
  {
    return lastIndex;
  }

  public void appendVisibles(int nArgs , FtsAtom args[])
  {
    int startIndex = args[0].intValue;
    int i=0;

    if (isInGroup()) 
	for(i = 0; ((i<nArgs-1)&&(startIndex+i<size)); i++)
	{      
	  postEdit(new UndoableValueSet(this, startIndex+i, visibles[startIndex+i]));
	  visibles[startIndex+i] = args[i+1].intValue;
	}
    else
	for(i = 0; ((i<nArgs-1)&&(startIndex+i<size)); i++)
	  visibles[startIndex+i] = args[i+1].intValue;
    
    if(startIndex+i > lastIndex)
	lastIndex = startIndex+i;
    notifySet();
  }  
  
  public void startEdit()
  {
    beginUpdate();
  }
  public void endEdit()
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
      pixelsSize = args[0].intValue;    
      pixels = new int[pixelsSize];

      for(i = 0; i<nArgs-1; i++)
	  pixels[i] = args[i+1].intValue;

      if(pixelsSize <= nArgs-1)
	  notifySet();
  }
    
  public void appendPixels(int nArgs , FtsAtom args[])
  {
    int startIndex = args[0].intValue;
    int i=0;

    for(i = 0; i<nArgs-1; i++)
	pixels[startIndex+i] = args[i+1].intValue;

    if(pixelsSize <= startIndex+nArgs-1)
	notifySet();
  }

  public void addPixels(int nArgs , FtsAtom args[])
  {
    int startIndex = args[0].intValue;
    int i=0;
    int newp = nArgs-1;
    int[] temp = new int[pixelsSize];    

    if(startIndex==0)
    {
	for(i = 0; i < newp; i++)
	    temp[i] = args[i+1].intValue;
	for(i = newp; i< pixelsSize; i++)
	    temp[i] = pixels[i-newp];
    }
    else
    {
	for(i = 0; i<pixelsSize-newp; i++)
	  temp[i] = pixels[i+newp];
	for(i = 1; i<= newp; i++)
	    temp[pixelsSize-newp-1+i] = args[i].intValue;
    }
    pixels = temp;
    notifySet();
  }

  /*
  ** Requests to the server
  */
    public void requestSetValue( int index, int value)
    {
	args.clear();
	args.addInt( index);
	args.addInt( value);
      
	try{
	  send( FtsSymbol.get("set_from_client"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsTableObject: I/O Error sending set_from_client Message!");
	    e.printStackTrace(); 
	  }
    }
    public void requestSetValues(int[] values, int startIndex, int size)
    {
	args.clear();
	args.addInt( startIndex);
		
	for(int i=0; i < size; i++)
	  args.addInt( values[i]);

	try{
	  send( FtsSymbol.get("set_from_client"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsTableObject: I/O Error sending set_from_client Message!");
	    e.printStackTrace(); 
	  }
    }
  
    public void requestSetVisibleWindow(int size, int startIndex, float zoom, int sizePixels)
    {
      args.clear();
      args.addInt(size+10);
      args.addInt(startIndex);
      args.addFloat(zoom);
      args.addInt(sizePixels);
	
      try{
	send( FtsSymbol.get("set_visible_window"), args);
      }
      catch(IOException e)
	{
	  System.err.println("FtsTableObject: I/O Error sending set_visible_window Message!");
	  e.printStackTrace(); 
	}
    }
    public void requestEndEdit()
    {
      try{
	send( FtsSymbol.get("end_edit"));
      }
      catch(IOException e)
	{
	  System.err.println("FtsTableObject: I/O Error sending end_edit Message!");
	  e.printStackTrace(); 
	}
    }

  private boolean firstTime = false;
  public void requestGetValues()
  {
    try{
      send( FtsSymbol.get("get_from_client"));
    }
    catch(IOException e)
      {
	System.err.println("FtsTableObject: I/O Error sending get_from_client Message!");
	e.printStackTrace(); 
      }

    firstTime = true;
  } 
  public void requestGetValues(int first, int last)
  { 
    if(!firstTime) requestGetValues();
    else
      {
	args.clear();
	args.addInt(first);
	args.addInt(last);
		
	try{
	  send( FtsSymbol.get("get_from_client"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsTableObject: I/O Error sending get_from_client Message!");
	    e.printStackTrace(); 
	  }
      }
  }
  public void requestGetPixels(int deltax, int deltap)
  { 
    if(deltax==0)	    
      try{
	send( FtsSymbol.get("get_pixels_from_client"));
      }
      catch(IOException e)
	{
	  System.err.println("FtsTableObject: I/O Error sending get_pixels_from_client Message!");
	  e.printStackTrace(); 
	}
    else
      {
	args.clear();
	args.addInt(deltax);
	args.addInt(deltap);
	
	try{
	  send( FtsSymbol.get("get_pixels_from_client"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsTableObject: I/O Error sending get_pixels_from_client Message!");
	    e.printStackTrace(); 
	  }
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
    args.clear();
    args.addInt(startIndex);
    args.addInt(size);
    
    try{
      send( FtsSymbol.get("copy_from_client"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTableObject: I/O Error sending copy_from_client Message!");
	e.printStackTrace(); 
      }
    thereIsAcopy = true;
  }
  public void requestCut(int startIndex, int size, int vsize, int pixsize)
  {
    args.clear();
    args.addInt( vsize);
    args.addInt( pixsize);
    args.addInt( startIndex);
    args.addInt( size);
	
    try{
      send( FtsSymbol.get("cut_from_client"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTableObject: I/O Error sending cut_from_client Message!");
	e.printStackTrace(); 
      }
    
    thereIsAcopy = true;	
    clearAllUndoRedo();
  }
  public void requestPaste(int startIndex, int size)
  {
    args.clear();
    args.addInt( startIndex);
    args.addInt( size);
	
    try{
      send( FtsSymbol.get("paste_from_client"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTableObject: I/O Error sending paste_from_client Message!");
	e.printStackTrace(); 
      }
    
    clearAllUndoRedo();
  }
  public void requestInsert(int startIndex, int vsize, int pixsize)
  {
    args.clear();
    args.addInt( vsize);
    args.addInt( pixsize);
    args.addInt( startIndex);
    
    try{
      send( FtsSymbol.get("insert_from_client"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsTableObject: I/O Error sending insert_from_client Message!");
	e.printStackTrace(); 
      }
    
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
  private int size = 0;
}










