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

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

// import javax.swing.*;
import javax.swing.JFileChooser;
import javax.swing.SwingUtilities;

import java.awt.*;
import java.util.*;
import java.io.*;

public class FtsGraphicObject extends FtsObject {
    
  static
  {
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "n_inlets"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setNumberOfInlets( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "n_outlets"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setNumberOfOutlets( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get("setError"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setError( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setErrorDescription"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setErrorDescription(args.getSymbol( 0).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setX"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentX( (float)args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setY"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentY( (float)args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setWidth"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentWidth( (float)args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setHeight"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentHeight( (float)args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setFont"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentFont( args.getSymbol( 0).toString(), args.getInt( 1), args.getInt( 2));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setFontName"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentFontName( args.getSymbol( 0).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setFontSize"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentFontSize( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setFontStyle"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentFontStyle( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setLayer"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentLayer( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "set_name"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setVariableName( args.getSymbol( 0).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "setComment"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).setCurrentComment( args.getSymbol( 0).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsGraphicObject.class, FtsSymbol.get( "openFileDialog"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsGraphicObject)obj).openFileDialog( args.getLength(), args.getAtoms());
	}
      });
  }
  
  protected transient FtsArgs args = new FtsArgs();
  
  public FtsGraphicObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
    this( server, parent, id, className, args[offset].stringValue);
  }

  public FtsGraphicObject(FtsServer server, FtsObject parent, int id, String className, String description)
  {
    super(server, parent, id);
    this.description = description;
    this.className = className;
  }

  public FtsGraphicObject(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);
  }

  public FtsGraphicObject(FtsServer server, FtsObject parent, FtsSymbol className) throws IOException
  {
    super(server, parent, className);
    this.description = className.toString();
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
  protected String varName = null;

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
	args.addSymbol(FtsSymbol.get("x"));
	args.addInt((int)x);

	try{
	  sendProperty(args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending setX Message!");
	    e.printStackTrace(); 
	  }

	this.x = x;
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
	args.addSymbol(FtsSymbol.get("y"));
	args.addInt((int)y);

	try{
	  sendProperty( args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending setY Message!");
	    e.printStackTrace(); 
	  }
	
	this.y = y;
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
	args.addSymbol(FtsSymbol.get("w"));
	args.addInt((int)w);

	try{
	  sendProperty( args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending setWidth Message!");
	    e.printStackTrace(); 
	  }

	this.width = w;
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
	args.addSymbol(FtsSymbol.get("h"));
	args.addInt((int)h);

	try{
	  sendProperty( args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending setHeight Message!");
	    e.printStackTrace(); 
	  }
	this.height = h;
      }
  }

  public final void setCurrentHeight(float h)
  {
    this.height = h;
  }

  public final void setCurrentBounds(float x, float y, float w, float h)
  {
    this.x      = x;
    this.y      = y;
    this.width  = w;
    this.height = h;
  } 
  
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
  
  public final void setErrorDescription(String ed)
  {
    errorDescription = ed;
  }

  /** Get the font property */

  public final String getFont()
  {
    return font;
  }

  /** Set the font property. Tell it to the server, too. */

  public final void setFont( String font)
  {
    if ((this.font == null) || (! this.font.equals(font)))
      {
	args.clear();
	args.addSymbol( FtsSymbol.get("font"));
	args.addSymbol( FtsSymbol.get(font));
	
	try{
	  sendProperty( args);
	}
	catch( IOException e)
	  {
	    System.err.println( "FtsGraphicObject: I/O Error sending setFont Message!");
	    e.printStackTrace(); 
	  }  	
	this.font = font;
      }
  }

  public final void setCurrentFont( String font, int size, int style)
  {
    ((GraphicObject)getObjectListener()).setCurrentFont( font, size, style);
  }

  public final void setCurrentFontName(String font)
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
	args.addSymbol( FtsSymbol.get("fs"));
	args.addInt(fontSize);
	  
	try{
	  sendProperty( args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending setFontSize Message!");
	    e.printStackTrace(); 
	  }  	
	this.fontSize = fontSize;
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
	args.addSymbol( FtsSymbol.get("fst"));
	args.addInt(fontStyle);
	  
	try{
	  sendProperty( args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending setFontStyle Message!");
	    e.printStackTrace(); 
	  }  
	this.fontStyle = fontStyle;
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

  public final void setLayer( int layer)
  {
    if (this.layer != layer)
      {
	args.clear();
	args.addSymbol( FtsSymbol.get("layer"));
	args.addInt( layer);
	  
	try{
	  sendProperty( args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending setLayer Message!");
	    e.printStackTrace(); 
	  }  
	this.layer = layer;
      }
  }

  public final void setCurrentLayer(int l)
  {
    this.layer = l;
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
  public void setComment( String comment)
  {
    args.clear();
    args.addSymbol( FtsSymbol.get("comment"));    
    args.addSymbol( FtsSymbol.get(comment));
	  
    try{
      sendProperty( args);
    }
    catch(IOException e)
      {
	System.err.println("FtsGraphicObject: I/O Error sending setComment Message!");
	e.printStackTrace(); 
      } 
    this.comment = comment;
  }

  public void setCurrentComment( String comment)
  {
    this.comment = comment;
  }

  public void requestSetName( String name)
  {
    if (this.varName != name)
      {
	args.clear();
	args.addSymbol( FtsSymbol.get( name));

	try{
	  send( FtsSymbol.get("set_name"), args);
	}
	catch(IOException e)
	  {
	    System.err.println("FtsGraphicObject: I/O Error sending set_name Message!");
	    e.printStackTrace(); 
	  }

	this.varName = name;
      }
  }

  public final void setVariableName( String name)
  {
    this.varName = name;
    ((GraphicObject)getObjectListener()).setCurrentName( name);
  }

  public final String getVariableName()
  {
    return varName;
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

  protected transient FtsGraphicListener graphicListener = null;
  public void setGraphicListener(FtsGraphicListener l)
  {
    graphicListener = l;
  }
  public FtsGraphicListener getGraphicListener()
  {
    return graphicListener;
  }  

  public String getClassName()
  {
    return className;
  }

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
    return (getParent() == JMaxApplication.getRootPatcher());
  }

  public FtsPatcherObject getRootPatcher()
  {
    FtsGraphicObject current = this;
    
    while(!current.isARootPatcher())	
      current = (FtsGraphicObject)current.getParent();
	
    return (FtsPatcherObject)current;
  }

  public Enumeration getGenealogy()
  {
    Vector gen = new Vector();
    FtsGraphicObject current = this;
    gen.addElement(current);
      
    if(!isARootPatcher())
      while(!((FtsGraphicObject)current.getParent()).isARootPatcher())
	{
	  gen.add(0, current.getParent());
	  current = (FtsGraphicObject) current.getParent();
	}
    if(((FtsGraphicObject)current.getParent()).isARootPatcher())
      gen.add(0, current.getParent());
	
    return gen.elements();
  }

  String description;
  String className;
  /*****************************************************************/
  //final variables used by invokeLater method
  private transient JFileChooser fd;
  private transient Frame parentFrame;
  private transient String dialogText;
  private transient String callbackMethod;
  
  public void openFileDialog(int nArgs, FtsAtom a[])
  {
    parentFrame = null;
    callbackMethod = a[0].symbolValue.toString();
    dialogText = a[1].symbolValue.toString();
    String defaultPath = a[2].symbolValue.toString();
    String defaultName = a[3].symbolValue.toString();

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
		  args.addSymbol(FtsSymbol.get( path));
      
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

