//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;
import javax.swing.*;

/**
 * The initial dialog.
 */

public class SplashDialog extends Window implements KeyListener, MouseListener, /*ActionListener ,*/ ComponentListener{
  static final int SPLASH_WIDTH = 400;
  static final int SPLASH_HEIGHT = 300;

  String itsVersionString;
  Image itsImage;

  public SplashDialog(String filename, String version) {

    super(new Frame());

    itsVersionString = version;
    itsImage = Toolkit.getDefaultToolkit().getImage(filename);
        
    addKeyListener(this);
    addMouseListener(this);
    
    Dimension screenDim = Toolkit.getDefaultToolkit().getScreenSize();
    setLocation((screenDim.width - SPLASH_WIDTH) / 2, (screenDim.height - SPLASH_HEIGHT) / 2);

    pack();
    setVisible(true);
    
    //Timer aTimer = new Timer(4000, this);
    //aTimer.setRepeats(false);
    //aTimer.start();
  }
	
    /*public void actionPerformed(ActionEvent e) {
      setVisible(false);
      itsImage = null;
      dispose();
      Timer aTimer = (Timer) e.getSource();
      aTimer.stop();
      }*/

   void disposeSplash()
   {
       setVisible(false);
       itsImage = null;
       dispose();
       MaxApplication.splash = null;
   }

  //////////////////////////////////////////////////////////mouseListener--inizio
  public void mouseClicked(MouseEvent e){}
  public void mousePressed(MouseEvent e){
    setVisible(false);
  }
  public void mouseReleased(MouseEvent e){}
  public void mouseEntered(MouseEvent e){}
  public void mouseExited(MouseEvent e){}

  //////////////////////////////////////////////////keyListener
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}  
  public void keyPressed(KeyEvent e){
    setVisible(false);
  }
  ///////////////////////////////////////////////////ComponentListener
  
  public void componentHidden(ComponentEvent e){}
  public void componentShown(ComponentEvent e)
  {
      disposeSplash();
  }
  public void componentMoved(ComponentEvent e){}
  public void componentResized(ComponentEvent e){}
  ///////////////////////////////////////////////////////////

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

    g.drawImage(itsImage, 0, 0, this);

    g.setColor(Color.black);
    g.drawString(itsVersionString, 20, 20);
  }
}
