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
  }

public FtsTrackEditorObject(FtsServer server, FtsObject parent, int objId)
{
		super(server, parent, objId);	
		
		wx = -1;
		wy = -1;
		ww = -1;
		wh = -1;
		label = "";
		zoom = (float)0.2;
		transp = 0;
		view = 0;
		rangeMode = 0;
		trackObj = (FtsTrackObject)parent;		
}

public void setEditorState( int nArgs, FtsAtom args[])
{
		int x = args[0].intValue;
		int y = args[1].intValue;
		int w = args[2].intValue;
		int h = args[3].intValue;
		String label = args[4].symbolValue.toString();
		float zoom = (float)args[5].doubleValue;
		int transp = args[6].intValue;
		int view = args[7].intValue;
		int rangeMode = args[8].intValue;
				
		if( x!= this.wx || y!=this.wy || w!=this.ww || h!=this.wh || 
				!label.equals(this.label) || zoom != this.zoom || 
				transp != this.transp || view != this.view || rangeMode != this.rangeMode) 
		{	
			this.wx = x; this.wy = y; this.ww = w; this.wh = h; 
			this.label = label; this.zoom = zoom; this.transp = transp; 
			this.view = view; this.rangeMode = rangeMode;
			trackObj.restoreEditorState();   
		}
}

public void requestSetWindow()
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

public void setLocation(int x, int y)
{	
		if(this.wx != x || this.wy != y)
		{			
			this.wx = x;
			this.wy = y;
			requestSetWindow();
		}
}

public void setSize(int w, int h)
{		
		if(this.ww != w || this.wh != h)
		{			
			this.ww = w;
			this.wh = h;
			requestSetWindow();
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
		if(this.zoom != zoom)
		{
			this.zoom = zoom;
			args.clear();
			args.addFloat( zoom);
			
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

public void setTransposition(int transp)
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

public void setViewMode(int view)
{	
		if(this.view != view)
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

public void setRangeMode(int rangeMode)
{	
		if(this.rangeMode != rangeMode)
		{
			this.rangeMode = rangeMode;
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

public void requestSetEditorState(Rectangle bounds)
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
	args.addFloat( zoom);
	args.addInt( transp);
	args.addInt( view);
	args.addInt( rangeMode);
	try{
		send( FtsSymbol.get("editor_state"), args);
	}
	catch(IOException e)
	{
			System.err.println("FtsTrackEditorObject: I/O Error sending state Message!");
			e.printStackTrace(); 
	}
}

public boolean haveContent()
{
		return ((ww != -1) && (wh != -1) && (wx != -1) && (wy != -1));
}

/////////////////////////////////////////////////////////////////////////////////////////
public void requestEventCreation(float time, String type, int nArgs, Object arguments[])
{
		args.clear();
		args.addDouble( (double)time);
		args.addSymbol( FtsSymbol.get(type));
		
		for(int i=0; i<nArgs; i++)
		{
			if( arguments[i] instanceof Double)
				args.addDouble(((Double)arguments[i]).floatValue());
			else
				if(  arguments[i] instanceof String)
					args.addSymbol( FtsSymbol.get( (String)arguments[i]));
			else
				args.add(arguments[i]);
		}
		try{
			send( FtsSymbol.get("addEvent"), args);
		}
		catch(IOException e)
		{
			System.err.println("FtsTrackEditorObject: I/O Error sending addEvent Message!");
			e.printStackTrace(); 
		}
}

public void requestEventCreationWithoutUpload(float time, String type, int nArgs, Object arguments[])
{
	args.clear();
	args.addDouble( (double)time);
	args.addSymbol( FtsSymbol.get( type));
	
	for(int i=0; i<nArgs; i++)
		if(arguments[i] instanceof Double)
			args.addDouble(((Double)arguments[i]).doubleValue());
		else
			if( arguments[i] instanceof String)
				args.addSymbol( FtsSymbol.get( (String)arguments[i]));
		else
			args.add(arguments[i]);
	
	try{
		send( FtsSymbol.get("makeEvent"), args);
	}
	catch(IOException e)
	{
		System.err.println("FtsTrackEditorObject: I/O Error sending makeEvent Message!");
		e.printStackTrace(); 
	}   
}

public void requestEventMove(TrackEvent evt, double newTime)
{
	args.clear();
	args.addObject( evt);
	args.addDouble(newTime);
	
	try{
		send( FtsSymbol.get("moveEvents"), args);
	}
	catch(IOException e)
	{
		System.err.println("FtsTrackEditorObject: I/O Error sending moveEvents Message!");
		e.printStackTrace(); 
	}   
}

public void requestEventsMove(Enumeration events, int deltaX, Adapter a)
{
	TrackEvent aEvent;
	
	args.clear();
	for (Enumeration e = events; e.hasMoreElements();) 
	{	  
		aEvent = (TrackEvent) e.nextElement();		    
		args.addObject( aEvent);
		args.addDouble((double)a.getInvX(a.getX(aEvent)+deltaX));
	}
	
	try{
		send( FtsSymbol.get("moveEvents"), args);
	}
	catch(IOException e)
	{
		System.err.println("FtsTrackEditorObject: I/O Error sending moveEvents Message!");
		e.printStackTrace(); 
	}   
}

public void deleteEvent(TrackEvent event)
{
	args.clear();
	args.add(event);
	
	try{
		send( FtsSymbol.get("removeEvents"), args);
	}
	catch(IOException e)
	{
		System.err.println("FtsTrackObject: I/O Error sending removeEvents Message!");
		e.printStackTrace(); 
	}  
}

public void deleteEvents(Enumeration events)
{
	args.clear();
	for (Enumeration e = events; e.hasMoreElements();) 
		args.add((TrackEvent) e.nextElement());
	
	try{
		send( FtsSymbol.get("removeEvents"), args);
	}
	catch(IOException e)
	{
		System.err.println("FtsTrackObject: I/O Error sending removeEvents Message!");
		e.printStackTrace(); 
	}  
}

//////////////////////////////////////////////////////////////////////////////////	

public int wx, wy, ww, wh, transp, view, rangeMode;
public String label;
public float zoom;
FtsTrackObject trackObj;
protected transient FtsArgs args = new FtsArgs();
}
