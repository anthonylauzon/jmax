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

/** The mda module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class MdaModule
{
  static public void initModule()
  {
    // Install the local mda entities

    // Mda.installDocumentHandler( new MaxTclFileDocumentHandler());

    Mda.installDocumentHandler( new MaxTclExecutedDocumentHandler());
    Mda.installDocumentType(new MaxTclExecutedDocumentType());

    // Install the tcl commands for Mda

    ircam.jmax.mda.tcl.TclMdaPackage.installPackage();
  }
}
