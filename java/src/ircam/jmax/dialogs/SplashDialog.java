package ircam.jmax.dialogs;

import java.awt.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;


/**
 * The initial dialog.
 */
public class SplashDialog extends Dialog {
  Frame parent;
  Image itsImage;
  Button b;
  
  public SplashDialog(Frame dw) {
    super(dw, "ermes",/* true*/false);
    parent = dw;
    
    Toolkit toolkit = Toolkit.getDefaultToolkit();
    String fs = MaxApplication.ermesProperties.getProperty("file.separator");
    String imagesPath = MaxApplication.ermesProperties.getProperty("root")+fs+"ermes"+fs+"images";
    itsImage = toolkit.getImage(imagesPath+fs+"Splash.gif");
    //Initialize this dialog to its preferred size.
    pack();
  }
	
  /* public boolean action(Event event, Object arg) {
     hide();
     return true;
  }*/

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
    Dimension d = new Dimension(500, 280);
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






