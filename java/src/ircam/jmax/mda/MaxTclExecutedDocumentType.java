//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.mda;


/** Class defining the fts patch type (named patch);
 */

public class MaxTclExecutedDocumentType extends MaxDocumentType
{
  public MaxTclExecutedDocumentType()
  {
    super(".tcl");
    setPrettyName("Tcl");
  }

  /** Tcl executed documents are not editable */

  public boolean isEditable()
  {
    return false;
  }

  /** Build an empty instance of FtsPatcherDocument, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxDocument newDocument()
  {
    return null;
  }

  public boolean canMakeNewDocument()
  {
    return false;
  }
}





