package ircam.jmax.fts;

import ircam.jmax.*;

/**
 * A generic FTS object with an float value; floatbox and sliders, for example
 */

public class FtsFloatValueObject extends FtsObject
{
  /*****************************************************************************/
  /*                                                                           */
  /*                               CONSTRUCTORS                                */
  /*                                                                           */
  /*****************************************************************************/

  float value; 

  /**
   * Create a FtsObject object;
   */

  public FtsFloatValueObject(FtsObject parent, String className, String description, int objId)
  {
    super(parent, className, null, description, objId);
  }

  public void setValue(float value)
  {
    this.value = value;
    Fts.getServer().putObjectProperty(this, "value", value);
  }

  public float getValue()
  {
    return value;
  }

  public void updateValue()
  {
    Fts.getServer().askObjectProperty(this, "value");
  }
       
  /* Over write the localPut message to handle value changes;
     Should get the FtsMessage and do a receiveFloat
   */

  protected void localPut(String name, float newValue)
  {
    if (name == "value")
      {
	value = newValue;
	
	if (listener instanceof FtsFloatValueListener)
	  ((FtsFloatValueListener) listener).valueChanged(newValue);
      }
    else
      super.localPut(name, newValue);
  }
}






