 //
// jMax
// Copyright (C) 1999 by IRCAM
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.mda; 

import java.util.*;
import javax.swing.*;

/**
 * This class represent a type of editable document, with 
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

  /** Create a new empty document of the type, in the given context */

  abstract public MaxDocument newDocument(MaxContext context);


  /**
   * Create a new document for the given MaxData; the context is not passed,
   * because MaxData probabily leave already in a context.
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


  /** Return true if the document type can save a new document file (sub document)
   * from a given data.
   */

  public boolean canMakeSubDocumentFile(MaxData data)
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


