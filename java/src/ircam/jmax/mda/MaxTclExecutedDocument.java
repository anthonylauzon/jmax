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
 * A MaxTclExecutedDocument is an empty document resulting from
 * the execution of a simple .tcl file; it exists so that we
 * can execute .tcl files with "Open".
 *
 * It can be usefull for the scheme code, also.
 */

public class MaxTclExecutedDocument extends MaxDocument 
{
  public MaxTclExecutedDocument(MaxContext context)
  {
    super(context, Mda.getDocumentTypeByName(".tcl"));
  }
}






