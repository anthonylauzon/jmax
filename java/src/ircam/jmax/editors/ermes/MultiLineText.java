//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.ermes;

import java.lang.*;
import java.util.*;
import java.awt.*;

import ircam.jmax.utils.*;

public class MultiLineText {

  private int width;
  private FontMetrics fm;
  private String text;
  private MaxVector lines;

  public MultiLineText()
  {
    width = 0;
    fm = null;
    text = null;
    lines = null;
  }

  private void adjust()
  {
    char array[] = text.toCharArray();
    int start;
    int length;

    /* Loop changed beacause it could
       generate a check on a length bigger by one
       then the length of the string (MDC); also added a time and memory optimization
       for the sigle line case (optimize if charsWidth take time, of course);
       the vector is allocated by need, and in the case of a single line
       a special enumerator is used.
       Also, modified the callers of adjust so that adjust is not called
       with 0 width or without font metrics.
       */

    if ((array.length <= 1) ||
	((text.indexOf('\n') == -1) && fm.charsWidth( array, 0, array.length) <= width))
      {
	lines = null;
	return;
      }

    // Start point to the start of an ideal current string
    // of length length; the loop try to make the string longer

    if (lines == null)
      lines = new MaxVector();
    else
      lines.removeAllElements();

    start = 0;
    length = 1;

    while (true)
      {
	if (start + length >= array.length)
	  {
	    // String end, cannot make bigger
	    // generate the current string and return

	    if (start < array.length)
	      lines.addElement( new String( array, start, array.length - start));

	    return;
	  }
	else if (array[ start + length] == '\n')
	  {
	    // Next char is a new line, so generate the current string
	    // and skip the new line

	    lines.addElement( new String( array, start, length));
	    start = start + length + 1;
	    length = 1;
	  }
	else if (fm.charsWidth( array, start, length + 1) >= width)
	  {
	    // Next char make the string too long, 
	    // generate the current string and advance

	    lines.addElement( new String( array, start, length));
	    start = start + length;
	    length = 1;
	  }
	else
	  {
	    // We can make the current string longer
	       
	    length++;
	  }
      }
  }

  private void debug()
  {
    //System.err.println( "*** width=" + width + " fm=" + fm + " text=" + text + " ***");
  }

  public void setWidth( int width)
  {
    this.width = width;

    debug();

    if ((fm != null) && (text != null) && (width != 0))
      adjust();
  }

  public void setFontMetrics( FontMetrics fm)
  {
    this.fm = fm;

    debug();

    if ((fm != null) && (text != null) && (width != 0))
      adjust();
  }

  public void setText( String text)
  {
    this.text = text;

    debug();

    if ((fm != null) && (text != null) && (width != 0))
      adjust();
  }

  public int getRows()
  {
    if (lines != null)
      return lines.size();
    else
      return 1;
  }

  final class MultiLineTextEnumerator implements Enumeration {

    MultiLineTextEnumerator()
    {
      elements = lines.elements();
    }

    public boolean hasMoreElements()
    {
      return elements.hasMoreElements();
    }

    public Object nextElement()
    {
      return elements.nextElement();
    }

    private Enumeration elements;
  }


  final class MultiLineSimpleEnumerator implements Enumeration {

    MultiLineSimpleEnumerator()
    {
      more = true;
    }

    public boolean hasMoreElements()
    {
      return more;
    }

    public Object nextElement()
    {
      more = false;
      return text;
    }

    private boolean more;
  }

  public Enumeration elements()
  {
    if (lines == null)
      return new MultiLineSimpleEnumerator();
    else
      return new MultiLineTextEnumerator();
  }
}

