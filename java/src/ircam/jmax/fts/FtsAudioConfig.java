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

public class FtsAudioConfig extends FtsObject
{
  protected FtsArgs args = new FtsArgs();

  static
  {
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("sampling_rates"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).setSamplingRates( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("buffer_sizes"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).setBufferSizes( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("sampling_rate"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).setSamplingRate( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("buffer_size"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).setBufferSize( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("inputs"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).setSources( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("outputs"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).setDestinations( args.getLength(), args.getAtoms());
	}
      });
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("insert"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).insertLabel( args.getInt( 0), args.getInt( 1), args.getAtoms(), 2, args.getLength());
	}
      });    
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("remove"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).removeLabel( (FtsAudioLabel)args.getObject( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsAudioConfig.class, FtsSymbol.get("clear"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsAudioConfig)obj).clear();
	}
      });
  }
    
  public FtsAudioConfig(FtsServer server, FtsObject parent, int id)
  {
    super(server, parent, id);
    labels = new Vector();
  }

  public FtsAudioConfig() throws IOException
  { 
    super( JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), FtsSymbol.get("audio_config"));
    labels = new Vector();
  }

  public FtsAudioConfig( String isBidon)
  { 
    super( JMaxApplication.getFtsServer(), JMaxApplication.getRootPatcher(), 100);
    sources = new String[4];
    sources[0] = "source1"; sources[1] = "source2"; sources[2] = "source3"; sources[3] = "source4";
    sourceChannels = new int[4];
    sourceChannels[0] = 4; sourceChannels[1] = 6; sourceChannels[2] = 2; sourceChannels[3] = 8; 
    destinations = new String[4];
    destinations[0] = "dest1"; destinations[1] = "dest2"; destinations[2] = "dest3"; destinations[3] = "dest4"; 
    destinationChannels = new int[4];
    destinationChannels[0] = 2; destinationChannels[1] = 4; destinationChannels[2] = 8; destinationChannels[3] = 4; 
    bufferSizes = new String[7];
    bufferSizes[0] = "64"; bufferSizes[1] = "128"; bufferSizes[2] = "256"; bufferSizes[3] = "512"; 
    bufferSizes[4] = "1024"; bufferSizes[5] = "2048"; bufferSizes[6] = "4096";
    samplingRates = new String[5];
    samplingRates[0] = "32"; samplingRates[1] = "44.1"; samplingRates[2] = "48"; samplingRates[3] = "88.2"; 
    samplingRates[4] = "96";
    labels = new Vector();
  }

  public void setListener( ircam.jmax.editors.configuration.AudioConfigPanel listener)
  {
    this.listener = listener;
    
    for( Enumeration e = labels.elements(); e.hasMoreElements();)
      ((FtsAudioLabel)e.nextElement()).setListener( listener);
  }

  /*****************************************************/
  /***********     Client -----> Server    *************/
  /*****************************************************/  

  public void requestInsertLabel( int index, String label)
  {
    args.clear();    
    args.addInt( index);
    args.addSymbol( FtsSymbol.get( label));

    try
      {
	send( FtsSymbol.get( "insert"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioConfig: I/O Error sending insert Message!");
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
	System.err.println("FtsAudioConfig: I/O Error sending remove Message!");
	e.printStackTrace(); 
      }
  }

  public void requestSetBufferSize( int bufferSize)
  {
    args.clear();    
    args.addInt( bufferSize);

    try
      {
	send( FtsSymbol.get( "buffer_size"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioConfig: I/O Error sending buffer_size Message!");
	e.printStackTrace(); 
      }
  }

 public void requestSetSamplingRate( int sRate)
  {
    args.clear();    
    args.addInt( sRate);

    try
      {
	send( FtsSymbol.get( "sampling_rate"), args);
      }
    catch(IOException e)
      {
	System.err.println("FtsAudioConfig: I/O Error sending sampling_rate Message!");
	e.printStackTrace(); 
      }
  }
  
  /*************************************************************/

  void setSamplingRates(int nArgs, FtsAtom[] args)
  {
    samplingRates = new String[ nArgs];
    for(int i = 0; i < nArgs; i++)
      samplingRates[i] = ""+args[i].intValue;
  
    if(listener != null)
      listener.samplingRatesChanged();
  }

  public String[] getSamplingRates()
  {
    return samplingRates;
  }

  void setSamplingRate(int sr)
  {
    samplingRate = sr;
  }

  public int getSamplingRate()
  {
    return samplingRate;
  }

  void setBufferSizes(int nArgs, FtsAtom[] args)
  {
    bufferSizes = new String[ nArgs];
    for(int i = 0; i < nArgs; i++)
      bufferSizes[i] = ""+args[i].intValue;
  
    if(listener != null)
      listener.bufferSizesChanged();
  }

  public String[] getBufferSizes()
  {
    return bufferSizes;
  }

  void setBufferSize(int bs)
  {
    bufferSize = bs;
  }

  public int getBufferSize()
  {
    return bufferSize;
  }

  void setSources(int nArgs, FtsAtom[] args)
  {
    sources = new String[ nArgs/2];
    sourceChannels = new int[ nArgs/2];
    int j = 0;
    for(int i = 0; i < nArgs; i+=2)
      {
	sources[j] = args[i].symbolValue.toString();
	sourceChannels[j] = args[i+1].intValue;
	j++;
      }
    if(listener != null)
      listener.sourcesChanged();
  }

  public String[] getSources()
  {
    return sources;
  }

  public int getInDeviceChannels( String input)
  {
    int id = 0;
    for( int i = 0; i < sources.length; i++)
      if( sources[i].equals( input))
	{
	  id = i;
	  break;
	}
    return sourceChannels[id];
  }

  void setDestinations(int nArgs, FtsAtom[] args)
  {
    destinations = new String[ nArgs/2];
    destinationChannels = new int[ nArgs/2];
    int j = 0;
    for(int i = 0; i < nArgs; i+=2)
      {
	destinations[j] = args[i].symbolValue.toString();
	destinationChannels[j] = args[i+1].intValue;
	j++;
      }
    if(listener != null)
      listener.destinationsChanged();
  }

  public int getOutDeviceChannels( String output)
  {
    int id = 0;
    for( int i = 0; i < destinations.length; i++)
      if( destinations[i].equals( output))
	{
	  id = i;
	  break;
	}
    return destinationChannels[id];
  }

  public String[] getDestinations()
  {
    return destinations;
  }

  void insertLabel( int index, int id, FtsAtom[] args, int offset, int nArgs)
  {
    FtsAudioLabel label = new FtsAudioLabel( JMaxApplication.getFtsServer(), 
					     JMaxApplication.getRootPatcher(),
					     id, args, offset, nArgs);
    labels.insertElementAt( label, index);
    
    if( listener != null)
      label.setListener( listener);
  }

  void removeLabel( FtsAudioLabel label)
  {
    labels.remove( label);	
  }

  public Enumeration getLabels()
  {
    return labels.elements();
  }

  public FtsAudioLabel getLabelAt( int index)
  {
    return (FtsAudioLabel) labels.elementAt( index);
  }

  public FtsAudioLabel getLabelByName( String lb)
  {
    FtsAudioLabel label;
    for( Enumeration e = labels.elements(); e.hasMoreElements();)
      {
	label = (FtsAudioLabel)e.nextElement();
	if( lb.equals( label.getLabel()))
	  return label;
      }
    return null;
  }

  public int getLabelIndex( FtsAudioLabel lb)
  {
    FtsAudioLabel label;
    int i = 0;
    for( Enumeration e = labels.elements(); e.hasMoreElements();)
      {
	label = (FtsAudioLabel)e.nextElement();
	if( lb.getLabel().equals( label.getLabel()))
	  return i;
	i++;
      }
    return -1;
  }

  public void clear()
  {
    labels.removeAllElements();
  }

  private String[] sources = new String[0];
  private int[] sourceChannels = new int[0];
  private String[] destinations = new String[0];
  private int[] destinationChannels = new int[0];
  private String[] bufferSizes = new String[0];
  private String[] samplingRates = new String[0];
  private Vector labels = null;  
  private int samplingRate;
  private int bufferSize;
  private ircam.jmax.editors.configuration.AudioConfigPanel listener = null;
}

