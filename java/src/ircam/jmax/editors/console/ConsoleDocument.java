
package ircam.jmax.editors.console;
import ircam.jmax.editors.project.*;
import ircam.jmax.*;

public class ConsoleDocument implements MaxDocument {
  
  ConsoleWindow itsConsoleWindow;

  public ConsoleDocument(ConsoleWindow theConsoleWindow){
    itsConsoleWindow = theConsoleWindow;
  }

  public boolean Load(String theWholeName){return true;}
  public boolean Save(){return true;}
  public boolean SaveAs(String theWholeName){return true;}
	
  public void InitDoc(String theTitle, String thePath, String thefName, Project theProject){}
  public String GetWholeName(){return "";}
  public String GetName(){return "";}
  public String GetType(){return "";}
  public String GetPath(){return "";}
  public boolean GetSaveFlag(){return true;}
  public boolean GetNeverSavedFlag(){return true;}
  public MaxWindow GetWindow(){return itsConsoleWindow;}
  public void SetFileName(String theName){}
  public void SetPathName(String theName){}
  public void ToSave(){}
}
