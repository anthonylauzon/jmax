package ircam.jmax.editors.ermes;

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
  }

  public static Settings sharedInstance()
  {
    return singleInstance;
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
}
