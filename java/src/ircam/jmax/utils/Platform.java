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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 


package ircam.jmax.utils;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.editors.patcher.PatcherFontManager;

//=====================================================
//   Platform 
//=====================================================

public class Platform
{
  static public String NEWLINE        = null;
  static public char   NEWLINECHAR    = 0;
  static public int    DELETE_KEY     = 0;
  static public int    BACKSPACE_KEY  = 0;	//to check in all the platforms
  static public int    ENTER_KEY      = 0;
  static public int    RETURN_KEY     = 0;
  static public int    FONT_SIZE      = 0;
  static public int    FONT_STYLE     = 0;
  static public String FONT_NAME      = null;
  static public int    LEFT_KEY       = KeyEvent.VK_LEFT;
  static public int    RIGHT_KEY      = KeyEvent.VK_RIGHT;
  static public int    UP_KEY         = KeyEvent.VK_UP;
  static public int    DOWN_KEY       = KeyEvent.VK_DOWN;

  static public String  SYSTEM     = null;
  static public boolean INAPPLET   = false;



  //------------------------------------------
  // setValues
  //------------------------------------------
   
  static public void setValues()
  { 
    if (!INAPPLET)
      {
	SYSTEM =  (String)(System.getProperties().getProperty("os.name"));
      }
    else if (SYSTEM == null)
      {
	SYSTEM = "unknown";
      }

    if (SYSTEM.equalsIgnoreCase("Mac OS X"))
      {
	setMacValues();
      }
    else if (SYSTEM.equals("Irix"))
      {
	setIrixValues();
      }
    else if (SYSTEM.equals("DEC"))
      {
	setIrixValues();
      }  
    else if (SYSTEM.equals("windows"))
      {
	setWindowsValues();
      }  
    else if (SYSTEM.equals("Linux"))
      {
	setLinuxValues();
      }  
    else
      {
	setDefaultValues();
      }
  }

  //------------------------------------------
  // setMacValues
  //------------------------------------------
   
  static protected void setMacValues()
  {
    NEWLINE       = "\n";
    NEWLINECHAR   = '\n';
    DELETE_KEY    = 8;
    BACKSPACE_KEY = 127;
    ENTER_KEY     = /*3*/10;
    RETURN_KEY    = 10;
    FONT_SIZE     = /*9*/10;
    FONT_STYLE    = Font.PLAIN;
    //FONT_NAME     = new String("monaco");
    FONT_NAME = PatcherFontManager.getInstance().getFontName("Variable Width Font");
  }

  //------------------------------------------
  // setIrixValues
  //------------------------------------------
   
  static protected void setIrixValues()
  {
    NEWLINE     = "\n";
    NEWLINECHAR = '\n';
    DELETE_KEY  = 127;
    BACKSPACE_KEY = 8;
    LEFT_KEY    = 37;
    UP_KEY      = 38;
    RIGHT_KEY   = 39;
    DOWN_KEY    = 40;
    ENTER_KEY   = 10;
    RETURN_KEY  = 10;
    FONT_SIZE   = 10;
    FONT_STYLE  = Font.PLAIN;
    FONT_NAME   = new String("SansSerif");
  }

  //------------------------------------------
  // setDECValues
  //------------------------------------------
   
  static protected void setDECValues()
  {
    NEWLINE     = "\n";
    NEWLINECHAR = '\n';
    DELETE_KEY  = 8;
    ENTER_KEY   = 10;
    RETURN_KEY  = 10;
    FONT_SIZE   = 18;
    FONT_STYLE  = Font.PLAIN;
    FONT_NAME   = new String("monaco");
  }

  //------------------------------------------
  // setWindowsValues
  //------------------------------------------
   
  static protected void setWindowsValues() 
  {
    setDefaultValues();
  }

  //------------------------------------------
  // setLinuxValues
  //------------------------------------------
   
  static protected void setLinuxValues()
  {
    setDefaultValues();
  }
        
  //------------------------------------------
  // setDefaultValues
  //------------------------------------------
   
  static protected void setDefaultValues()
  {
    NEWLINE     = "\n";
    NEWLINECHAR = '\n';
    DELETE_KEY  = 8;
    BACKSPACE_KEY = 127;
    ENTER_KEY   = 10;
    RETURN_KEY  = 10;
    //FONT_NAME = new String("default");
    FONT_NAME = PatcherFontManager.getInstance().getFontName("Variable Width Font");
    FONT_SIZE   = 10;
    FONT_STYLE  = Font.PLAIN;
  }
}
 




