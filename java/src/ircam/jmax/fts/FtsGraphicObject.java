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

package ircam.jmax.fts;

import ircam.ftsclient.*;
import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;

import javax.swing.undo.*;
import javax.swing.event.*;
import javax.swing.*;
import java.awt.*;
import java.util.*;
import java.io.*;

/**
 * An fts remote data that offers a built-in undo support.
 * 
 */
public class FtsGraphicObject extends FtsObject {
    
  static
  {
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setNumberOfInlets"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setNumberOfInlets(argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setNumberOfOutlets"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setNumberOfOutlets(argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setError"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setError(argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setErrorDescription"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).errorDescription = argv[0].stringValue; 
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setX"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentX((float)argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setY"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentY((float)argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setWidth"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentWidth((float)argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setHeight"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentHeight((float)argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setFont"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentFont(argv[0].stringValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setFontSize"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentFontSize(argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setFontStyle"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentFontStyle(argv[0].intValue);
	  }
	});
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setLayer"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentLayer(argv[0].intValue);
	  }
        });
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setComment"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).setCurrentComment(argv[0].stringValue);
	  }
        });
      FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("openFileDialog"), new FtsMessageHandler(){
	  public void invoke( FtsObject obj, int argc, FtsAtom[] argv)
	  {
	      ((FtsGraphicObject)obj).openFileDialog(argc, argv);
	  }
        });
  }
  
    protected FtsArgs args = new FtsArgs();

    public FtsGraphicObject(FtsServer server, FtsObject parent, FtsSymbol ftsClassName, int nArgs, FtsAtom args[], int id)
    {
	super(server, parent, id);

	this.ftsClassName = ftsClassName.toString();

	if(nArgs==0) 
	    description = this.ftsClassName;
	else 
	    description = this.ftsClassName + " " + FtsParse.unparseArguments(nArgs, args);
    }

    public FtsGraphicObject(FtsServer server, FtsObject parent, FtsSymbol ftsClassName, FtsArgs args) throws IOException
    {
	super(server, parent, ftsClassName, args);
	this.ftsClassName = ftsClassName.toString();
	this.description = this.ftsClassName;
    }

    public FtsGraphicObject(FtsServer server, FtsObject parent, FtsSymbol ftsClassName) throws IOException
    {
	super(server, parent, ftsClassName);
	this.ftsClassName = ftsClassName.toString();
	this.description = this.ftsClassName;
    }

    /****************************************************************************/
    /*                          graphic properties                              */
    /****************************************************************************/

    protected float x = (float)-1; 
    protected float y = (float)-1 ;
    protected float width = (float)-1;
    protected float height = (float)-1;
    protected boolean isError = false;
    protected String  errorDescription;
    protected String font = null;
    protected int fontSize = -1;
    protected int fontStyle = -1;
    protected int layer = -1;
    protected String comment = "";
    protected int noutlets = -1;
    protected int ninlets = -1;

    /** Get the X property */

    public final float getX()
    {
	return x;
    }
    public final void setX(float x)
    {
	if (this.x != x)
	    {
		args.clear();
		args.add((int)x);
      
		try{
		    send( FtsSymbol.get("setX"), args);
		}
		catch(IOException e)
		    {
			System.err.println("FtsGraphicObject: I/O Error sending setX Message!");
			e.printStackTrace(); 
		    }
		this.x = x;
		setDirty();
	    }
  }

  public final void setCurrentX(float x)
  {
      this.x = x;
  }

  /** Get the Y property */

    public final float getY()
    {
	return y;
    }

  /** Set the Y property. Tell it to the server, too. */

    public final void setY(float y)
    {
	if (this.y != y)
	    {
		args.clear();
		args.add((int)y);
      
		try{
		    send( FtsSymbol.get("setY"), args);
		}
		catch(IOException e)
		    {
			System.err.println("FtsGraphicObject: I/O Error sending setY Message!");
			e.printStackTrace(); 
		    }
		this.y = y;
		setDirty();
	    }
    }

    public final void setCurrentY(float y)
    {
	this.y = y;
    }

    /** Get the Width property */

    public final float getWidth()
    {
	return width;
    }

  public final void setWidth(float w)
  {
    if (this.width != w)
      {
	args.clear();
	args.add((int)w);
      
	try{
	    send( FtsSymbol.get("setWidth"), args);
	}
	catch(IOException e)
	    {
		System.err.println("FtsGraphicObject: I/O Error sending setWidth Message!");
		e.printStackTrace(); 
	    }  
	this.width = w;
	setDirty();
      }
  }

    public final void setCurrentWidth(float w)
    {
	this.width = w;
    }

    /** Get the Height property */

    public final float getHeight()
    {
	return height;
    }


  /** Set the Height property. Tell it to the server, too. */

    public final void setHeight(float h)
    {
	if (this.height != h)
	    {
		args.clear();
		args.add((int)h);
      
		try{
		    send( FtsSymbol.get("setHeight"), args);
		}
		catch(IOException e)
		    {
			System.err.println("FtsGraphicObject: I/O Error sending setHeight Message!");
			e.printStackTrace(); 
		    }  
		this.height = h;
		setDirty();
	    }
    }

    public final void setCurrentHeight(float h)
    {
	this.height = h;
    }

  public void setDefaults(){}

  /** Get the Error property. Error is a read only property. */

  public final boolean isError()
  {
    return isError;
  }

  public final void setError(int err)
  {
      if (err == 0)
	  isError = false;
      else
	  {
	      isError = true;
	      /* WARNING: what whit this???? */
	      //parent.addErrorObject(this);
	  }
      
      if (listener instanceof FtsObjectErrorListener)
	  ((FtsObjectErrorListener)listener).errorChanged(isError);	
  }

  /** Get the error description property. Error description is a read only property. */

  public final String getErrorDescription()
  {
    return errorDescription;
  }

  /** Get the font property */

  public final String getFont()
  {
    return font;
  }

  /** Set the font property. Tell it to the server, too. */

  public final void setFont(String font)
  {
    if ((this.font == null) || (! this.font.equals(font)))
      {
	  args.clear();
	  args.add(font);
	  
	  try{
	      send( FtsSymbol.get("setFont"), args);
	  }
	  catch(IOException e)
	      {
		  System.err.println("FtsGraphicObject: I/O Error sending setFont Message!");
		  e.printStackTrace(); 
	      }  	
	this.font = font;
	setDirty();
      }
  }
  public final void setCurrentFont(String font)
  {
      this.font = font;

      if(PatcherFontManager.getInstance().isToRecoverFont(font))
	  font = PatcherFontManager.getInstance().getRecoveringFont();
  }
  /** Get the font size property */

  public final int getFontSize()
  {
    return fontSize;
  }

  /** Set the font size property. Tell it to the server, too. */

  public final void setFontSize(int fontSize)
  {
    if (this.fontSize != fontSize)
      {
	  args.clear();
	  args.add(fontSize);
	  
	  try{
	      send( FtsSymbol.get("setFontSize"), args);
	  }
	  catch(IOException e)
	      {
		  System.err.println("FtsGraphicObject: I/O Error sending setFontSize Message!");
		  e.printStackTrace(); 
	      }  	
	this.fontSize = fontSize;
	setDirty();
      }
  }

    public final void setCurrentFontSize(int fs)
    {
	this.fontSize = fs;
    }

 /** Get the font style property */

  public final int getFontStyle()
  {
    return fontStyle;
  }

  /** Set the font size property. Tell it to the server, too. */

  public final void setFontStyle(int fontStyle)
  {
    if (this.fontStyle != fontStyle)
      {
	args.clear();
	args.add(fontStyle);
	  
	try{
	    send( FtsSymbol.get("setFontStyle"), args);
	}
	catch(IOException e)
	    {
		System.err.println("FtsGraphicObject: I/O Error sending setFontStyle Message!");
		e.printStackTrace(); 
	    }  
	  //fts.getServer().putObjectProperty(this, "fst", fontStyle);
	this.fontStyle = fontStyle;
	setDirty();
      }
  }

    public final void setCurrentFontStyle(int fs)
    {
	this.fontStyle = fs;
    }
  /** Get the layer  property */

  public final int getLayer()
  {
    return layer;
  }

  /** Set the layer property. Tell it to the server, too. */

  public final void setLayer(int layer)
  {
    if (this.layer != layer)
      {
	args.clear();
	args.add(layer);
	  
	try{
	    send( FtsSymbol.get("setLayer"), args);
	}
	catch(IOException e)
	    {
		System.err.println("FtsGraphicObject: I/O Error sending setLayer Message!");
		e.printStackTrace(); 
	    }  
	this.layer = layer;
	setDirty();
      }
  }

    public final void setCurrentLayer(int l)
    {
	this.layer = l;
    }
  /** Set the color property. Tell it to the server.
      Colors are not locally stored, can only be set, and they are meaningfull only
      for some object
  */

  public final void setColor(int color)
  {
      args.clear();
      args.add(color);
	  
      try{
	  send( FtsSymbol.get("setColor"), args);
      }
      catch(IOException e)
	  {
	      System.err.println("FtsGraphicObject: I/O Error sending setColor Message!");
	      e.printStackTrace(); 
	  }  
      setDirty();
  }

  public void setNumberOfInlets(int ins)
  {
      ninlets = ins;

      if ((getParent() != null) && (getParent() instanceof FtsPatcherObject))
	  ((FtsPatcherObject) getParent()).firePatcherChangedNumberOfInlets(ninlets);

      if (listener instanceof FtsInletsListener)
	  ((FtsInletsListener)listener).inletsChanged(ninlets);
  }
  public void setNumberOfOutlets(int outs)
  {
      noutlets = outs;

      if ((getParent() != null) && (getParent() instanceof FtsPatcherObject))
	  ((FtsPatcherObject) getParent()).firePatcherChangedNumberOfOutlets(noutlets);

      if (listener instanceof FtsOutletsListener)
	  ((FtsOutletsListener)listener).outletsChanged(noutlets);
  }

  public int getNumberOfInlets()
  {
      return ninlets;
  }
  public int getNumberOfOutlets()
  {
      return noutlets;
  }

  public String getComment()
  {
    return comment;
  }
  public void setComment(String comment)
  {
    args.clear();
    args.add(comment);
	  
    try{
	send( FtsSymbol.get("setComment"), args);
    }
    catch(IOException e)
	{
	    System.err.println("FtsGraphicObject: I/O Error sending setComment Message!");
	    e.printStackTrace(); 
	} 
    this.comment = comment;
  }

  public void setCurrentComment(String comment)
  {
    this.comment = comment;
  }
  /********************************************************************************/
    
    protected transient Object listener;

    /** Set the unique object listener */

    public void setObjectListener(Object obj)
    {
	listener = obj;
    }

    /** Get the current object listener */
    
    public Object getObjectListener()
    {
	return listener;
    }

    protected FtsGraphicListener graphicListener = null;
    public void setGraphicListener(FtsGraphicListener l)
    {
	graphicListener = l;
    }
    public FtsGraphicListener getGraphicListener()
    {
	return graphicListener;
    }  

  /********************************************************************************/
    public String getClassName()
    {
	return ftsClassName;
    } 
    
    /* WARNING: to implement when document stuff implemented */
    public void setDirty()
    {
	//nothing for now!!!!
    }

    /* By default is className. Redefined if needed */
    public String getDescription()
    {
	return description;
    }
    public void setDescription(String descr)
    {
	description = descr;
    }

    public boolean isARootPatcher()
    {
	return (getParent() == getServer().getRoot());
    }

    public Enumeration getGenealogy()
    {
	Vector gen = new Vector();
	FtsObject current = this;
	gen.addElement(current);
      
	if(!isARootPatcher())
	    while(!current.getParent().isARootPatcher())
	      {
		  gen.add(0, current.getParent());
		  current = current.getParent();
	      }
	if(current.getParent().isARootPatcher())
	    gen.add(0, current.getParent());
	
	return gen.elements();
    }

    String ftsClassName;
    String description;

    /*****************************************************************/
 //final variables used by invokeLater method
    private transient JFileChooser fd;
    private transient Frame parentFrame;
    private transient String dialogText;
    private transient String callbackMethod;
  
    public void openFileDialog(int nArgs, FtsAtom a[])
    {
	parentFrame = null;
	callbackMethod = a[0].stringValue;
	dialogText = a[1].stringValue;
	String defaultPath = a[2].stringValue;
	String defaultName = a[3].stringValue;

	fd = new JFileChooser(defaultPath);
	fd.setDialogTitle(dialogText);
	fd.setSelectedFile(new File(defaultPath, defaultName));
      
	if(this instanceof FtsObjectWithEditor)
	    parentFrame = ((FtsObjectWithEditor)this).getEditorFrame();

	FtsObject current = this;
	while(((parentFrame==null)||(!parentFrame.isVisible()))&&(!isARootPatcher()))
	    {
		current = current.getParent();
		if(current instanceof FtsObjectWithEditor)
		    parentFrame = ((FtsObjectWithEditor)current).getEditorFrame();
	    }

	if(parentFrame!=null)
	    {
		/*
		  NOTE: we used invokeLater because the fileDialog is modal so when we show it via a
		  message object (export ...), we lose a mouseup event on the message object leaving 
		  the interactionEngine in an incorrect state (RunCtrlInteraction instead of RunModeInteraction)
		  So invokeLater allow to consume the mouseUp event before fileDialog is shown
		*/
		SwingUtilities.invokeLater(new Runnable() {
			public void run()
			{ 
			    if (fd.showDialog(parentFrame, dialogText) == JFileChooser.APPROVE_OPTION)
				{
				    String path = fd.getSelectedFile().getAbsolutePath();
				    args.clear();
				    args.add(path);
      
				    try{
					send( FtsSymbol.get(callbackMethod), args);
				    }
				    catch(IOException e)
					{
					    System.err.println("FtsGraphicObject: I/O Error sending"+callbackMethod+" Message!");
					    e.printStackTrace(); 
					}
				} 
			}
		    });
	    }
    }
}




