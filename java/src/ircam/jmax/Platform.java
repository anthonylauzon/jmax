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


package ircam.jmax;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import ircam.jmax.editors.patcher.PatcherFontManager;
import ircam.jmax.editors.patcher.Settings;
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
  static public int    HELP_KEY       = 0;

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
	SYSTEM = "Mac OS X";
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
    else if (SYSTEM.toLowerCase().startsWith("windows"))
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
    //setDefaultUIColors(Color.red);
  }

  static protected void setDefaultUIComponentsFont(String defFont)
  {
      Font plain = new Font(defFont, Font.PLAIN, 12);

      UIManager.put("List.font", plain);
      UIManager.put("TextArea.font", plain);
      UIManager.put("TextField.font", plain);      
      UIManager.put("Table.font", plain);
      UIManager.put("TableHeader.font", plain);

      Font bold = new Font(defFont, Font.BOLD, 12);

      UIManager.put("Button.font", bold);
      UIManager.put("CheckBox.font", bold);
      UIManager.put("CheckBoxMenuItem.font", bold);
      UIManager.put("ColorChooser.font", bold);
      UIManager.put("ComboBox.font", bold);
      UIManager.put("FileChooser.font", bold);
      UIManager.put("Frame.font", bold);
      UIManager.put("Label.font", bold);   
      UIManager.put("Menu.font", bold);
      UIManager.put("MenuBar.font", bold);
      UIManager.put("MenuItem.font", bold);
      UIManager.put("OptionPane.font", bold);
      UIManager.put("Panel.font", bold);
      UIManager.put("PopupMenu.font", bold);
      UIManager.put("RadioButton.font", bold);
      UIManager.put("RadioButtonMenuItem.font", bold);
      UIManager.put("TabbedPane.font", bold);
      UIManager.put("TitledBorder.font", bold);
      UIManager.put("ToggleButton.font", bold);
      UIManager.put("ToolBar.font", bold);
      UIManager.put("ToolTip.font", bold);
      UIManager.put("Tree.font", bold);
  }

  static protected void setDefaultUIColors(Color color)
  {
      UIManager.put("Panel.background", color);
      UIManager.put("Viewport.background", color);
      UIManager.put("MenuItem.background", color);
      UIManager.put("MenuItem.selectionBackground", color);
      UIManager.put("MenuBar.background", color);
      UIManager.put("ScrollBar.background", color);
      UIManager.put("Menu.background", color);
      UIManager.put("OptionPane.background", color);
      UIManager.put("ToolBar.background", color);
      UIManager.put("ScrollPane.background", color);
      UIManager.put("ToolBar.floatingBackground", color);
      UIManager.put("Button.background", color);
      UIManager.put("ComboBox.background", color);
      UIManager.put("ComboBox.disabledBackground", color);
      UIManager.put("ComboBox.listBackground", color);
      UIManager.put("ComboBox.selectionBackground", color);
      UIManager.put("ToolBar.dockingBackground", color);
      UIManager.put("ToggleButton.background", color);
      UIManager.put("CheckBoxMenuItem.background", color);
      UIManager.put("RadioButtonMenuItem.background", color);
      UIManager.put("RadioButton.background", color);
      UIManager.put("Separator.background", color);
      UIManager.put("Tree.textBackground", color);
      UIManager.put("Tree.background", color);
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
    ENTER_KEY     = 10;
    RETURN_KEY    = 10;
    FONT_SIZE     = 10;
    FONT_STYLE    = Font.PLAIN;
    FONT_NAME = PatcherFontManager.getInstance().getFontName("Variable Width Font");
    HELP_KEY      = KeyEvent.VK_HELP;
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
    HELP_KEY    = KeyEvent.VK_H;

    FONT_SIZE   = 10;
    FONT_STYLE  = Font.PLAIN;
    FONT_NAME   = new String("Palatino");
    
    PatcherFontManager.getInstance().setRecoveringFont(FONT_NAME);
    String[] fonts = {"SansSerif", "default", "Lucida Sans"};
    PatcherFontManager.getInstance().setToRecoverFonts(fonts);

    setDefaultUIComponentsFont(FONT_NAME);
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
    HELP_KEY    = KeyEvent.VK_H;
    FONT_SIZE   = 18;
    FONT_STYLE  = Font.PLAIN;
    FONT_NAME   = new String("monaco");
  }

  //------------------------------------------
  // setWindowsValues
  //------------------------------------------
   
  static protected void setWindowsValues() 
  {
    NEWLINE     = "\n";
    NEWLINECHAR = '\n';
    DELETE_KEY  = 8;
    BACKSPACE_KEY = 127;
    ENTER_KEY   = 10;
    RETURN_KEY  = 10;
    HELP_KEY    = KeyEvent.VK_H;
    FONT_NAME = new String("Arial");
    FONT_SIZE   = 10;
    FONT_STYLE  = Font.PLAIN;
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
    HELP_KEY    = KeyEvent.VK_H;
    FONT_NAME = PatcherFontManager.getInstance().getFontName("Variable Width Font");
    FONT_SIZE   = 10;
    FONT_STYLE  = Font.PLAIN;
  }
}
 
