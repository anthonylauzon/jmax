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

public class FtsMidiConfig extends FtsObject
{
  protected FtsArgs args = new FtsArgs();

  static
  {
    FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("inputs"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMidiConfig)obj).setSources( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("outputs"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMidiConfig)obj).setDestinations( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("insert"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMidiConfig)obj).insertLabel( args.getInt( 0), args.getSymbol( 1).toString());

          if(args.getLength() > 2)
            ((FtsMidiConfig)obj).setInput( args.getInt( 0), args.getSymbol( 2).toString());
            
          if(args.getLength() > 3)
            ((FtsMidiConfig)obj).setOutput( args.getInt( 0), args.getSymbol( 3).toString());            
	}
      });
    FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("set"), new FtsMessageHandler(){
      public void invoke( FtsObject obj, FtsArgs args)
      {
        ((FtsMidiConfig)obj).set( args.getInt( 0), args.getSymbol( 1).toString(), args.getSymbol( 2).toString(), args.getSymbol( 3).toString());
      }
    });    
     FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("remove"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsMidiConfig)obj).removeLabel( args.getInt( 0));
	}
      });
     FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("input"), new FtsMessageHandler(){
	 public void invoke( FtsObject obj, FtsArgs args)
	 {
	   ((FtsMidiConfig)obj).setInput( args.getInt( 0), args.getSymbol( 1).toString());
	 }
       });
     FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("output"), new FtsMessageHandler(){
	 public void invoke( FtsObject obj, FtsArgs args)
	 {
	   ((FtsMidiConfig)obj).setOutput( args.getInt( 0), args.getSymbol( 1).toString());
	 }
       });
     FtsObject.registerMessageHandler( FtsMidiConfig.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
	 public void invoke( FtsObject obj, FtsArgs args)
	 {
	   ((FtsMidiConfig)obj).clear();
	 }
       });
  }
    
  public FtsMidiConfig(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);
    labels = new Vector();
  }

  public FtsMidiConfig() throws IOException
  { 
    super(JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("slider"));
  }

  public void setListener( ircam.jmax.editors.configuration.MidiConfigPanel listener)
  {
    this.listener = listener;
  }

  public void requestInsertLabel( int index, String label)
  {
    labels.insertElementAt( new MidiLabel( label, null, null), index);
    
    args.clear();
    
    args.addInt( index);
    args.addSymbol( FtsSymbol.get( label));

    try
      {
	send( FtsSymbol.get( "insert"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsMidiConfig: I/O Error sending insert Message!");
	e.printStackTrace(); 
      }
  }

  public void requestRemoveLabel( int index)
  {
    args.clear();
    
    args.addInt( index);

    try
      {
	send( FtsSymbol.get( "remove"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsMidiConfig: I/O Error sending remove Message!");
	e.printStackTrace(); 
      }
  }

  public void requestSetInput( int index, String input)
  {
    args.clear();
    
    args.addInt( index);
    args.addSymbol( FtsSymbol.get( input));

    try
      {
	send( FtsSymbol.get( "input"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsMidiConfig: I/O Error sending input Message!");
	e.printStackTrace(); 
      }
  }

  public void requestSetOutput( int index, String output)
  {
    args.clear();
    
    args.addInt( index);
    args.addSymbol( FtsSymbol.get( output));
    
    try
      {
	send( FtsSymbol.get( "output"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsMidiConfig: I/O Error sending output Message!");
	e.printStackTrace(); 
      }
  }

  void setSources(int nArgs, FtsAtom[] args)
  {
    sources = new String[ nArgs];
    for(int i = 0; i < nArgs; i++)
      sources[i] = args[i].symbolValue.toString();
  
    if(listener != null)
      listener.sourcesChanged();
  }

  public String[] getSources()
  {
    return sources;
  }
  void setDestinations(int nArgs, FtsAtom[] args)
  {
    destinations = new String[ nArgs];
    for(int i = 0; i < nArgs; i++)
      destinations[i] = args[i].symbolValue.toString();
  
    if(listener != null)
      listener.destinationsChanged();
  }
  public String[] getDestinations()
  {
    return destinations;
  }

  void insertLabel( int index, String label)
  {
    labels.insertElementAt( new MidiLabel( label, null, null), index);
  }

  void removeLabel( int index)
  {
    if( index >= 0 && index < labels.size())
        labels.remove( index);	
  }

  public Enumeration getLabels()
  {
    return labels.elements();
  }

  void set( int index, String name, String input, String output)
  {
    ((MidiLabel)labels.elementAt( index)).set( name, input, output);
    if(listener != null)
      listener.labelChanged( index, name, input, output);
  }
  
  void setInput( int index, String input)
  {
    ((MidiLabel)labels.elementAt( index)).setInput( input);
    if(listener != null)
      listener.labelChanged( index, null, input, null);
  }

  void setOutput( int index, String output)
  {
    ((MidiLabel)labels.elementAt( index)).setOutput( output);
    if(listener != null)
      listener.labelChanged( index, null, null, output);
  }

  public void clear()
  {
    labels.removeAllElements();
  }

  public class MidiLabel extends Object
  {
    public MidiLabel( String label, String input, String output)
    {
      this.label = label;
      this.input = input;
      this.output = output;
    }
    public void set( String name, String in, String out){label = name; input = in; output = out;}
    public void setInput( String in){ input = in;}
    public void setOutput( String out){ output = out;}
    public String label, input, output;
  }

  private String[] sources;
  private String[] destinations;
  private Vector labels; 
  private ircam.jmax.editors.configuration.MidiConfigPanel listener;
}

