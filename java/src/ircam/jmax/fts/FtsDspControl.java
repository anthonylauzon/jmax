package ircam.jmax.fts;

import java.beans.*;
import ircam.jmax.fts.*;

public class FtsDspControl extends FtsRemoteData
{
  /* Incoming calls */

  static final int DAC_SLIP_STATE     = 1;
  static final int INVALID_FPE_STATE  = 2;
  static final int DIVIDE_BY_ZERO_FPE_STATE  = 3;
  static final int OVERFLOW_FPE_STATE = 4;
  static final int SAMPLING_RATE = 5;
  static final int FIFO_SIZE     = 6;
  static final int DSP_ON        = 7;

  /* Outgoing calls (also DSP_ON can be outgoing) */

  static final int FPE_START_COLLECT = 8;
  static final int FPE_STOP_COLLECT  = 9;

  static final int DSP_PRINT  = 10;

  protected Integer dacSlip;
  protected Integer invalidFpe;
  protected Integer divideByZeroFpe;
  protected Integer overflowFpe;
  protected Integer samplingRate;
  protected Integer fifoSize;
  protected Boolean dspOn;

  protected PropertyChangeSupport listeners;

  public FtsDspControl()
  {
    super();

    dacSlip         = new Integer(0);
    invalidFpe      = new Integer(0);
    divideByZeroFpe = new Integer(0);
    overflowFpe     = new Integer(0);
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
    
    // Remote call to be implemented
  }

  public void setFifoSize(Integer value)
  {
    fifoSize = value;

    // Remote call to be implemented
  }

  public void setDspOn(Boolean value)
  {
    dspOn = value;

    remoteCall( DSP_ON, (dspOn.booleanValue() ? 1 : 0));
  }

  public void dspPrint()
  {
    remoteCall( DSP_PRINT);
  }

  /* Fpe support */

  public void startFpeCollecting(FtsObjectSet set)
  {
    Object args[] = new Object[1];

    args[0] = set;
    remoteCall(FPE_START_COLLECT, args);
  }

  public void stopFpeCollecting()
  {
    remoteCall(FPE_STOP_COLLECT);
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

  /* remote call support */

  public final void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    Object oldValue, newValue;
    String prop;

    newValue = (Integer) stream.getNextArgument();

    switch(key)
      {
      case DAC_SLIP_STATE:
	oldValue = dacSlip;
	dacSlip  = (Integer) newValue;
	prop     = "dacSlip";
	break;
      case INVALID_FPE_STATE:
	oldValue = invalidFpe;
	invalidFpe = (Integer) newValue;
	prop     = "invalidFpe";
	break;
      case DIVIDE_BY_ZERO_FPE_STATE:
	oldValue = divideByZeroFpe;
	divideByZeroFpe = (Integer) newValue;
	prop     = "divideByZeroFpe";
	break;
      case OVERFLOW_FPE_STATE:
	oldValue = overflowFpe;
	overflowFpe = (Integer) newValue;
	prop     = "overflowFpe";
	break;
      case SAMPLING_RATE:
	oldValue = samplingRate;
	samplingRate = (Integer) newValue;
	prop     = "samplingRate";
	break;
      case FIFO_SIZE:
	oldValue = fifoSize;
	fifoSize = (Integer) newValue;
	prop     = "fifoSize";
	break;
      case DSP_ON:
	if ((newValue instanceof Integer) && (((Integer) newValue).intValue() == 0))
	  newValue = new Boolean(false);
	else
	  newValue = new Boolean(true);

	oldValue = dspOn;
	dspOn    = (Boolean) newValue;
	prop     = "dspOn";
	break;
      default:
	return;
      }

    listeners.firePropertyChange(prop, oldValue, newValue);
  }
}





