package ircam.jmax.fts;

/**
 * This class represent an Atom list 
 * Atom lists cannot  be updated/changed partially, for the moment.
 * A string representation is supported for atom lists
 */

public class FtsAtomList
{
  FtsAtomList()
  {
  }

  /** Get the list in text form */

  public String getValuesAsText()
  {
  }

  /** Set the list as text */
  
  public void setValuesAsText(String value)
  {
  }

  /** Get the list   */

  public Vector values()
  {
  }

  /** Get the whole content  of the vector from FST */

  public void forceUpdate()
  {
  }

  /** Declare that all the  values in the vector has been changed
      and should be sent to FTS
      */

  public void changed()
  {
  }
}
