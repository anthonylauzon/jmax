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
    itsSplashDialog.setLocation(200,200);
    itsSplashDialog.pack();
    itsSplashDialog.setVisible(true);
    try {
      sleep(2000);
    } catch (InterruptedException e) {}
    itsSplashDialog.setVisible(false);
    itsSplashDialog.dispose();
  }
}

