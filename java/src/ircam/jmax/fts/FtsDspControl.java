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
import ircam.jmax.fts.*;

/** Java class for the dsp control remote data class.
 *
 * It offer visibility and control over a number of DSP related
 * server parameter and status.
 * It is implemented as a Java Beans, with Java Beans Properties 
 * corresponding to DSP server properties.
 */

public class FtsDspControl extends FtsObject
{
  protected Integer dacSlip;
  protected Integer invalidFpe;
  protected Integer divideByZeroFpe;
  protected Integer overflowFpe;
  protected Integer denormalizedFpe;
  protected Integer samplingRate;
  protected Integer fifoSize;
  protected Boolean dspOn;

  protected PropertyChangeSupport listeners;

  public FtsDspControl(Fts fts, FtsObject parent, String variableName, String classname, int nArgs, FtsAtom args[])
  {
    super(fts, parent, variableName, classname, "");

    dacSlip         = new Integer(0);
    invalidFpe      = new Integer(0);
    divideByZeroFpe = new Integer(0);
    overflowFpe     = new Integer(0);
    denormalizedFpe = new Integer(0);
    samplingRate    = new Integer(0);
    fifoSize        = new Integer(0);
    dspOn           = new Boolean(false);

    listeners = new PropertyChangeSupport(this);
  }

  /* Accessors for fields */

  public Integer getDacSlip()
  {
    return dacSlip;
  }

  public Integer getInvalidFpe()
  {
    return invalidFpe;
  }

  public Integer getDivideByZeroFpe()
  {
    return divideByZeroFpe;
  }

  public Integer getOverflowFpe ()
  {
    return overflowFpe ;
  }

  public Integer getDenormalizedFpe ()
  {
    return denormalizedFpe ;
  }

  public Integer getSamplingRate()
  {
    return samplingRate;
  }

  public Integer getFifoSize()
  {
    return fifoSize;
  }

  public Boolean getDspOn()
  {
    return dspOn;
  }

  /* Modifiers, for values that can be modified */

  public void setSamplingRate(Integer value)
  {
    samplingRate = value;
  }

  public void setFifoSize(Integer value)
  {
    fifoSize = value;
  }

  public void setDspOn(Boolean value)
  {
    if(value.booleanValue())
      sendArgs[0].setInt(1);
    else
      sendArgs[0].setInt(0);       

    sendMessage(FtsObject.systemInlet, "dsp_on", 1, sendArgs);
  }

  public void dspPrint()
  {
      sendMessage(FtsObject.systemInlet, "dsp_print", 0, null);
  }

  /* Fpe support */

  public void startFpeCollecting(FtsObjectSet set)
  {
      sendArgs[0].setObject(set); 
      sendMessage(FtsObject.systemInlet, "fpe_start_collect", 1, sendArgs);
  }

  public void stopFpeCollecting()
  {
      sendMessage(FtsObject.systemInlet, "fpe_stop_collecting", 0, null);
  }

  public void clearFpeCollecting()
  {
      sendMessage(FtsObject.systemInlet, "fpe_clear_collecting", 0, null);
  }

  public void setCheckNan( boolean checkNan)
  {
      sendArgs[0].setInt(checkNan ? 1 : 0);
      sendMessage(FtsObject.systemInlet, "set_check_nan", 1, sendArgs);
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

  public void setDACSlipState(int nArgs , FtsAtom args[])
  {
    Integer oldValue = dacSlip;
    dacSlip  = (Integer) args[0].getValue();

    listeners.firePropertyChange("dacSlip", oldValue, dacSlip);
  }
  public void setInvalidFpeState(int nArgs , FtsAtom args[])
  {
    Integer oldValue = invalidFpe;
    invalidFpe  = (Integer) args[0].getValue();

    listeners.firePropertyChange("invalidFpe", oldValue, invalidFpe);
  }
  public void setDivideByZeroFpeState(int nArgs , FtsAtom args[])
  {
    Integer oldValue = divideByZeroFpe;
    divideByZeroFpe  = (Integer) args[0].getValue();

    listeners.firePropertyChange("divideByZeroFpe", oldValue, divideByZeroFpe);
  }  
  public void setOverflowFpeState(int nArgs , FtsAtom args[])
  {
    Integer oldValue = overflowFpe;
    overflowFpe  = (Integer) args[0].getValue();

    listeners.firePropertyChange("overflowFpe", oldValue, overflowFpe);
  }
  public void setDenormalizeFpeState(int nArgs , FtsAtom args[])
  {
    Integer oldValue = denormalizedFpe;
    denormalizedFpe  = (Integer) args[0].getValue();

    listeners.firePropertyChange("denormalizedFpe", oldValue, denormalizedFpe);
  }
  public void setSamplingRate(int nArgs , FtsAtom args[])
  {
    Integer oldValue = samplingRate;
    samplingRate  = (Integer) args[0].getValue();

    listeners.firePropertyChange("samplingRate", oldValue, samplingRate);
  }
  public void setFifoSize(int nArgs , FtsAtom args[])
  {
    Integer oldValue = fifoSize;
    fifoSize  = (Integer) args[0].getValue();

    listeners.firePropertyChange("fifoSize", oldValue, fifoSize);
  }
  public void setDspOnState(int nArgs , FtsAtom args[])
  {
    Boolean oldValue = dspOn;
    if(args[0].getInt() == 0)
	dspOn  = new Boolean(false);
    else
	dspOn  = new Boolean(true);

    listeners.firePropertyChange("dspOn", oldValue, dspOn);
  } 
}





