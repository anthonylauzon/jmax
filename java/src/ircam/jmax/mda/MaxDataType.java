package ircam.jmax.mda; 

import java.util.*;

/** This class represent a type of editable data, with 
 * all its interesting properties; interesting properties
 * are those properties that have system level interest :->
 *
 * This class maintain a list of all types (its instances) and
 * provide a function to get a type by name, and an enumerator
 * of all the types.
 *
 * Types cannot be deleted once loaded.
 * 
 * the type must be instantiated somewhere, either in the Max main
 * or during the loading of a package.
 */

abstract public class MaxDataType
{
  /*****************************************************************************/
  /*                                                                           */
  /*                Static Code                                         */
  /*                                                                           */
  /*****************************************************************************/

  static Vector typeList = new Vector();

  static public MaxDataType getTypeByName(String name)
  {
    for (int i = 0; i < typeList.size() ; i++)
      {
	MaxDataType type;

	type = (MaxDataType) typeList.elementAt(i);

	if (type.getName().equals(name))
	  return type;
      }

    return null;
  }
  
  static public Enumeration getTypes()
  {
    return typeList.elements();
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                Instance variables                                         */
  /*                                                                           */
  /*****************************************************************************/

  /** The type name */

  String name;

  /** The instance list */

  Vector instanceList = new Vector();

  /** The defaut editor class; this class should actually
    build a "Editor Factory" that actually build the real 
    editor, and keep the relationship between the real editor
    and the mda system */

  Class defaultEditorClass;

  /*****************************************************************************/
  /*                                                                           */
  /*                 Constructors                                              */
  /*                                                                           */
  /*****************************************************************************/

  protected MaxDataType(String name)
  {
    this.name = name;

    typeList.addElement(this);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                 Type Handling                                             */
  /*                                                                           */
  /*****************************************************************************/

  /** Get the type name, *only* for showing it to the user */

  public String getName()
  {
    return name;
  }

  /** the Java class instances belong to */

  abstract public Class instanceClass();

  /** Set the default editor class; throw an exception if the
   *  class do not implement the MaxDataEditor interface
   */

  public void setDefaultEditorClass(Class editorClass) throws MaxDataException
  {
    Class[] interfaces = editorClass.getInterfaces();

    
    for (int i = 0; i < interfaces.length ; i++)
      {
	if (interfaces[i] == MaxDataEditor.class)
	  {
	    defaultEditorClass = editorClass;
	    return;
	  }
      }

    throw new MaxDataException("default editor is not a MaxDataEditor");
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                Instance Handling                                          */
  /*                                                                           */
  /*****************************************************************************/

  /** Create a new empty instance of the type */

  abstract public MaxData newInstance();

  /** private, called when an instance is disposed */

  public void disposeInstance(MaxData instance)
  {
    instanceList.removeElement(instance);
  }

  /** Get an enumeration of all the active instances */

  public Enumeration instances()
  {
    return instanceList.elements();
  }
}
