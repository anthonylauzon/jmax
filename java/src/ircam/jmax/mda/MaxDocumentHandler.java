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
import java.io.*;
import java.awt.*;

import javax.swing.*;

/**
 * An Handler for documents.
 * A Document Handler is an object able to load a Max Document 
 * from a document file; the class also handle the document base of all
 * the existing MaxDocumentHandler, and provide a static function
 * to find the document handler can reconize an address, and another
 * to directly load the document.
 */

abstract public class MaxDocumentHandler
{
  /** Return true if this Document Handler can load a new instance
    from the given address; by default return true if the file
    exists and it is readable */

  public boolean canLoadFrom(File file)
  {
    return file.exists() && file.canRead();
  }


  /** Return true if this document handler can save 
    to the given address; by default return true if the file
    exists, if it writable, and if we can load from it.
    */

  public boolean canSaveTo(File file)
  {
    if (file.exists() && file.canRead())
      return file.canWrite() && canLoadFrom(file);
    else
      return file.canWrite();
  }

  /** Return true if this document handler can save a given instance
    to the given address; by default return false */

  public boolean canSaveTo(MaxDocument document, File file)
  {
    if (file.exists())
      return file.canWrite() && canLoadFrom(file);
    else
      return file.canWrite();
  }

  /** Load an document from a document file, in a given context.
    If you want to call this, probabily you really want to call
    the static loadDocument.
    */

  abstract protected MaxDocument loadDocument(MaxContext context, File file) throws MaxDocumentException;

  abstract public void saveDocument(MaxDocument document, File file) throws MaxDocumentException;

  protected void saveSubDocument(MaxDocument document, MaxData data, File file) throws MaxDocumentException
  {
    throw new MaxDocumentException("SYSTEM ERROR: Cannot save subdocuments !");
  }

  /** Return a string describing the kind of files this handler can load 
    It is used for the file chooser dialog
    */

  abstract public String getDescription();

  public Icon getIcon()
  {
    return null;
  }
}







