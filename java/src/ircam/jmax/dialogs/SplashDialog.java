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

// import javax.swing.*;
import javax.swing.Timer;

/**
 * The initial dialog.
 */

public class SplashDialog extends Window {

  private static final int SPLASH_SHOW_DURATION = 4000;
  private static final int SPLASH_WIDTH = 400;
  private static final int SPLASH_HEIGHT = 300;

  public SplashDialog( String filename, String version) 
  {
    super(new Frame());

    itsVersionString = version;
    itsImage = Toolkit.getDefaultToolkit().getImage(filename);
        
    Dimension screenDim = Toolkit.getDefaultToolkit().getScreenSize();
    setLocation((screenDim.width - SPLASH_WIDTH) / 2, (screenDim.height - SPLASH_HEIGHT) / 2);

    pack();
    setVisible(true);
    
    timer = new Timer( SPLASH_SHOW_DURATION, new ActionListener() {
	public void actionPerformed( ActionEvent e)
	{
	  disposeSplash();
	}
      });
    timer.setRepeats(false);
    timer.start();

    addKeyListener( new KeyAdapter() {
	public void keyPressed(KeyEvent e){
	  disposeSplash();
	}
      });
    addMouseListener( new MouseAdapter() {
	public void mousePressed(MouseEvent e){
	  disposeSplash();
	}
      });
  }
	
  private void disposeSplash()
  {
    setVisible(false);
    itsImage = null;
    dispose();
    timer.stop();
  }

  public Dimension getMinimumSize() 
  {
    return new Dimension(SPLASH_WIDTH, SPLASH_HEIGHT);
  }    
  
  public Dimension getPreferredSize() 
  {
    return getMinimumSize();	
  }
  
  public void paint(Graphics g) 
  {
    Dimension d = getSize();

    g.drawImage(itsImage, 0, 0, this);

    g.setColor(Color.black);
    g.drawString(itsVersionString, 20, 20);
  }

  private String itsVersionString;
  private Image itsImage;
  private Timer timer;
}
