package ircam.jmax;

import  ircam.jmax.editors.project.*;

/**
 * The interface implemented by a generic Ermes Document.
 * (Load, Save, SaveAs, saved flag...).
 */
/*prova prova abstract class*/public interface MaxDocument {
	/*prova prova protected String itsFileName = "";
	 protected String itsPathName = "";
	 protected String itsDocumentType = "";
	 protected boolean alreadySaved =false;*/
	
 /** 
 * The document's load method.
 */
  abstract boolean Load(String theWholeName);
  /*
   * The document's save.
   */ 
 abstract boolean Save();	
  abstract boolean SaveAs(String theWholeName);
	
  abstract public void InitDoc(String theTitle, String thePath, String thefName, Project theProject);
  /*prova prova public String GetWholeName() {
    return itsFileName + itsPathName;
    }*/abstract public String GetWholeName();
  
  /*prova prova public String GetName() {
    return itsFileName; 
    }*/abstract public String GetName();
  
  /*prova prova public String GetType() {
    return itsDocumentType; 
    }*/abstract public String GetType();
  
  /*prova prova public String GetPath() {
    return itsPathName; 
    }*/abstract public String GetPath();
  
  /*prova prova public boolean GetSaveFlag() {
    return alreadySaved;
    }*/abstract public boolean GetSaveFlag();

  abstract public boolean GetNeverSavedFlag();

  abstract public MaxWindow GetWindow();
  
  /*prova prova protected void SetFileName(String theName) {
    itsFileName = theName;
    }*/abstract public void SetFileName(String theName);
  
  /*prova prova protected void SetPathName(String theName) {
    itsPathName = theName;
    }*/abstract public void SetPathName(String theName);

  /**
   * The document needs to be saved.
   */
  abstract public void ToSave();
}









