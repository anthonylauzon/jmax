package ircam.jmax.mda; 

import java.util.*;
import com.sun.java.swing.*;

/** This class represent a type of editable document, with 
 * all its interesting properties.
 *
 * This class maintain a list of all types (its documents) and
 * provide a function to get a type by name, and an enumerator
 * of all the types; a function is given to install a type in 
 * the global table.
 *
 * Types cannot be deleted once loaded.
 * 
 * the type must be instantiated somewhere, either in the Max main
 * or during the loading of a package.
 */

abstract public class MaxDocumentType
{
  /*****************************************************************************/
  /*                                                                           */
  /*                Instance variables                                         */
  /*                                                                           */
  /*****************************************************************************/

  /** The type name */

  String name;

  /** a name valid for user interface purposes ONLY */

  String prettyName; 

  /** The document list */

  DefaultListModel documentList = new DefaultListModel();

  /*****************************************************************************/
  /*                                                                           */
  /*                 Constructors                                              */
  /*                                                                           */
  /*****************************************************************************/

  protected MaxDocumentType(String name)
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

  /*****************************************************************************/
  /*                                                                           */
  /*                Document Handling                                          */
  /*                                                                           */
  /*****************************************************************************/


  /** return true if the document is editable; some documents may be read/load only  */

  public boolean isEditable()
  {
    return true;
  }

  /** Create a new empty document of the type */

  abstract public MaxDocument newDocument();


  /** Create a new document for the given MaxData;
   */

  public MaxDocument newDocument(MaxData data) throws MaxDocumentException
  {
    throw new MaxDocumentException("Cannot create document for " + data);
  }

  /** Return true if the document type can instantiate a empty document
   * from scratch (not all the document types can); only the document type
   * that can will be automatically put in the "New" menu.
   */

  public boolean canMakeNewDocument()
  {
    return false;
  }

  /** Return true if the document type can instantiate a new document
   * from a given data.
   */

  public boolean canMakeNewDocument(MaxData data)
  {
    return false;
  }

  /** Register an document in the document table (called by the MaxDocument 
    constructor) */

  void registerDocument(MaxDocument document)
  {
    documentList.addElement(document);
  }

  /** private, called when an document is disposed */

  void disposeDocument(MaxDocument document)
  {
    documentList.removeElement(document);
  }

  /** Get the ListModel (a defaultlistmodel, actually) of all the active documents */

  public DefaultListModel getDocuments()
  {
    return documentList;
  }
}


