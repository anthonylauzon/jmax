//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.image.*;

public class Settings {

  private Color errorColor;
  private Color editBackgroundColor;
  private Color lockBackgroundColor;
  private Color UIColor;
  private Color objColor;
  private Color selectedColor;
  private Color textColor;
  private Color selectedTextColor;
  private Color consoleBackgroundColor;
  private Color consoleForegroundColor;

  private static Settings singleInstance;

  static
  {
    singleInstance = new Settings();
  }

  public Settings()
  {
    Toolkit.getDefaultToolkit().sync();
    UIColor = new Color( 173, 224, 255);
    objColor = new Color( 153, 204, 204);
    editBackgroundColor = new Color( 230, 230, 230);
    lockBackgroundColor = Color.white;
    errorColor = Color.red;
    selectedColor = new Color( 51, 153, 204);
    textColor = Color.black;
    selectedTextColor = Color.black;
    consoleBackgroundColor = Color.white;
    consoleForegroundColor = Color.black;
  }

  public static Settings sharedInstance()
  {
    return singleInstance;
  }

  /* merry Xmas */
  public void setCoolColors()
  {
      editBackgroundColor = Color.yellow;
      lockBackgroundColor = Color.blue.darker();
      consoleBackgroundColor = Color.magenta.darker();
      consoleForegroundColor = Color.white;
      objColor = Color.pink;
      UIColor = new Color( 153, 204, 204);
      selectedColor = Color.orange;
  }

  // ----------------------------------------
  // UIColor property
  // ----------------------------------------
  public Color getUIColor()
  {
    return UIColor;
  }

  public void setUIColor( Color UIColor)
  {
    this.UIColor = UIColor;
  }

  // ----------------------------------------
  // ObjColor
  // ----------------------------------------
  public Color getObjColor()
  {
    return objColor;
  }

  public void setObjColor( Color objColor)
  {
    this.objColor = objColor;
  }

  // ----------------------------------------
  // EditBackgroundColor
  // ----------------------------------------
  public Color getEditBackgroundColor()
  {
    return editBackgroundColor;
  }

  public void setEditBackgroundColor( Color editBackgroundColor)
  {
    this.editBackgroundColor = editBackgroundColor;
  }

  // ----------------------------------------
  // LockBackgroundColor
  // ----------------------------------------
  public Color getLockBackgroundColor()
  {
    return lockBackgroundColor;
  }

  public void setLockBackgroundColor( Color lockBackgroundColor)
  {
    this.lockBackgroundColor = lockBackgroundColor;
  }

  // ----------------------------------------
  // ErrorColor
  // ----------------------------------------
  public Color getErrorColor()
  {
    return errorColor;
  }

  public void setErrorColor( Color errorColor)
  {
    this.errorColor = errorColor;
  }

  // ----------------------------------------
  // SelectedColor
  // ----------------------------------------
  public Color getSelectedColor()
  {
    return selectedColor;
  }

  public void setSelectedColor( Color selectedColor)
  {
    this.selectedColor = selectedColor;
  }

  // ----------------------------------------
  // TextColor
  // ----------------------------------------
  public Color getTextColor()
  {
    return textColor;
  }

  public void setTextColor( Color textColor)
  {
    this.textColor = textColor;
  }

  // ----------------------------------------
  // SelectedTextColor
  // ----------------------------------------
  public Color getSelectedTextColor()
  {
    return selectedTextColor;
  }

  public void setSelectedTextColor( Color selectedTextColor)
  {
    this.selectedTextColor = selectedTextColor;
  }

    public Color getConsoleBackgroundColor()
    {
	return consoleBackgroundColor;
    }
    public Color getConsoleForegroundColor()
    {
	return consoleForegroundColor;
    }
}
