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
package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;


// Now cache on two index, name and size; all the fonts are created
// and stored in this class
// Not as fast as it can be ... for the moment, because make just a linear
// search in the font table

public class FontCache {

  static MaxVector fontTable = new MaxVector();
  static MaxVector fontMetricsTable  = new MaxVector();


  private static int lookupFontIndex( String fontName, int fontSize)
  {
    Object[] objects = fontTable.getObjectArray();
    int osize = fontTable.size();

    for ( int i = 0; i < osize; i++)
      {
	Font font = (Font) objects[i];

	if ((fontSize == font.getSize()) &&
	    (fontName.equals(font.getName())))
	  return i;
      }
		
    return -1;
  }
  

  public static final Font lookupFont( String fontName, int fontSize)
  {
    int idx;

    idx = lookupFontIndex(fontName, fontSize);

    if (idx < 0)
      {
	// Not found, make and store a new one

	Font font = new Font( fontName, Font.PLAIN, fontSize);

	fontTable.addElement(font);
	fontMetricsTable.addElement(Toolkit.getDefaultToolkit().getFontMetrics( font));

	return font;
      }
    else
      return (Font) fontTable.elementAt(idx);
  }


  public static final FontMetrics lookupFontMetrics( String fontName, int fontSize)
  {
    int idx;

    idx = lookupFontIndex(fontName, fontSize);

    if (idx < 0)
      {
	// Not found, make and store a new one

	Font font = new Font( fontName, Font.PLAIN, fontSize);
	FontMetrics fontMetrics = Toolkit.getDefaultToolkit().getFontMetrics( font);
	fontTable.addElement(font);
	fontMetricsTable.addElement(fontMetrics);

	return fontMetrics;
      }
    else
      return (FontMetrics) fontMetricsTable.elementAt(idx);
  }
}

