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

  /** This method, called only once at initialization, 
   * try to handle the indexed color map problem (missing colors),
   * implementing an alternative color matching and using 
   * the available colors in the current colormap */
  private Color findBestUIColorMatch(int r, int g, int b)
  {
    Toolkit toolkit = Toolkit.getDefaultToolkit();
    ColorModel cm = toolkit.getColorModel();

    if ( cm instanceof DirectColorModel)
      return new Color(r, g, b);
    else
      {
	IndexColorModel icm = (IndexColorModel) cm;

	int TOLERANCE = 15;


	// find the first color whose blue component is equal or darker
	// then the g and r, trying to conserve the luminance...
	for (int i = 0; i < icm.getMapSize(); i++)
	  {
	    if (Math.abs(icm.getRed(i) - r) <= TOLERANCE && 
		Math.abs(icm.getGreen(i) - g) <= TOLERANCE && 
		Math.abs(icm.getBlue(i) - b) <= TOLERANCE)
	      {
		return new Color(icm.getRed(i), icm.getGreen(i), icm.getBlue(i));
	      }
	  }
      }
    //no way: let the system's color matching do the job
    return new Color(r, g, b);
  }
}
