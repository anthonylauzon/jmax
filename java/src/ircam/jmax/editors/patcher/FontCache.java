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
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


// Now cache on two index, name and size; all the fonts are created
// and stored in this class
// Not as fast as it can be ... for the moment, because make just a linear
// search in the font table

public class FontCache {

  static MaxVector fontTable = new MaxVector();
  static MaxVector fontMetricsTable  = new MaxVector();


  private static int lookupFontIndex( String fontName, int fontSize, int fontStyle)
  {
    Object[] objects = fontTable.getObjectArray();
    int osize = fontTable.size();

    for ( int i = 0; i < osize; i++)
      {
	Font font = (Font) objects[i];

	if ((fontSize == font.getSize()) &&
	    (fontName.equals(font.getName()))&&
	    (fontStyle == font.getStyle()))
	  return i;
      }
		
    return -1;
  }
  

  public static final Font lookupFont( String fontName, int fontSize, int fontStyle)
  {
    int idx;

    idx = lookupFontIndex(fontName, fontSize, fontStyle);

    if (idx < 0)
      {
	// Not found, make and store a new one

	Font font = new Font( fontName, fontStyle, fontSize);

	fontTable.addElement(font);
	fontMetricsTable.addElement(Toolkit.getDefaultToolkit().getFontMetrics( font));

	return font;
      }
    else
      return (Font) fontTable.elementAt(idx);
  }


  public static final FontMetrics lookupFontMetrics( String fontName, int fontSize, int fontStyle)
  {
    int idx;

    idx = lookupFontIndex(fontName, fontSize, fontStyle);

    if (idx < 0)
      {
	// Not found, make and store a new one

	  Font font = new Font( fontName, /*Font.PLAIN*/fontStyle, fontSize);
	FontMetrics fontMetrics = Toolkit.getDefaultToolkit().getFontMetrics( font);
	fontTable.addElement(font);
	fontMetricsTable.addElement(fontMetrics);

	return fontMetrics;
      }
    else
      return (FontMetrics) fontMetricsTable.elementAt(idx);
  }
}










