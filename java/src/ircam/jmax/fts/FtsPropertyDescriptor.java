package ircam.jmax.fts;

import java.util.*;

import tcl.lang.*;

/** Property descriptor.
 * 
 * Describe meta-properties of a property; 
 * current supported meta-properties are:
 * <ul>
 * <li> Persistent/volatile
 * <li> Client only/FTS mirrowed
 * <li> Parser from TclObject
 * <li> Parser to TclObject
 * </ul>
 *
 * The meta-properties are set/reset with a set of static functions,
 * that may generate instances of the class.
 */

public class FtsPropertyDescriptor
{
  boolean persistent = false;
  boolean clientOnly = true;
  FtsTclPropertyParser parser = null;
  Object defaultValue = null;

  private static Hashtable propertyDescriptors = new Hashtable();

  static public boolean isPersistent(String property)
  {
    if (propertyDescriptors.containsKey(property))
      return ((FtsPropertyDescriptor) propertyDescriptors.get(property)).persistent;
    else
      return false;
  }

  public static void setPersistent(String property, boolean v)
  {
    if (propertyDescriptors.containsKey(property))
      ((FtsPropertyDescriptor) propertyDescriptors.get(property)).persistent = v;
    else
      {
	FtsPropertyDescriptor p = new FtsPropertyDescriptor();

	p.persistent = v;
	propertyDescriptors.put(property, p);
      }
  }

  static boolean isClientOnly(String property)
  {
    if (propertyDescriptors.containsKey(property))
      return ((FtsPropertyDescriptor) propertyDescriptors.get(property)).clientOnly;
    else
      return true;
  }

  static void setClientOnly(String property, boolean v)
  {
    if (propertyDescriptors.containsKey(property))
      ((FtsPropertyDescriptor) propertyDescriptors.get(property)).clientOnly = v;
    else
      {
	FtsPropertyDescriptor p = new FtsPropertyDescriptor();

	p.clientOnly = v;
	propertyDescriptors.put(property, p);
      }
  }


  static void setTclParser(String property, FtsTclPropertyParser parser)
  {
    if (propertyDescriptors.containsKey(property))
      ((FtsPropertyDescriptor) propertyDescriptors.get(property)).parser = parser;
    else
      {
	FtsPropertyDescriptor p = new FtsPropertyDescriptor();

	p.parser = parser;
	propertyDescriptors.put(property, p);
      }
  }

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


  /** Unparse a value for a given property; use either 
   *  a custom unparser installed in the property
   * descriptor, or use toString.
   */

  static String unparse(String property, Object value)
  {
    if (propertyDescriptors.containsKey(property))
      {
	FtsTclPropertyParser parser;

	parser = ((FtsPropertyDescriptor) propertyDescriptors.get(property)).parser;

	if (parser != null)
	  return parser.unparse(value);
	else
	  return value.toString();
      }
    else
      return value.toString();
  }


  /** Parse a TCL value for a given property; use either 
   *  a custom parser installed in the property
   * descriptor, or try a default strategy (first Object, then Int, then Float, 
   * otherwise String).
   */

  static Object parse(Interp interp, String property, TclObject value)
  {
    if (propertyDescriptors.containsKey(property) &&
	((FtsPropertyDescriptor) propertyDescriptors.get(property)).parser != null)
      return ((FtsPropertyDescriptor) propertyDescriptors.get(property)).parser.parse(interp, value);
    else
      {
	try
	  {
	    return ReflectObject.get(interp, value);
	  }
	catch (TclException e)
	  {
	    try
	      {
		return new Integer(TclInteger.get(interp, value));
	      }
	    catch (TclException e2)
	      {
		try
		  {
		    return new Float(TclDouble.get(interp, value));
		  }
		catch (TclException e3)
		  {
		    return value.toString();
		  }
	      }
	  }
      }
  }
}
