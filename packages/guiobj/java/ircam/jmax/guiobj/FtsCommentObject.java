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

package ircam.jmax.guiobj;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

/**
 * The proxy of an FTS comment object.
 * The comment is stored in the comment property.
 */

public class FtsCommentObject extends FtsGraphicObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsCommentObject.class, FtsSymbol.get("color"), new FtsMessageHandler(){
	public void invoke(FtsObject obj, FtsArgs args)
	{
	  ((FtsCommentObject)obj).setCurrentColor( args.getInt(0));
	}
      });
  }

  public FtsCommentObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom args[], int offset, int length)
  {
    super(server, parent, id, className, "");
    setNumberOfInlets(0);
    setNumberOfOutlets(0);
  }

  public void setCurrentComment( String comment)
  {
    super.setCurrentComment( comment);
    ((Comment)getObjectListener()).computeRenderer();
    ((Comment)getObjectListener()).renderer.update();
    ((Comment)getObjectListener()).redraw();
  }

  public final void setColor(int color)
  {
    args.clear();
    args.addInt(color);
    
    try{
      send( FtsSymbol.get( "color"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsCommentObject: I/O Error sending setColor Message!");
	e.printStackTrace(); 
      }  
  }
  
  public void setCurrentColor(int color)
  {
    this.color = color;
    (( Comment)getObjectListener()).setCurrentColor( color);
  }

  public int getColor()
  {
    return color;
  }
  
  protected transient FtsArgs args = new FtsArgs();
  int color;
}






