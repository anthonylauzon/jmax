package ircam.jmax.mda; 

import java.util.*;
import com.sun.java.swing.*;

/** This class represent a type of editable data, with 
 * all its interesting properties.
 *
 * This class maintain a list of all types (its instances) and
 * provide a function to get a type by name, and an enumerator
 * of all the types; a function is given to install a type in 
 * the global table.
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

  static DefaultListModel typeList = new DefaultListModel();

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
  
  static public DefaultListModel getTypes()
  {
    return typeList;
  }

  static public void installDataType(MaxDataType type)
  {
    typeList.addElement(type);
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                Instance variables                                         */
  /*                                                                           */
  /*****************************************************************************/

  /** The type name */

  String name;

  /** a name valid for user interface purposes ONLY */

  String prettyName; 

  /** The instance list */

  DefaultListModel instanceList = new DefaultListModel();

  /** The defaut editor factory; this object should actually
    build an indirect object that actually build the real 
    editor, and keep the relationship between the real editor
    and the mda system */

  MaxDataEditorFactory defaultEditorFactory = null;

  /*****************************************************************************/
  /*                                                                           */
  /*                 Constructors                                              */
  /*                                                                           */
  /*****************************************************************************/

  protected MaxDataType(String name)
  {
    this.name = name;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                 Type Handling                                             */
  /*                                                                           */
  /*****************************************************************************/

  /** Get the type name */

  public String getName()
  {
    return name;
  }

  /** Get the type pretty name, valid only for UI purposes */

  public String getPrettyName()
  {
    return prettyName;
  }

  /** Set the pretty printing name */

  public void setPrettyName(String name)
  {
    prettyName = name;
  }

  /** Set the default editor factory
   */

  public void setDefaultEditorFactory(MaxDataEditorFactory  editorFactory) 
  {
    defaultEditorFactory = editorFactory;
  }

  /** Get the default editor factory
   */

  public MaxDataEditorFactory getDefaultEditorFactory() 
  {
    return defaultEditorFactory;
  }

  /** 
   * Check if we have an editor factory for this type 
   */

  public boolean haveEditorFactory()
  {
    return defaultEditorFactory != null;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*                Instance Handling                                          */
  /*                                                                           */
  /*****************************************************************************/

  /** Create a new empty instance of the type */

  abstract public MaxData newInstance();

  /** Return true if the data type can instantiate a empty instance
   * from scratch (not all the data types can); only the data type
   * that can will be automatically put in the "New" menu.
   */

  public boolean canMakeNewInstance()
  {
    return false;
  }

  /** Register an instance in the instance table (called by the MaxData 
    constructor) */

  void registerInstance(MaxData instance)
  {
    instanceList.addElement(instance);
  }

  /** private, called when an instance is disposed */

  void disposeInstance(MaxData instance)
  {
    instanceList.removeElement(instance);
  }

  /** Get the ListModel (a defaultlistmodel, actually) of all the active instances */

  public DefaultListModel instances()
  {
    return instanceList;
  }
}
