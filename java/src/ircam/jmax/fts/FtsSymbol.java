//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax.fts;

/**
 * A class that is used for communicating String with/from FTS.
 * A FtsSymbol contains an interned String and a cache index.
 */

public class FtsSymbol {

  /**
   * Allocate a new FtsSymbol that contains the string argument.
   * @param s  a String
   */
  public FtsSymbol( String value)
  {
    this.value = value.intern();
    this.cacheIndex = -1;
  }

  /**
   * Returns the contained string
   * @return the contained string
   */
  public String getString()
  {
    return value;
  }

  /*
   * Tells if the symbol is cached
   * @return <code>true</code> if symbol is cached, <code>false</code> if not
   */
  protected boolean isCached()
  {
    return cacheIndex >= 0;
  }

  /*
   * Returns the cache index
   * @return the cache index
   */
  protected int getCacheIndex()
  {
    return cacheIndex;
  }

  /*
   * Sets the cache index
   * @param cacheIndex the new cache index
   */
  protected void setCacheIndex( int cacheIndex)
  {
    this.cacheIndex = cacheIndex;
  }

  private String value;
  private int cacheIndex;
}

