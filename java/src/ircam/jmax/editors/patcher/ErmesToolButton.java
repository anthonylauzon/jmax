
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

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.awt.image.*;
import java.io.*;
import ircam.jmax.*;
import ircam.jmax.toolkit.*;

import ircam.jmax.editors.patcher.*;

class ErmesToolButton extends JToggleButton
{
  String description;
  String message;
  ErmesToolBar  toolBar;
  boolean state = false;

  static MouseListener mListener = new MouseListener(){
    public void mouseClicked(MouseEvent e){
    } 
    public void mouseEntered(MouseEvent e){
    } 
    public void mouseExited(MouseEvent e){
    } 
    public void mousePressed(MouseEvent e){
      ErmesToolButton button = (ErmesToolButton)e.getSource();
      if(!button.isSelected()){
	  button.setCursor(Cursors.get(button.getDescription()));
	  button.state = true;
	  button.setSelected(true);
	  button.toolBar.buttonSelected( button);
      }
    } 
    public void mouseReleased(MouseEvent e){
    }     
  };

  ErmesToolButton(ErmesToolBar  toolBar, String description, ImageIcon theIcon, String cursorName, String message)
  {
    super( theIcon);

    setDoubleBuffered( false);
    setMargin( new Insets(0,0,0,0));
    this.description = description;
    this.message = message;
    this.toolBar = toolBar;
    
    if(Cursors.get(description) == null)
    {
	ImageObserver observer =  new ImageObserver(){
		public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height)
		{
		    return true;
		}
	    };	
	Image image = Toolkit.getDefaultToolkit().getImage(cursorName);
	Toolkit.getDefaultToolkit().prepareImage(image, -1, -1, observer);
	Dimension bestSize = Toolkit.getDefaultToolkit().getBestCursorSize(image.getWidth(observer), image.getHeight(observer));
	BufferedImage bi = new BufferedImage(bestSize.width, bestSize.height, BufferedImage.TYPE_INT_ARGB);
	Toolkit.getDefaultToolkit().prepareImage(bi, bestSize.width, bestSize.height, observer);
	bi.createGraphics().drawImage(image, 0, 0, observer);

	if((image.getWidth(observer)>0)&&(image.getHeight(observer)>0))
	    {
		Cursor cursor = Toolkit.getDefaultToolkit().createCustomCursor(bi, new Point(0,1), description+" cursor");    
		Cursors.loadCursor(description, cursor);
	    }
    }    
    addMouseListener(ErmesToolButton.mListener);
  }

  void reset(){
    state = false;
    setCursor( Cursor.getDefaultCursor());
  }

  String getDescription()
  {
    return description;
  }

  String getMessage()
  {
    return message;
  }
}







