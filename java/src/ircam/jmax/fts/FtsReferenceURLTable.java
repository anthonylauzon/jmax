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

