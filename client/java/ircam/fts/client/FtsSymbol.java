//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
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

package ircam.fts.client;

import java.io.*;
import java.util.*;

public class FtsSymbol implements Serializable{

  public static FtsSymbol get( String s)
  {
    FtsSymbol symbol = (FtsSymbol)symbolTable.get( s);

    if (symbol != null)
      return symbol;

    symbol = new FtsSymbol( s);
    symbolTable.put( s, symbol);

    return symbol;
  }

  private FtsSymbol( String s)
  {
    this.s = s;
  }

  public String toString()
  {
    return s;
  }

  private static HashMap symbolTable = new HashMap();

  private String s;
}
