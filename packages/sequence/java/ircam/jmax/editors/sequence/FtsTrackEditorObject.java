//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.sequence;

import ircam.fts.client.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.awt.datatransfer.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import java.awt.Rectangle;

public class FtsTrackEditorObject extends FtsObject 
{
	static
  {
		FtsObject.registerMessageHandler( FtsTrackEditorObject.class, FtsSymbol.get("editor"), new FtsMessageHandler(){
			public void invoke( FtsObject obj, FtsArgs args)
		  {
				((FtsTrackEditorObject)obj).setEditorState(args.getLength(), args.getAtoms());		  
		  }
		});
		FtsObject.registerMessageHandler( FtsTrackEditorObject.class, FtsSymbol.get("columns"), new FtsMessageHandler(){
			public void invoke( FtsObject obj, FtsArgs args)
		  {
				((FtsTrackEditorObject)obj).restoreTableColumns(args.getLength(), args.getAtoms());		  
		  }
		});
    FtsObject.registerMessageHandler( FtsTrackEditorObject.class, FtsSymbol.get("props_to_draw"), new FtsMessageHandler(){
			public void invoke( FtsObject obj, FtsArgs args)
		  {
				((FtsTrackEditorObject)obj).restorePropertiesToDraw(args.getLength(), args.getAtoms());		  
		  }
		});
    FtsObject.registerMessageHandler( FtsTrackEditorObject.class, FtsSymbol.get("grid_mode"), new FtsMessageHandler(){
			public void invoke( FtsObject obj, FtsArgs args)
		  {
				((FtsTrackEditorObject)obj).restoreGridMode(args.getLength(), args.getAtoms());		  
		  }
		});
		FtsObject.registerMessageHandler( FtsTrackEditorObject.class, FtsSymbol.get("range"), new FtsMessageHandler(){
			public void invoke( FtsObject obj, FtsArgs args)
		  {
				((FtsTrackEditorObject)obj).restoreRange((float)args.getDouble(0), (float)args.getDouble(1));		  
		  }
		});
    FtsObject.registerMessageHandler( FtsTrackEditorObject.class, FtsSymbol.get("tableSize"), new FtsMessageHandler(){
			public void invoke( FtsObject obj, FtsArgs args)
		  {
				((FtsTrackEditorObject)obj).restoreTableSize(args.getLength(), args.getAtoms());		  
		  }
		});
  }

public FtsTrackEditorObject(FtsServer server, FtsObject parent, int objId)
{
		super(server, parent, objId);	
		
		wx = -1;
		wy = -1;
		ww = -1;
		wh = -1;
		tab_w = -1;
		tab_h = -1;
		label = "";
		zoom = (float)0.2;
		transp = 0;
		view = 0;
		rangeMode = 0;
    gridMode = 0;
		trackObj = (FtsTrackObject)parent;		
		columnNames = new Vector();
    propsToDraw = new Vector();
    rangeMin = (float)0.0;
    rangeMax = (float)1.0;
}

public void setEditorState( int nArgs, FtsAtom args[])
{
  if( !trackObj.isInSequence())
  {
    int x = args[0].intValue;
		int y = args[1].intValue;
		int w = args[2].intValue;
		int h = args[3].intValue;
		String label = args[4].symbolValue.toString();
		float zoom = (float)args[5].doubleValue;
		int transp = args[6].intValue;
		int view = args[7].intValue;
		int rMode = args[8].intValue;
		float rMin = (float)0.0;
    float rMax = (float)1.0;
    if(nArgs > 8)
    {
      rMin = (float)args[9].doubleValue;
      rMax = (float)args[10].doubleValue;
    }
    
		if( x!= this.wx || y!=this.wy || w!=this.ww || h!=this.wh || 
				!label.equals(this.label) || zoom != this.zoom || 
				transp != this.transp || view != this.view || rMode != this.rangeMode ||
        rMin != this.rangeMin || rMax != this.rangeMax) 
		{	
			this.wx = x; this.wy = y; this.ww = w; this.wh = h; 
			this.label = label; this.zoom = zoom; this.transp = transp; 
			this.view = view; this.rangeMode = rMode;
      this.rangeMin = rMin; this.rangeMax = rMax;
      SwingUtilities.invokeLater(new Runnable() {
        public void run()
       { 
        trackObj.restoreEditorState();
       }
      });      
 		}
  }
  else
  {
    String label = args[0].symbolValue.toString();
    int view = args[1].intValue;
		int rMode = args[2].intValue;
    float rMin = (float)0.0;
    float rMax = (float)1.0;
    if(nArgs > 3)
    {
      rMin = (float)args[3].doubleValue;
      rMax = (float)args[4].doubleValue;
    }
    
    if(!label.equals(this.label) || view != this.view || rMode != this.rangeMode ||
       rMin != this.rangeMin || rMax != this.rangeMax) 
		{	
			this.label = label; 
			this.view = view; this.rangeMode = rMode;
      this.rangeMin = rMin; this.rangeMax = rMax;
      SwingUtilities.invokeLater(new Runnable() {
        public void run()
        { 
          trackObj.restoreEditorState();   		
        }});
    }
  }
}

public void restoreTableColumns( int nArgs, FtsAtom args[])
{
	columnNames.removeAllElements();
	for(int i = 0; i<nArgs; i++)
		columnNames.add( args[i].symbolValue.toString());
}

public void restorePropertiesToDraw( int nArgs, FtsAtom args[])
{
	propsToDraw.removeAllElements();
	for(int i = 0; i<nArgs; i++)
		propsToDraw.add( args[i].symbolValue.toString());  
}

public void restoreTableSize( int nArgs, FtsAtom args[])
{	
	if(nArgs==2)
	{
		this.tab_w = args[0].intValue;
		this.tab_h = args[1].intValue;	
	}
}

public void restoreGridMode( int nArgs, FtsAtom args[])
{	
	if(nArgs==1)
		this.gridMode = args[0].intValue;
}

public void restoreRange(float min, float max)
{
  this.rangeMin = min;
  this.rangeMax = max;
}

public Enumeration getTableColumns()
{
	if(columnNames.size() > 0)
		return columnNames.elements();
	else return null;
}

public Enumeration getPropertiesToDraw()
{
	if(propsToDraw.size() > 0)
		return propsToDraw.elements();
	else return null;
}

public void requestSetWindow()
{	
  if( !trackObj.isInSequence())
	{
    if(wx == -1 || wy == -1 || ww == -1 || wh == -1)
			return;

		args.clear();
		args.addInt( wx);
		args.addInt( wy);
		args.addInt( ww);
		args.addInt( wh);
		
		try{
			send( FtsSymbol.get("window"), args);
		}
		catch(IOException e)
		{
			System.err.println("FtsTrackEditorObject: I/O Error sending window Message!");
			e.printStackTrace(); 
		}
  }
}

public void setLocation(int x, int y)
{	
  if( !trackObj.isInSequence())
	{
		if(this.wx != x || this.wy != y)
		{			
			this.wx = x;
			this.wy = y;
			requestSetWindow();
		}
  }
}
public void setSize(int w, int h)
{		  
  if( !trackObj.isInSequence())
	{
    if(this.ww != w || this.wh != h)
    {			
      this.ww = w;
      this.wh = h;
      requestSetWindow();
    }
  }
}
public void setLabel(String lab)
{
		if(!this.label.equals(lab))
		{
			this.label = lab;
			args.clear();
			args.addSymbol( FtsSymbol.get(lab));
			
			try{
				send( FtsSymbol.get("label"), args);
			}
			catch(IOException e)
			{
				System.err.println("FtsTrackEditorObject: I/O Error sending label Message!");
				e.printStackTrace(); 
			}
		}
}

public void setZoom(float zoom)
{
  if( !trackObj.isInSequence() && !trackObj.isUploading() && !trackObj.isSequenceUploading())
	{
		if(this.zoom != zoom)
		{
			this.zoom = zoom;
			args.clear();
			args.addDouble( (double)zoom);
			
			try{
				send( FtsSymbol.get("zoom"), args);
			}
			catch(IOException e)
			{
				System.err.println("FtsTrackEditorObject: I/O Error sending zoom Message!");
				e.printStackTrace(); 
			}		
		}
  }
}
public void setTransposition(int transp)
{	
  if( !trackObj.isInSequence()  && !trackObj.isUploading() && !trackObj.isSequenceUploading())
	{
		if(this.transp != transp)
		{
			this.transp = transp;
			args.clear();
			args.addInt( transp);
			
			try{
				send( FtsSymbol.get("transp"), args);
			}
			catch(IOException e)
			{
				System.err.println("FtsTrackEditorObject: I/O Error sending transp Message!");
				e.printStackTrace(); 
			}
		}
  }
}
public void setViewMode(int view)
{		
		if(this.view != view  && !trackObj.isUploading() && !trackObj.isSequenceUploading())
		{    
			this.view = view;
			args.clear();
			args.addInt( view);
			
			try{
				send( FtsSymbol.get("view"), args);
			}
			catch(IOException e)
			{
				System.err.println("FtsTrackEditorObject: I/O Error sending viewMode Message!");
				e.printStackTrace(); 
			}
		}
}

public void setRangeMode(int rMode)
{	
		if(this.rangeMode != rMode  && !trackObj.isUploading() && !trackObj.isSequenceUploading())
		{     			
      this.rangeMode = rMode;
			args.clear();
			args.addInt( rangeMode);
			try{
				send( FtsSymbol.get("range_mode"), args);
			}
			catch(IOException e)
			{
				System.err.println("FtsTrackEditorObject: I/O Error sending rangeMode Message!");
				e.printStackTrace(); 
			}
		}
}

public void setRange(float min, float max)
{	
		if((this.rangeMin != min ||  this.rangeMax != max) && !trackObj.isUploading() && !trackObj.isSequenceUploading())
		{     
			this.rangeMin = min;
			this.rangeMax = max;
			args.clear();
			args.addDouble( (double)rangeMin);
			args.addDouble( (double)rangeMax);
			
			try{
				send( FtsSymbol.get("range"), args);
			}
			catch(IOException e)
			{
				System.err.println("FtsTrackEditorObject: I/O Error sending range Message!");
				e.printStackTrace(); 
			}
		}
}


public void setGridMode(int gridMode)
{	
		if(this.gridMode != gridMode  && !trackObj.isUploading() && !trackObj.isSequenceUploading())
		{     
			this.gridMode = gridMode;
			args.clear();
			args.addInt( gridMode);
			
			try{
				send( FtsSymbol.get("grid_mode"), args);
			}
			catch(IOException e)
			{
				System.err.println("FtsTrackEditorObject: I/O Error sending grid_mode Message!");
				e.printStackTrace(); 
			}
		}
}


public void setTableSize(int tw, int th)
{	
	if(tw == -1 || th == -1)
		return;
	
	if(this.tab_w != tw || this.tab_h != th)
	{
		tab_w = tw;
		tab_h = th;
		args.clear();
		args.addInt( tw);
		args.addInt( th);
		
		try{
			send( FtsSymbol.get("tableSize"), args);
		}
		catch(IOException e)
		{
			System.err.println("FtsTrackEditorObject: I/O Error sending tableSize Message!");
			e.printStackTrace(); 
		}
	}
}

public void setTableColumnOrder(int size, Enumeration colNames)
{	
	this.columnNames.removeAllElements();
	args.clear();
	
	String name;
	for(Enumeration e = colNames; e.hasMoreElements(); )
	{
		name = (String)e.nextElement();
		this.columnNames.add( name);
		args.addSymbol( FtsSymbol.get(name));
	}
	
	if( args.getLength() > 0)
	{
		try{
			send( FtsSymbol.get("columns"), args);
		}
		catch(IOException e)
	  {
			System.err.println("FtsTrackEditorObject: I/O Error sending table_column_order Message!");
			e.printStackTrace(); 
		}
	}
}

public void setPropertiesToDraw(Enumeration anenum)
{
  if( !trackObj.isInSequence())
	{
    this.propsToDraw.removeAllElements();
    args.clear();
	
    String name;
    for(Enumeration e = anenum; e.hasMoreElements(); )
    {
      name = (String)e.nextElement();
      this.propsToDraw.add( name);
      args.addSymbol( FtsSymbol.get(name));
    }
    
    if( args.getLength() > 0)
    {
      try{
        send( FtsSymbol.get("props_to_draw"), args);
      }
      catch(IOException e)
      {
        System.err.println("FtsTrackEditorObject: I/O Error sending prop_to_draw Message!");
        e.printStackTrace(); 
      }
    }  
  }
}
public void requestSetEditorState(Rectangle bounds)
{	
  if( !trackObj.isInSequence())
	{
    if(wx == -1)
      wx = bounds.x;
    if(wy == -1)
      wy = bounds.y;
    if(ww == -1)
      ww = bounds.width;
    if(wh == -1)
      wh = bounds.height;
	
    if(wx == -1 || wy == -1 || ww == -1 || wh == -1)
      return;
	
    args.clear();
    args.addInt( wx);
    args.addInt( wy);
    args.addInt( ww);
    args.addInt( wh);
    args.addSymbol( FtsSymbol.get(label));
    args.addDouble( (double)zoom);
    args.addInt( transp);
    args.addInt( view);
    args.addInt( rangeMode);
  }
  else
  {
    args.clear();
    args.addSymbol( FtsSymbol.get(label));
    args.addInt( view);
    args.addInt( rangeMode);    
  }
  if(rangeMin != 0.0 || rangeMax != 1.0)
  {
    args.addDouble( (double)rangeMin);
    args.addDouble( (double)rangeMax);
  }
  
  try{
    send( FtsSymbol.get("editor_state"), args);
  }
  catch(IOException e)
  {
    System.err.println("FtsTrackEditorObject: I/O Error sending state Message!");
    e.printStackTrace(); 
  } 
}

public void requestListenersNotify(FtsArgs listenerArgs)
{
	/*args.clear();

	for(int i=0; i<nArgs; i++)
		if(arguments[i] instanceof Double)
			args.addDouble(((Double)arguments[i]).doubleValue());
		else
			if( arguments[i] instanceof String)
				args.addSymbol( FtsSymbol.get( (String)arguments[i]));
		else
			args.add(arguments[i]);*/
	
	try{
		send( FtsSymbol.get("listenersNotify"), listenerArgs);
	}
	catch(IOException e)
	{
		System.err.println("FtsTrackEditorObject: I/O Error sending listenersNotify Message!");
		e.printStackTrace(); 
	}   
}


public boolean haveContent()
{
  if( !trackObj.isInSequence())
		return ((ww != -1) && (wh != -1) && (wx != -1) && (wy != -1));
  else
    return (!label.equals("") || view != 0 || rangeMode != 0 || gridMode != 0 || rangeMin != 0.0 || rangeMax != 1.0);
}

//////////////////////////////////////////////////////////////////////////////////	

public int wx, wy, ww, wh, transp, view, gridMode, tab_w, tab_h; 
public int rangeMode = 0;
public String label;
public float zoom, rangeMin, rangeMax;
public Vector columnNames, propsToDraw;
FtsTrackObject trackObj;
protected transient FtsArgs args = new FtsArgs();
}
