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
package ircam.jmax.fts;

import java.util.*;

/**
 * This class provide a registration service for reference urls.
 * The class is filled thru the TCL Command <b>referenceURL</b>.
 *
 * @see FtsReferenceURLCmd
 */


public class FtsReferenceURLTable
{
  static Hashtable ftsReferenceURLTable = new Hashtable(256, 0.99f);

  static public void add(String className, String url)
  {
    ftsReferenceURLTable.put(className, url);
  }

  static public boolean exists(String className)
  {
    return ftsReferenceURLTable.containsKey(className);
  }

  static String getReferenceURL(String className)
  {
    return (String) ftsReferenceURLTable.get(className);
  }

  public static String getReferenceURL(FtsObject obj)
  {
    if (exists(obj.getClassName()))
      return getReferenceURL(obj.getClassName());
    else
      return null;
  }
}

