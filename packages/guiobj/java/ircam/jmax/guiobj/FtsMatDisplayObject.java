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
// Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import java.io.*;
import java.util.*;
import java.text.*;
import java.awt.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

public class FtsMatDisplayObject extends FtsGraphicObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsMatDisplayObject.class, FtsSymbol.get("display"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMatDisplayObject)obj).display(args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsMatDisplayObject.class, FtsSymbol.get("scroll"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMatDisplayObject)obj).scroll(args.getLength(), args.getAtoms());
	}
      });

    FtsObject.registerMessageHandler( FtsMatDisplayObject.class, FtsSymbol.get("range"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMatDisplayObject)obj).setCurrentRange(args.getFloat(0), args.getFloat(1));
	}
      });

    FtsObject.registerMessageHandler( FtsMatDisplayObject.class, FtsSymbol.get("color"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMatDisplayObject)obj).setCurrentColor(args.getInt(0), args.getInt(1));
	}
      });

    FtsObject.registerMessageHandler( FtsMatDisplayObject.class, FtsSymbol.get("zoom"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMatDisplayObject)obj).setCurrentZoom(args.getInt(0), args.getInt(1));
	}
      });

    FtsObject.registerMessageHandler( FtsMatDisplayObject.class, FtsSymbol.get("window"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMatDisplayObject)obj).setCurrentWindowSize(args.getInt(0),args.getInt(1));
	}
      });
    FtsObject.registerMessageHandler( FtsMatDisplayObject.class, FtsSymbol.get("size"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMatDisplayObject)obj).setSize(args.getInt(0),args.getInt(1));
	}
      });
  }

  private static final float defaultMin = (float)0.0;
  private static final float defaultMax = (float)1.0;

  int mWindow = 130;
  int nWindow = 130;
  int mSize = 0;
  int nSize = 0;
  byte pixels[];
  int nZoom;
  int mZoom;
  int minColor; 
  int maxColor; 
  float min = defaultMin;
  float max = defaultMax;
  int scroll = 0;

  public FtsMatDisplayObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
    super(server, parent, id, className, args, offset, length);
    
    ninlets = 1;
    noutlets = 0;
  }

  public void display(int nArgs, FtsAtom args[])
  { 
    scroll = 0;

    for(int m=0; m<mSize; m++)
      {
	int offset = m * mZoom * nSize * nZoom;

	for(int n=0; n<nSize; n++)
	  {
	    int index = m * nSize + n;
	    int pix = offset + n * nZoom;
	    int step = nSize * nZoom;
	    
	    for(int j=0; j<mZoom; j++)
	      for(int i=0; i<nZoom; i++)
		pixels[pix + j * step + i] = (byte)args[index].intValue;
	  }
      }

    ((FtsDisplayListener) listener).display();
  }
  
  public void scroll(int nArgs, FtsAtom args[])
  {
    scroll -= 1;

    if(scroll < 0)
      scroll += nSize;

    for(int m=0; m<mSize; m++)
      {
	int offset = m * mZoom * nSize * nZoom;
	int step = nSize * nZoom;	

	for(int j=0; j<mZoom;j++)
	  {
	    for(int i=0; i<nZoom; i++)
	      {
		pixels[offset + j * step + scroll * nZoom + i] = (byte)args[m].intValue;
	      }
	  }
      }

    ((MatDisplay)listener).display();
  }

  public void setSize(int m, int n)
  {
    mSize = m;
    nSize = n;
  }

  public void setCurrentWindowSize(int m, int n)
  {
    if(m != mWindow || n != nWindow)
      {
	mWindow = m;
	nWindow = n;

	pixels = new byte[mWindow * nWindow];
	scroll = 0;
      }
  }

  public void setWindowSize(int m, int n)
  {
    args.clear();
    args.addInt(m);
    args.addInt(n);

    try
      {
	send( FtsSymbol.get("window"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsMatDisplayObject: I/O Error sending window Message!");
	e.printStackTrace(); 
      }
  }  

 public void setCurrentZoom(int mZ, int nZ)
  {
    mZoom = mZ;
    nZoom = nZ;
    scroll = 0;    

    if(listener != null)
      {
	((FtsDisplayListener) listener).display();
      }
  }
 
  public void setZoom(int m, int n)
  {
    args.clear();
    args.addInt(m);
    args.addInt(n);
    
    try{
      send( FtsSymbol.get("zoom"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsMatDisplayObject: I/O Error sending zoom Message!");
	e.printStackTrace(); 
      }
  }

  public int getNZoom()
  {
    return nZoom;
  }  

  public int getMZoom()
  {
    return mZoom;
  } 

  public void setCurrentRange(float min, float max)
  {
    this.min = min;
    this.max = max;

    if(listener != null)
      {
	((FtsDisplayListener) listener).display();
      }
  }
  
  public void setRange(float min, float max)
  {
    args.clear();
    args.addFloat(min);
    args.addFloat(max);

    try{
      send( FtsSymbol.get("range"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsMatDisplayObject: I/O Error sending range Message!");
	e.printStackTrace(); 
      }
  }

  public float getMinimum()
  {
    return min;
  }
  public float getMaximum()
  {
    return max;
  }
  
  public void setCurrentColor(int min, int max)
  {    
    minColor = min; 
    maxColor = max; 

    if(listener != null)
      {
	((MatDisplay) listener).setColor( new Color(min), new Color(max));
	((FtsDisplayListener) listener).display();
      }
  }
  
  public void setColor(int min, int max)
  {
    args.clear();
    args.addInt(min);
    args.addInt(max);
    
    try{
      send( FtsSymbol.get("color"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsMatDisplayObject: I/O Error sending color Message!");
	e.printStackTrace(); 
      }
  }

  public void setMinColor(int c)
  {
    minColor = c; 
    setColor(minColor, maxColor);
  } 

  public void setMaxColor(int c)
  {
    maxColor = c; 
    setColor(minColor, maxColor);
  } 

  public int getMinColor()
  {
    return minColor;
  } 

  public int getMaxColor()
  {
    return maxColor;
  } 

  public int getScaledScroll()
  {
    return (scroll * nZoom);
  }
}



