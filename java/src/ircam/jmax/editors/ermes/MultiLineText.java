package ircam.jmax.editors.frobber;

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
    text = "";
    lines = new MaxVector();
  }

  private void adjust()
  {
    char array[] = text.toCharArray();
    int i, start = 0;

    lines.removeAllElements();

    for ( i = 0; i < array.length; i++)
      {
	if ( array[i] == '\n')
	  {
	    lines.addElement( new String( array, start, i - start));
	    start = i + 1;
	  }
	else if (fm != null && fm.charsWidth( array, start, i - start + 1) >= width)
	  {
	    lines.addElement( new String( array, start, i - start));
	    start = i;
	  }
      }

    if ( i > start || i == 0)
      {
	lines.addElement( new String( array, start, i - start));
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

    adjust();
  }

  public void setFontMetrics( FontMetrics fm)
  {
    this.fm = fm;

    debug();

    adjust();
  }

  public void setText( String text)
  {
    this.text = text;

    debug();

    adjust();
  }

  public int getRows()
  {
    return lines.size();
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

  public Enumeration elements()
  {
    return new MultiLineTextEnumerator();
  }
}
