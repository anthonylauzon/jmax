package ircam.jmax;

import java.awt.*;

/**
 * The interface implemented by all the components in
 * ermes that want to be handled in the window
 * list (for example, the editors)
 */
public interface MaxWindow {
  public abstract MaxDocument GetDocument();
  public abstract Frame GetFrame();
  public abstract void InitFromDocument(MaxDocument theDocument);
  public abstract void ToFront();
  public abstract boolean Close();
  public abstract void AddWindowToMenu(String theName);
  public abstract void RemoveWindowFromMenu(String theName);
  public abstract void ChangeWinNameMenu(String theOldName, String theNewName);
  public abstract void AddToSubWindowsMenu(String theTopWinName, String theSubWinName, boolean theFirstItem);
  public abstract void RemoveFromSubWindowsMenu(String theTopWindowName,String theSubWindowName,boolean theLastItem);
}
