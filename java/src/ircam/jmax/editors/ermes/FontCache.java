package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;

// (fd) Trying to fix the font mess !!!

class FontCache {

  private static String defaultFontName = null;

  private static String getDefaultFontName()
  {
    if (defaultFontName == null)
      {
	defaultFontName = MaxApplication.getProperty("jmaxDefaultFont");

	if (defaultFontName == null)
	  defaultFontName = "SansSerif";
      }

    return defaultFontName;
  }

  public static final Font lookupFont( int fontSize)
  {
    // DEBUG
    //accessCount++;

    if ( 0 <= fontSize && fontSize < DEFAULT_FONT_CACHE_LENGTH)
      {
	if ( fontTable[fontSize] != null)
	  {
	    return fontTable[fontSize];

	    // DEBUG
	    //hitCount++;
	  }
	else
	  {
	    Font font = new Font( getDefaultFontName(), Font.PLAIN, fontSize);

	    fontTable[fontSize] = font;
	    fontMetricsTable[fontSize] = dummyPanel.getFontMetrics( font);

	    return font;
	  }
      }
    else
      {
	return new Font( getDefaultFontName(), Font.PLAIN, fontSize);
      }

    // DEBUG
    //if (accessCount % 8 == 0)
    //  System.err.println( "FontCache[" + (100.0*hitCount)/accessCount + "% hit rate]");
  }

  public static final FontMetrics lookupFontMetrics( int fontSize)
  {
    // DEBUG
    //accessCount++;

    if ( 0 <= fontSize && fontSize < DEFAULT_FONT_CACHE_LENGTH)
      {
	if ( fontMetricsTable[fontSize] != null)
	  {
	    return fontMetricsTable[fontSize];

	    // DEBUG
	    //hitCount++;
	  }
	else
	  {
	    Font font = new Font( getDefaultFontName(), Font.PLAIN, fontSize);
	    FontMetrics fontMetrics = dummyPanel.getFontMetrics( font);

	    fontTable[fontSize] = font;
	    fontMetricsTable[fontSize] = fontMetrics;

	    return fontMetrics;
	  }
      }
    else
      {
	Font font = new Font( getDefaultFontName(), Font.PLAIN, fontSize);
	return dummyPanel.getFontMetrics( font);
      }

    // DEBUG
    //if (accessCount % 8 == 0)
    //  System.err.println( "FontCache[" + (100.0*hitCount)/accessCount + "% hit rate]");
  }

  private static Font fontTable[];
  private static FontMetrics fontMetricsTable[];
  // A dummy component to get FontMetrics from
  private static Panel dummyPanel;  

  private static final int DEFAULT_FONT_CACHE_LENGTH = 32;

  // DEBUG
  //private static int hitCount = 0;
  //private static int accessCount = 0;

  static
  {
    fontTable = new Font[ DEFAULT_FONT_CACHE_LENGTH];
    fontMetricsTable = new FontMetrics[ DEFAULT_FONT_CACHE_LENGTH];

    dummyPanel = new Panel();

    int preferredFontSizes[] = { 8, 10, 12, 14, 16, 18, 20, 24};

    for ( int i = 0; i < preferredFontSizes.length; i++)
      {
	int fontSize = preferredFontSizes[i];

	Font font = new Font( getDefaultFontName(), Font.PLAIN, fontSize);
	fontTable[fontSize] = font;
	fontMetricsTable[fontSize] = dummyPanel.getFontMetrics( font);
      }
  }
}

