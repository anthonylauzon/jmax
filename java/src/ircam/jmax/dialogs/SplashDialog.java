package ircam.jmax.dialogs;

import java.awt.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;


/**
 * The initial dialog.
 */

public class SplashDialog extends Dialog
{
  static final int SPLASH_WIDTH = 500;
  static final int SPLASH_HEIGHT = 280;

  /** An inner class that is the thread used for make the splash
   * dialog go away, after a while.
   */

  class SplashDialogTimeout extends Thread
  {
    int time;

    public SplashDialogTimeout(int time)
    {
      super("SplashDialogTimeoutThread");
      this.time = time;

      run();
    }
    
    public void run()
    {
      try
	{
	  sleep(time);
	}
      catch (InterruptedException e)
	{
	}

      SplashDialog.this.hide();
      SplashDialog.this.dispose();
    }
  }

  Image itsImage;
  SplashDialogTimeout to;

  public SplashDialog(Frame dw, String filename) {
    super(dw, "ermes",/* true*/false);
    
    itsImage = Toolkit.getDefaultToolkit().getImage(filename).getScaledInstance(SPLASH_WIDTH,
										SPLASH_HEIGHT,
										Image.SCALE_DEFAULT);

    //Initialize this dialog to its preferred size.

    move(200,200);
    pack();
    show();

    to = new SplashDialogTimeout(2000);
  }
	
  public boolean mouseDown(Event evt, int x, int y) {
    hide();
    return true;
  }
  
  public boolean keyDown(Event evt,int key) {
    if (key == ircam.jmax.utils.Platform.RETURN_KEY){	
      hide();
      return true;
    }
    return false;
  }

  public Dimension minimumSize() {
    Dimension d = new Dimension(SPLASH_WIDTH, SPLASH_HEIGHT);
    return d;
  }    
  
  public Dimension preferredSize() {
    return minimumSize();	
  }
	
  
  
  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    Dimension d = size();
    g.setColor(Color.white);
    g.fillRect(0, 0, d.width, d.height);        
    g.drawImage(itsImage, /*4*/12, /*4*/20,this);
  }
}






