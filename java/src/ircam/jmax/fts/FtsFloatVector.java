package ircam.jmax.fts;

/** This class represent an Float vector in 
 *  FTS
 */

class FtsFloatVector
{
  int[] values;

  FtsFloatVector(int size)
  {
  }

  /** Get the vector size */

  public int size()
  {
  }

  /** Get the values  */

  public float[] values()
  {
  }

  /** Get the whole content  of the vector from FST */

  public void forceUpdate()
  {
  }

  /** Declare that a range in the vector has been changed
      and this range need to be sent to FTS
      */

  public void changed(int from, int to)
  {
  }

  /** Declare that a value in the vector has been changed
      and this value need to be sent to FTS
      */

  public void changed(int idx)
  {
  }

  /** Declare that all the  values in the vector has been changed
      and should be sent to FTS
      */

  public void changed()
  {
  }
}
