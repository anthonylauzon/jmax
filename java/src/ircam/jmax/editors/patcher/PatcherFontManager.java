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

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;

public class PatcherFontManager
{
  static private PatcherFontManager fontManager = new PatcherFontManager();

  public PatcherFontManager()
  {
      initFontNameTable();
      initFontSizeTable();
  }

  public static PatcherFontManager getInstance()
  {
    return fontManager;
  }

  void initFontNameTable()
  {      
    for(int i=0; i<jmaxFontNames.length; i++)
	fontNames.put(jmaxFontNames[i], defaultFontNames[i]);
  }

  void initFontSizeTable()
  {
      for(int i=0; i<jmaxFontSizesNames.length; i++)
	  fontSizes.put(jmaxFontSizesNames[i], new Integer(defaultFontSizes[i]));
  }

  /****************************************************************************/
  /*                                                                          */
  /*           ACTIONS                                                        */
  /*                                                                          */
  /****************************************************************************/
  public String getFontName(String name)
  {
      return (String)(fontNames.get(name));
  }
  public int getFontSize(String size)
  {
      return ((Integer)fontSizes.get(size)).intValue();
  }

  public boolean isDefaultFontName(String name)
  {
      return fontNames.containsValue(name);
  }

  public String[] getJMaxFontSizes()
  {
    return jmaxFontSizesNames;
  }
  public String[] getJMaxFontNames()
  {
    return jmaxFontNames;
  }

  private Hashtable fontNames = new Hashtable();
  private Hashtable fontSizes = new Hashtable();

  private String[] jmaxFontSizesNames = {"tiny", "small", "normal", "large", "larger", "huge"};
  private String[] jmaxFontNames = {"Variable Width Font", "Fixed Width Font"};
  private String[] defaultFontNames = {"Lucida Sans", "Lucida Bright"};
  private int[] defaultFontSizes = {8, 10, 12, 14, 18, 24};
}







