package ircam.jmax.fts;

/** This inteface define the behaviour of an FtsObject that
 *  "have" an integer table; it is an interface so that
 *  can be implemented by different object in the FtsObject 
 * hierarchy, and not only "table"
 */

interface FtsIntegerVectorObject
{
  abstract public void saveVectorToFts();
  abstract public void loadVectorFromFts();
  abstract public void bindVector(FtsIntegerVector vector);
  abstract public void unbindVector(FtsIntegerVector vector);
}
