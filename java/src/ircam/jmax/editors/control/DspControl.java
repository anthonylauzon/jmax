package ircam.jmax.editors.control;

import java.beans.*;
import ircam.jmax.fts.*;

public class DspControl extends FtsRemoteData
{
  static final int DAC_SLIP_STATE     = 1;
  static final int INVALID_FPE_STATE  = 2;
  static final int DIVIDE_BY_ZERO_FPE_STATE  = 3;
  static final int OVERFLOW_FPE_STATE = 4;

  protected Integer dacSlip;
  protected Integer invalidFpe;
  protected Integer divideByZeroFpe;
  protected Integer overflowFpe;

  protected PropertyChangeSupport listeners;

  public DspControl()
  {
    super();

    dacSlip    = new Integer(0);
    invalidFpe = new Integer(0);
    divideByZeroFpe = new Integer(0);
    overflowFpe = new Integer(0);

    listeners = new PropertyChangeSupport(this);
  }

  /* Accessors for fields */

  Integer getDacSlip()
  {
    return dacSlip;
  }

  Integer getInvalidFpe ()
  {
    return invalidFpe;
  }

  Integer getDivideByZeroFpe()
  {
    return divideByZeroFpe;
  }

  Integer getOverflowFpe ()
  {
    return overflowFpe ;
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

  public final void call( int key, FtsMessage msg)
  {
    Integer oldValue, newValue;
    String prop;

    if (msg.getNumberOfArguments() >= 2)
      newValue = (Integer) msg.getArgument(2);
    else
      newValue = null;

    switch(key)
      {
      case DAC_SLIP_STATE:
	oldValue = dacSlip;
	dacSlip = newValue;
	prop = "dacSlip";
	break;
      case INVALID_FPE_STATE:
	oldValue = invalidFpe;
	invalidFpe = newValue;
	prop = "invalidFpe";
	break;
      case DIVIDE_BY_ZERO_FPE_STATE:
	oldValue = divideByZeroFpe;
	divideByZeroFpe = newValue;
	prop = "divideByZeroFpe";
	break;
      case OVERFLOW_FPE_STATE:
	oldValue = overflowFpe;
	overflowFpe = newValue;
	prop = "overflowFpe";
	break;
      default:
	return;
      }

    listeners.firePropertyChange(prop, oldValue, newValue);
  }

  /** Max Data support ?? */

  public String getName()
  {
    return "DspControl";
  }
}



