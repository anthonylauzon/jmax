//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.mda;


/**
 * Class defining the an executable tcl document.
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

  /** Tcl executed documents cannot be created inside jMax (still, we may add
   a text editor, actually) */

  public MaxDocument newDocument(MaxContext ignore)
  {
    return null;
  }

  /** Tcl executed documents cannot be created inside jMax (still, we may add
   a text editor, actually) */

  public boolean canMakeNewDocument(MaxContext ignore)
  {
    return false;
  }
}





