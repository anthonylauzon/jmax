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







