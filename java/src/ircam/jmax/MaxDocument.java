package ircam.jmax;

import java.io.*;

/**
 * The interface implemented by a generic Ermes Document.
 * (Load, Save, SaveAs, saved flag...).
 */

public interface MaxDocument
{
  /** 
   * The document's load method.
   */

  abstract boolean Load(File file);

  /*
   * The document's save.
   */ 
  abstract boolean Save();	

  abstract public void InitDoc(String theTitle, File file/*, Project theProject*/);

  abstract public String GetType();

  abstract public String GetTitle();

  abstract public File GetFile();
  
  abstract public boolean GetSaveFlag();

  abstract public boolean GetNeverSavedFlag();

  abstract public MaxWindow GetWindow();
  
  abstract public void SetFile(File theFile);
  
  /**
   * The document needs to be saved.
   */

  abstract public void ToSave();
}









