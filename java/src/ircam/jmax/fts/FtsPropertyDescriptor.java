package ircam.jmax.fts;

import java.util.*;


/** Property descriptor.
 * 
 * Describe meta-properties of a property; 
 * current supported meta-properties are:
 * <ul>
 * <li> Persistent/volatile
 * <li> Client only/FTS mirrowed
 * </ul>
 *
 * The meta-properties are set/reset with a set of static functions,
 * that may generate instances of the class.
 */

public class FtsPropertyDescriptor
{
  Object defaultValue = null;

  private static Hashtable propertyDescriptors = new Hashtable();

  /** Get the default value for a property; if set, a default value
   *  is returned if the property value is not set for the object.
   * also, persistent property which value is the same of the 
   * default value are not saved.
   */

  static public Object getDefaultValue(String property)
  {
    if (propertyDescriptors.containsKey(property))
      return ((FtsPropertyDescriptor) propertyDescriptors.get(property)).defaultValue;
    else
      return null;
  }

  /** Set the default value for the property 
   *  If set, a default value
   *  is returned if the property value is not set for the object.
   *  also, persistent property which value is the same of the 
   *  default value are not saved.
   */

  public static void setDefaultValue(String property, Object v)
  {
    if (propertyDescriptors.containsKey(property))
      ((FtsPropertyDescriptor) propertyDescriptors.get(property)).defaultValue = v;
    else
      {
	FtsPropertyDescriptor p = new FtsPropertyDescriptor();

	p.defaultValue = v;
	propertyDescriptors.put(property, p);
      }
  }
}

