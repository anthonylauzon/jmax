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
import ircam.jmax.editors.configuration.*;

public class FtsConfig extends FtsObjectWithEditor
{
  protected FtsArgs args = new FtsArgs();

  static
  {
    FtsObject.registerMessageHandler( FtsConfig.class, FtsSymbol.get("midi_config"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsConfig)obj).setMidiConfig( new FtsMidiConfig( JMaxApplication.getFtsServer(), 
							     JMaxApplication.getRootPatcher(), 
							     args.getInt( 0)));
	}
      });
    FtsObject.registerMessageHandler( FtsConfig.class, FtsSymbol.get("audio_config"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsConfig)obj).setAudioConfig( new FtsAudioConfig( JMaxApplication.getFtsServer(), 
							       JMaxApplication.getRootPatcher(), 
							       args.getInt( 0)));
	}
      });
     FtsObject.registerMessageHandler( FtsConfig.class, FtsSymbol.get("name"), new FtsMessageHandler(){
	 public void invoke( FtsObject obj, FtsArgs args)
	 {
	   ((FtsConfig)obj).setFileName( args.getSymbol( 0).toString());
	 }
       });
     FtsObject.registerMessageHandler( FtsConfig.class, FtsSymbol.get("setDirty"), new FtsMessageHandler(){
	 public void invoke( FtsObject obj, FtsArgs args)
	 {
	   ((FtsConfig)obj).setDirty( args.getInt( 0) == 1);
	 }
       });
  }
    
  public FtsConfig(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);
  }

  public FtsConfig() throws IOException
  { 
    super( JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("config"));
  }

  void setMidiConfig( FtsMidiConfig mc)
  {
    midiConfig = mc;
  }

  public FtsMidiConfig getMidiConfig()
  {
    return midiConfig;
  }

  void setAudioConfig( FtsAudioConfig ac)
  {
    audioConfig = ac;
  }
  
  public FtsAudioConfig getAudioConfig()
  {
    return audioConfig;
  }

  public void upload()
  {
    try
      {
	send( FtsSymbol.get("upload"));
      }
    catch(IOException e)
      {
	System.err.println("FtsConfig: I/O Error sending upload Message!");
	e.printStackTrace(); 
      }
  }

  public void save( String fileName)
  {
    args.clear();
    args.addSymbol( FtsSymbol.get( fileName));
    try
      {
	send( FtsSymbol.get("save"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsConfig: I/O Error sending save Message!");
	e.printStackTrace(); 
      }
  }

//   public void load( String fileName)
//   {
//     args.clear();
//     args.addSymbol( FtsSymbol.get( fileName));
//     try
//       {
// 	send( FtsSymbol.get("load"), args);
//       }
//     catch(IOException e)
//       {
// 	System.err.println("FtsConfig: I/O Error sending load Message!");
// 	e.printStackTrace(); 
//       }
//   }

  public void openEditor(int nArgs, FtsAtom[] args)
  {
    if(getEditorFrame() == null)
      setEditorFrame( new ConfigurationEditor(this));
    
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

  public void setFileName( String fn)
  {
    this.fileName = fn;
    
  }
  
  public String getFileName()
  {
    return fileName;
  }

  private String fileName = null;
  private boolean isDirty = false;
  private FtsMidiConfig midiConfig = null;
  private FtsAudioConfig audioConfig = null;
}

