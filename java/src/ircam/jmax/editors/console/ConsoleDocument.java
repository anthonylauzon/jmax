
package ircam.jmax.editors.console;

import java.io.*;
import ircam.jmax.editors.project.*;
import ircam.jmax.*;

public class ConsoleDocument implements MaxDocument {
  
  ConsoleWindow itsConsoleWindow;

  public ConsoleDocument(ConsoleWindow theConsoleWindow){
    itsConsoleWindow = theConsoleWindow;
  }

  public boolean Load(File file){return true;}
  public boolean Save(){return true;}
  public boolean SaveAs(File file){return true;}
	
  public void InitDoc(String theTitle, File theFile, Project theProject){}
  public String GetTitle(){return "jMax Console";}
  public File GetFile(){return null;}
  public String GetType(){return "";}
  public boolean GetSaveFlag(){return true;}
  public boolean GetNeverSavedFlag(){return true;}
  public MaxWindow GetWindow(){return itsConsoleWindow;}
  public void SetFile(File theFile){}
  public void ToSave(){}
}
