//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import java.util.*;

/**
 * A class used to register ValueInfo and acces it by name*/

public class ValueInfoTable {

  public static void registerInfo(ValueInfo info)
  {
    infos.put(info.getName(), info);
  }
  
  public static ValueInfo getValueInfo(String type)
  {
    ValueInfo info = (ValueInfo)(infos.get(type));
    if(info == null)
      info = AnythingValue.info;
    
    return info;
  }

  public static Enumeration getTypeNames()
  {
    return infos.keys();
  }

  public static void init()
  {
    ValueInfoTable.registerInfo(AmbitusValue.info);
    ValueInfoTable.registerInfo(IntegerValue.info);
    ValueInfoTable.registerInfo(MessageValue.info);
    ValueInfoTable.registerInfo(FloatValue.info);
  }

  //---
  private static Hashtable infos = new Hashtable();
}
