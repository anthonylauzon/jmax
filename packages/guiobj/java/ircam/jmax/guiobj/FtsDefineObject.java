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
//

package ircam.jmax.guiobj;

import java.io.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

public class FtsDefineObject extends FtsGraphicObject
{
  static
  {
    FtsObject.registerMessageHandler( FtsDefineObject.class, FtsSymbol.get("type"), new FtsMessageHandler(){
	  public void invoke(FtsObject obj, FtsArgs args)
	  {
	    ((FtsDefineObject)obj).setType( args.getSymbol(0).toString());
	  }
	});
      FtsObject.registerMessageHandler( FtsDefineObject.class, FtsSymbol.get("expression"), new FtsMessageHandler(){
	  public void invoke(FtsObject obj, FtsArgs args)
	  {
	    ((FtsDefineObject)obj).setExpression( args.getString(0));
	  }
	});
     FtsObject.registerMessageHandler( FtsDefineObject.class, FtsSymbol.get("valid"), new FtsMessageHandler(){
	  public void invoke(FtsObject obj, FtsArgs args)
	  {
	    ((FtsDefineObject)obj).setValid( (args.getInt(0) != 0));
	  }
	});
    }

  public FtsDefineObject(FtsServer server, FtsObject parent, int id, String className, FtsAtom[] args, int offset, int length)
  {
    super(server, parent, id, className, args[offset].stringValue);

    ninlets = 0;
    noutlets = 0;
    
    type = "const";
    expression = "";
  }

  public int getNumberOfInlets()
  {
    return 0;
  }
  public int getNumberOfOutlets()
  {
    return 0;
  }
  
  public void requestSetExpression( String expression)
  {
    this.expression = expression;

    args.clear();
    args.addRawString( expression);
    
    try
      {
	send( FtsSymbol.get("expression"), args);
      }
    catch( IOException e)
      {
	System.err.println("FtsDefineObject: I/O Error sending expression Message!");
	e.printStackTrace(); 
      }
  }
  
  public void requestSetType( String type)
  {
    this.type = type;

    args.clear();
    args.addSymbol( FtsSymbol.get( type));
    
    try
      {
	send( FtsSymbol.get("type"), args);
      }
    catch( IOException e)
      {
	System.err.println("FtsDefineObject: I/O Error sending setType Message!");
	e.printStackTrace(); 
      }
  }
  
  public String getExpression()
  {
    return expression;
  }
  
  public void setExpression( String expression)
  {  
    this.expression = expression;
    ((Define)listener).expressionChanged( expression);	
  }
  
  public String getType()
  {
    return type;
  }
  
  public void setType( String type)
  {
    this.type = type;
    ((Define)listener).typeChanged( type);
  }
  
   public boolean isValid()
  {
    return valid;
  }
  
  public void setValid( boolean valid)
  {
    this.valid = valid;
    ((Define)listener).validChanged( valid);
  }

  
  private String expression;
  private String type;
  private boolean valid = true;
}






