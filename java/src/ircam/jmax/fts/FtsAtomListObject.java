package ircam.jmax.fts;

/** This inteface define the behaviour of an FtsObject that
 *  "have" an integer table; it is an interface so that
 *  can be implemented by different object in the FtsObject 
 * hierarchy, and not only "table"
 */

interface FtsAtomListObject
{
  abstract public void saveAtomListToFts();
  abstract public void loadAtomListFromFts();
  abstract public void bindList(FtsAtomList vector);
  abstract public void unbindList(FtsAtomList vector);
}
