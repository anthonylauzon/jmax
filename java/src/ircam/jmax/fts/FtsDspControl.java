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

import java.beans.*;
import java.io.*;

import ircam.fts.client.*;
import ircam.jmax.*;

/** Java class for the dsp control remote data class.
 *
 * It offer visibility and control over a number of DSP related
 * server parameter and status.
 * It is implemented as a Java Beans, with Java Beans Properties 
 * corresponding to DSP server properties.
 */

public class FtsDspControl extends FtsObject
{
  protected int dacSlip;
  protected int invalidFpe;
  protected int divideByZeroFpe;
  protected int overflowFpe;
  protected int denormalizedFpe;
  protected int samplingRate;
  protected int fifoSize;
  protected boolean dspOn;

  protected PropertyChangeSupport listeners;
  boolean atomic = false;

  protected FtsArgs args = new FtsArgs();

  static
  {
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setDspOnState"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setDspOn( ( args.getInt( 0) == 0) ? false : true);
	}
      });
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setFifoSize"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setFifoSize( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setSamplingRate"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setSamplingRate( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setDACSlipState"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setDacSlip( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setInvalidFpeState"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setInvalidFpe( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setDivideByZeroFpeState"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setDivideByZeroFpe( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setOverflowFpeState"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setOverflowFpe( args.getInt( 0));
	}
      });
    FtsObject.registerMessageHandler( FtsDspControl.class, FtsSymbol.get("setDenormalizeFpeState"), new FtsMessageHandler(){
	public void invoke( FtsObject obj, FtsArgs args)
	{
	  ((FtsDspControl)obj).setDenormalizedFpe( args.getInt( 0));
	}
      });
  }
    
  public FtsDspControl() throws IOException
  {
    super(JMaxApplication.getServer(), JMaxApplication.getServer().getRoot(), FtsSymbol.get("__dspcontrol"));

    dacSlip         = 0;
    invalidFpe      = 0;
    divideByZeroFpe = 0;
    overflowFpe     = 0;
    denormalizedFpe = 0;
    samplingRate    = 0;
    fifoSize        = 0;
    dspOn           = false;

    listeners = new PropertyChangeSupport(this);

    FtsPatcherObject.addGlobalEditListener(new FtsEditListener(){	    
	public void objectAdded(FtsObject object)
	{
	  if(!atomic && dspOn) restart();
	};
	public void objectRemoved(FtsObject object){};
	public void connectionAdded(FtsConnection connection)
	{
	  if(!atomic && dspOn) restart();
	};
	public void connectionRemoved(FtsConnection connection){};
	public void atomicAction(boolean active)
	{
	  atomic = active;
	  if(!atomic && dspOn) restart();
	};
      });
  }

  /* Accessors for fields */

  public int getDacSlip()
  {
    return dacSlip;
  }

  public int getInvalidFpe()
  {
    return invalidFpe;
  }

  public int getDivideByZeroFpe()
  {
    return divideByZeroFpe;
  }

  public int getOverflowFpe ()
  {
    return overflowFpe ;
  }

  public int getDenormalizedFpe ()
  {
    return denormalizedFpe ;
  }

  public int getSamplingRate()
  {
    return samplingRate;
  }

  public int getFifoSize()
  {
    return fifoSize;
  }

  public boolean getDspOn()
  {
    return dspOn;
  }

  /* Modifiers, for values that can be modified */

  public void setSamplingRate(int value)
  {
    int old = samplingRate;
    samplingRate = value;
    listeners.firePropertyChange("samplingRate", new Integer(old), new Integer(samplingRate));
  }

  public void setFifoSize(int value)
  {
    int old = fifoSize;
    fifoSize = value;
    listeners.firePropertyChange("fifoSize", new Integer(old), new Integer(fifoSize));
  }

  public void setInvalidFpe(int value)
  {
    int old = invalidFpe;
    invalidFpe = value;
    listeners.firePropertyChange("invalidFpe", new Integer(old), new Integer(invalidFpe));
  }

  public void setOverflowFpe(int value)
  {
    int old = overflowFpe;
    overflowFpe = value;
    listeners.firePropertyChange("overflowFpe", new Integer(old), new Integer(overflowFpe));
  }

  public void setDivideByZeroFpe(int value)
  {
    int old = divideByZeroFpe;
    divideByZeroFpe = value;
    listeners.firePropertyChange("divideByZeroFpe", new Integer(old), new Integer(divideByZeroFpe));
  }

  public void setDenormalizedFpe(int value)
  {
    int old = denormalizedFpe;
    denormalizedFpe = value;
    listeners.firePropertyChange("denormalizedFpe", new Integer(old), new Integer(denormalizedFpe));
  }

  public void setDacSlip(int value)
  {
    int old = dacSlip;
    dacSlip = value;
    listeners.firePropertyChange("dacSlip", new Integer(old), new Integer(dacSlip));
  }

  public void setDspOn(boolean value)
  {
    boolean old = dspOn;
    dspOn = value;
    listeners.firePropertyChange("dspOn", new Boolean(old), new Boolean(dspOn));
  }

  public void requestSetDspOn(boolean value)
  {
    args.clear();
    args.addBoolean(value);
      
    try{
      send( FtsSymbol.get("dsp_on"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsDspControl: I/O Error sending dspOn Message!");
	e.printStackTrace(); 
      }
  }

  public void dspPrint()
  {
    try{
      send( FtsSymbol.get("dsp_print"));
    }
    catch(IOException e)
      {
	System.err.println("FtsDspControl: I/O Error sending dsp_print Message!");
	e.printStackTrace(); 
      }
  }

  public void restart()
  {
    try{
      send( FtsSymbol.get("dsp_restart"));
    }
    catch(IOException e)
      {
	System.err.println("FtsDspControl: I/O Error sending dsp_restart Message!");
	e.printStackTrace(); 
      }
  }

  /* Fpe support */

  public void startFpeCollecting(FtsObjectSet set)
  {
    args.clear();
    args.addObject(set);
      
    try{
      send( FtsSymbol.get("fpe_start_collect"), args);
    }
    catch(IOException e)
      {
	System.err.println("FtsDspControl: I/O Error sending fpe_start_collect Message!");
	e.printStackTrace(); 
      }
  }

  public void stopFpeCollecting()
  {
    try{
      send( FtsSymbol.get("fpe_stop_collecting"));
    }
    catch(IOException e)
      {
	System.err.println("FtsDspControl: I/O Error sending fpe_stop_collect Message!");
	e.printStackTrace(); 
      }
  }

  public void clearFpeCollecting()
  {
    try{
      send( FtsSymbol.get("fpe_clear_collecting"));
    }
    catch(IOException e)
      {
	System.err.println("FtsDspControl: I/O Error sending fpe_clear_collecting Message!");
	e.printStackTrace(); 
      }
  }

  public void setCheckNan( boolean checkNan)
  {
    /*sendArgs[0].setInt(checkNan ? 1 : 0);
      sendMessage(FtsObject.systemInlet, "set_check_nan", 1, sendArgs);*/
  }

  /* Listeners support */

  public void addPropertyChangeListener(PropertyChangeListener listener)
  {
    listeners.addPropertyChangeListener(listener);
  }

  public void removePropertyChangeListener(PropertyChangeListener listener)
  {
    listeners.removePropertyChangeListener(listener);
  }
}

