package ircam.jmax.dialogs;
import java.awt.*;

/**
 * A thread used for make the splash dialog go away, after a while.
 */

public class SplashDialogThread extends Thread {
  Frame itsFrame;
  SplashDialog itsSplashDialog;

  public SplashDialogThread(Frame theFrame, String str) {
    super(str);
    itsFrame = theFrame;
  }
    
  public void run() {
    itsSplashDialog = new SplashDialog(itsFrame);
    itsSplashDialog.move(200,200);
    itsSplashDialog.pack();
    itsSplashDialog.show();
    try {
      sleep(2000);
    } catch (InterruptedException e) {}
    itsSplashDialog.hide();
    itsSplashDialog.dispose();
  }
}
