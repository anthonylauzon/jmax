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


/** This interface represent the minimum requirement on 
 *  an "Max Editing Unit", any Java object, part of a document,
 *  that we want to "edit" with a single user visible entity
 * called editor.
 * 
 * For the moment, quite minimal.
 */

public interface MaxData
{
  /** Get the document this data belong to */

  abstract public MaxDocument getDocument();
}


