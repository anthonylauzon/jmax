package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;
import com.sun.java.swing.Timer;

/**
 * The initial dialog.
 */

public class SplashDialog extends Dialog implements KeyListener, MouseListener, ActionListener{
  static final int SPLASH_WIDTH = 500;
  static final int SPLASH_HEIGHT = 280;
  String itsVersionString;

  /** An inner class that is the thread used for make the splash
   * dialog go away, after a while.
   */

  /*2003  class SplashDialogTimeout extends Thread
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
    
    SplashDialog.this.setVisible(false);
    SplashDialog.this.dispose();
    }
    }*/
  
  Image itsImage;
  //2003SplashDialogTimeout to;
  

  public SplashDialog(Frame dw, String filename, String version) {
    super(dw, "jMax",/* true*/false);
    
    itsVersionString = version;
    itsImage = Toolkit.getDefaultToolkit().getImage(filename);
    
    
    addKeyListener(this);
    addMouseListener(this);
    
    setLocation(200,200);
    pack();
    setVisible(true);
    
    //2003to = new SplashDialogTimeout(2000);
    //start2003
    Timer aTimer = new Timer(2000, this);
    //aTimer.setInitialDelay(2000);
    aTimer.setRepeats(false);
    aTimer.start();
    //end 2003
  }
	
  public void actionPerformed(ActionEvent e) {
    setVisible(false);
    dispose();
    Timer aTimer = (Timer) e.getSource();
    aTimer.stop();
  }
  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--inizio
  public void mouseClicked(MouseEvent e){}
  
  public void mousePressed(MouseEvent e){
    setVisible(false);
  }
  public void mouseReleased(MouseEvent e){}
  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}
  /////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////mouseListener--fine
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  
  public void keyPressed(KeyEvent e){
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY) setVisible(false);
  }
  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--fine

  public Dimension getMinimumSize() {
    Dimension d = new Dimension(SPLASH_WIDTH, SPLASH_HEIGHT);
    return d;
  }    
  
  public Dimension getPreferredSize() {
    return getMinimumSize();	
  }
  
  //--------------------------------------------------------
  //	paint
  //--------------------------------------------------------
  public void paint(Graphics g) {
    Dimension d = getSize();
    g.setColor(Color.white);
    g.fillRect(0, 0, d.width, d.height);        
    g.drawImage(itsImage, /*4*/12, /*4*/20,this);
    g.setColor(Color.black);
    g.drawString(itsVersionString, 24, d.height-24);
  }
}
















