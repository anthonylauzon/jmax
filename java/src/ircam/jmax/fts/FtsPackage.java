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
import ircam.jmax.editors.project.*;

public class FtsPackage extends FtsObjectWithEditor
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
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("template"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).addTemplates( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("help"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).addHelps( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("setNames"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).setName( args.getSymbol( 0).toString());
	  ((FtsPackage)obj).setDir( args.getSymbol( 1).toString());
	  ((FtsPackage)obj).setFileName( args.getSymbol( 2).toString());
	}
      });
     FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("hasSummaryHelp"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).hasSummaryHelp = (args.getInt( 0) == 1);
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("updateDone"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  if(((FtsPackage)obj).listener != null)
	    ((FtsPackage)obj).listener.updateDone();
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("uploadDone"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	
{
	  ((FtsPackage)obj).uploadDone();
	}
      });
    FtsObject.registerMessageHandler( FtsPackage.class, FtsSymbol.get("setDirty"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsPackage)obj).setDirty( args.getInt( 0) == 1);
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
    templates = new Vector();
    helpPatches = new Vector();
  }

  public FtsPackage(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);

    requires = new Vector();
    templatePaths = new Vector();
    absPaths = new Vector();
    dataPaths = new Vector();
    templates = new Vector();
    helpPatches = new Vector();
  }

  public void setPackageListener(ircam.jmax.editors.project.ConfigPackagePanel listener)
  {
    this.listener = listener;
  }

  public void set(String message, Enumeration values)
  {
    String value;
    args.clear();

    for(; values.hasMoreElements();)
      {
	value =  (String)values.nextElement();
	if( value != null)
	  args.addSymbol( FtsSymbol.get( value));
      }
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
      {
	if( values[i] != null)
	  args.addSymbol( FtsSymbol.get(values[i]));
      }
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

  public void requestAddTemplate(String name, String fileName, int index)
  {
    args.clear();
    args.addSymbol( FtsSymbol.get(name));
    args.addSymbol( FtsSymbol.get(fileName));
    args.addInt( index);

    try
      {
	send( FtsSymbol.get("insert_template"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsPackage: I/O Error sending template Message!");
	e.printStackTrace(); 
      }
  }


  public void requestRemoveTemplate(String name, String fileName)
  {
    args.clear();
    args.addSymbol( FtsSymbol.get(name));
    args.addSymbol( FtsSymbol.get(fileName));
     
    try
      {
	send( FtsSymbol.get("remove_template"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsPackage: I/O Error sending remove_template Message!");
	e.printStackTrace(); 
      }
  }

  public void save( String name)
  {
    args.clear();
    args.addSymbol( FtsSymbol.get( (name != null) ? name : fileName));

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
    templates.removeAllElements();
    helpPatches.removeAllElements();

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

  public void upload()
  {
    try
      {
	send( FtsSymbol.get("upload"));
      }
    catch(IOException e)
      {
	System.err.println("FtsPackage: I/O Error sending upload Message!");
	e.printStackTrace(); 
      }
  }

  void addRequires(int nArgs, FtsAtom[] args)
  {
    requires.removeAllElements();

    for(int i = 0; i < nArgs; i += 2)
      addPackage(args[i].symbolValue.toString(), args[i+1].intValue);
  }

  void addPackage(String pkgName, int id)
  {
    requires.addElement(pkgName);
  }

  void addTemplatePath(int nArgs, FtsAtom[] args)
  {
    templatePaths.removeAllElements();

    for(int i = 0; i<nArgs; i++)
      templatePaths.addElement( args[i].symbolValue.toString());
  
    if( listener != null)
      listener.templatePathChanged();
  }
  
  void addAbstractionPath(int nArgs, FtsAtom[] args)
  {
    absPaths.removeAllElements();

    for(int i = 0; i<nArgs; i++)
      absPaths.addElement( args[i].symbolValue.toString());
  }

  void addDataPath(int nArgs, FtsAtom[] args)
  {
    dataPaths.removeAllElements();

    for(int i = 0; i<nArgs; i++)
      dataPaths.addElement( args[i].symbolValue.toString());

    if( listener != null)
      listener.dataPathChanged();
  }

  void addTemplates(int nArgs, FtsAtom[] args)
  {
    templates.removeAllElements();

    for(int i = 0; i < nArgs; i+=2)
      templates.addElement( new TwoNames( args[i].symbolValue.toString(),  
					  args[i+1].symbolValue.toString()));

    if( listener != null)
      listener.templateChanged();
  }

  void addHelps(int nArgs, FtsAtom[] args)
  {
    helpPatches.removeAllElements();

    for(int i = 0; i < nArgs; i+=2)
      helpPatches.addElement( new TwoNames( args[i].symbolValue.toString(),  
					    args[i+1].symbolValue.toString()));

    /*if( listener != null)
      listener.helpChanged();*/
  }

  public void setName( String name)
  {
    this.name = name;    
  }

  public String getName()
  {
    return name;
  }

  public void setFileName( String fn)
  {
    this.fileName = fn;
  }

  public String getFileName()
  {
    return fileName;
  }

  public void setDir( String dir)
  {
    this.dir = dir;
  }

  public String getDir()
  {
    return dir;
  }

  void uploadDone()
  {    
    if( hasSummaryHelp)
      FtsHelpPatchTable.addSummary( name.toString(), dir + "/help/" + name + ".summary.jmax");
  }

  public void openEditor(int nArgs, FtsAtom[] args)
  {
    if(getEditorFrame() == null)
      setEditorFrame( new ProjectEditor(this));
    
    showEditor();
  }
  public void destroyEditor()
  {
    disposeEditor();
  }

  public void setDirty( boolean d)
  {
    isDirty = d;
  }
  public boolean isDirty()
  {
    return isDirty;
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

  public Enumeration getTemplates()
  {
    return templates.elements();
  }

  public Enumeration getHelps()
  {
    return helpPatches.elements();
  }

  public class TwoNames extends Object
  {
    public TwoNames( String name, String fileName)
    {
      this.name = name;
      this.fileName = fileName;
    }
    public void set( String n, String fn){ name = n; fileName = fn;}
    public String name, fileName;
  }

  private Vector requires, templatePaths, absPaths, dataPaths, templates, helpPatches;
  private String name;
  private String fileName;
  private String dir;
  ircam.jmax.editors.project.ConfigPackagePanel listener;
  private boolean hasSummaryHelp = true;
  private boolean isDirty = false;
}


