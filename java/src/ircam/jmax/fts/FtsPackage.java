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

package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.fts.client.*;
import ircam.jmax.*;

public class FtsPackage extends FtsObject
{
  protected FtsArgs args = new FtsArgs();

  static
  {
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("require"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).addRequires( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("template_path"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).addTemplatePath( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("abstraction_path"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).addAbstractionPath( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("data_path"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).addDataPath( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("setName"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).setName( args.getSymbol( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("updateDone"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).listener.ftsActionDone();
	}
      });
  }
    
  public FtsPackage() throws IOException
  {
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("package"));

    requires = new Vector();
    templatePaths = new Vector();
    absPaths = new Vector();
    dataPaths = new Vector();
  }

  public FtsPackage(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);

    requires = new Vector();
    templatePaths = new Vector();
    absPaths = new Vector();
    dataPaths = new Vector();
  }

  public void setFtsActionListener(FtsActionListener listener)
  {
    this.listener = listener;
  }

  public void set(String message, Enumeration values)
  {
    args.clear();

    for(; values.hasMoreElements();)
      args.addSymbol( FtsSymbol.get( (String)values.nextElement()));
      
    try
      {
	send( FtsSymbol.get(message), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsPackage: I/O Error sending "+message+" Message!");
	e.printStackTrace(); 
      }
  }

  public void set(String message, String[] values)
  {
    args.clear();

    for(int i = 0; i< values.length; i++)
      args.addSymbol( FtsSymbol.get(values[i]));
     
    try
      {
	send( FtsSymbol.get(message), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsPackage: I/O Error sending "+message+" Message!");
	e.printStackTrace(); 
      }
  }

  public void save()
  {
    args.clear();
    args.addSymbol( name);

    try
      {
	send( FtsSymbol.get("save"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsPackage: I/O Error sending save Message!");
	e.printStackTrace(); 
      }
  }

  public void update()
  {
    requires.removeAllElements();
    templatePaths.removeAllElements();
    dataPaths.removeAllElements();

    try
      {
	send( FtsSymbol.get("update"));
      }
    catch(IOException e)
      {
	System.err.println("FtsPackage: I/O Error sending update Message!");
	e.printStackTrace(); 
      }
  }

  void addRequires(int nArgs, FtsAtom[] args)
  {
    for(int i = 0; i < nArgs; i++)
      requires.addElement( args[i].symbolValue.toString());
  }

  void addTemplatePath(int nArgs, FtsAtom[] args)
  {
    for(int i = 0; i<nArgs; i++)
      templatePaths.addElement( args[i].symbolValue.toString());
  }

  void addAbstractionPath(int nArgs, FtsAtom[] args)
  {
    for(int i = 0; i<nArgs; i++)
      absPaths.addElement( args[i].symbolValue.toString());
  }

  void addDataPath(int nArgs, FtsAtom[] args)
  {
    for(int i = 0; i<nArgs; i++)
      dataPaths.addElement( args[i].symbolValue.toString());
  }


  public void setName( FtsSymbol name)
  {
    this.name = name;
  }

  public FtsSymbol getName()
  {
    return name;
  }

  /*************************************/

  public Enumeration getRequires()
  {
    return requires.elements();
  }

  public Enumeration getTemplatePaths()
  {
    return templatePaths.elements();
  }

  public Enumeration getAbstractionPaths()
  {
    return absPaths.elements();
  }

  public Enumeration getDataPaths()
  {
    return dataPaths.elements();
  }

  private Vector requires;
  private Vector templatePaths;
  private Vector absPaths;
  private Vector dataPaths;
  private FtsSymbol name;
  private FtsActionListener listener;
}

