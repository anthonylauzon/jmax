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

public class FtsAudioLabel extends FtsObject
{
  protected FtsArgs args = new FtsArgs();

  static
  {
    FtsObject.registerMessageHandler( FtsAudioLabel.class, FtsSymbol.get("label"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioLabel)obj).setLabelName( args.getSymbol( 0).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsAudioLabel.class, FtsSymbol.get("input"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioLabel)obj).setInput( args.getSymbol( 0).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsAudioLabel.class, FtsSymbol.get("output"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioLabel)obj).setOutput( args.getSymbol( 0).toString());
	}
      });
    FtsObject.registerMessageHandler( FtsAudioLabel.class, FtsSymbol.get("input_channel"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioLabel)obj).setInputChannel( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsAudioLabel.class, FtsSymbol.get("output_channel"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioLabel)obj).setOutputChannel( args.getInt( 0));
	}
      });
  }
  public FtsAudioLabel(FtsServer server, FtsObject parent, int id, FtsAtom args[], int offset, int length)
  {
    super(server, parent, id);

    label = args[ offset].symbolValue.toString();
    
    if( length-offset >= 3)
      {
	input = args[offset+1].symbolValue.toString();
	inChannel = args[offset+2].intValue;
      }
    if( length-offset == 5)
      {
	output = args[offset+3].symbolValue.toString();
	outChannel = args[offset+4].intValue;
      }
  }

  public void setListener( ircam.jmax.editors.configuration.AudioConfigPanel listener)
  {
    this.listener = listener;
  }

  /*****************************************************/
  /***********     Client -----> Server    *************/
  /*****************************************************/  
  public void requestChangeLabel( String label)
  {
    args.clear();   
    args.addSymbol( FtsSymbol.get( label));

    try
      {
	send( FtsSymbol.get( "label"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioLabel: I/O Error sending ChangeLabel Message!");
	e.printStackTrace(); 
      }
  }

  public void requestSetInput( String input)
  {
    args.clear();   
    args.addSymbol( FtsSymbol.get( input));

    try
      {
	send( FtsSymbol.get( "input"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioLabel: I/O Error sending input Message!");
	e.printStackTrace(); 
      }
  }

  public void requestSetOutput( String output)
  {
    args.clear();
    args.addSymbol( FtsSymbol.get( output));
    
    try
      {
	send( FtsSymbol.get( "output"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioLabel: I/O Error sending output Message!");
	e.printStackTrace(); 
      }
  }

  public void requestSetInChannel( int ch)
  {
    args.clear();
    args.addInt( ch);
    
    try
      {
	send( FtsSymbol.get( "input_channel"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioLabel: I/O Error sending in_channel Message!");
	e.printStackTrace(); 
      }
  }

  public void requestSetOutChannel( int ch)
  {
    args.clear();
    args.addInt( ch);
    
    try
      {
	send( FtsSymbol.get( "output_channel"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioLabel: I/O Error sending out_channel Message!");
	e.printStackTrace(); 
      }
  }
  /************************************************************/

  public String getLabel()
  {
    return label;
  }

  public void setLabelName( String label)
  {
    this.label = label;
    if( listener != null)
      listener.labelNameChanged( this);
  }

  public void setInput( String in)
  {
    input = in;
    if( listener != null)
      listener.labelInputChanged( this);
  }

  public String getInput()
  {
    return input;
  }

  public void setInputChannel( int inCh)
  {
    inChannel = inCh;
    if( listener != null)
      listener.labelInChannelChanged( this);
  }

  public int getInputChannel()
  {
    return inChannel;
  }

  public void setOutput( String out)
  {
    this.output = out;
    if( listener != null)
      listener.labelOutputChanged( this);
  }

  public String getOutput()
  {
    return output;
  }

  public void setOutputChannel( int outCh)
  {
    outChannel = outCh;
    if( listener != null)
      listener.labelOutChannelChanged( this);
  }

  public int getOutputChannel()
  {
    return outChannel;
  }

  private String label = null;
  private String input = null;
  private String output = null;
  public int inChannel, outChannel; 
  private ircam.jmax.editors.configuration.AudioConfigPanel listener = null;
}

