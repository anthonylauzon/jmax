package ircam.jmax.editors.ermes;
import java.awt.*;

/**
 * A thread used to flash the message box when clicked.
 */

public class ErmesObjMessThread extends Thread {
  ErmesObjMessage itsMessage = null;
  ErmesSketchPad itsSketchPad;

  public ErmesObjMessThread(ErmesSketchPad theSketch, String str) {
    super(str);
    itsSketchPad = theSketch;
  }
    
  public void run() {
    Graphics aGraphics = itsSketchPad.getGraphics();
    while(true){
      if((aGraphics != null)&&(itsMessage!=null)) {
	itsMessage.itsFlashing = true;
	itsMessage.Paint_specific(aGraphics);
	try {
	  sleep(100);
	} catch (InterruptedException e) {}
	itsMessage.itsFlashing = false;
	itsMessage.Paint_specific(aGraphics);
      }	
      suspend();
    }
  }
  
  public void SetMessage(ErmesObjMessage theMessage){
    itsMessage = theMessage;
  }

}
