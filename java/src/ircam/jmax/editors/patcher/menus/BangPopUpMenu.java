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

package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.patcher.actions.*;

/** Implement the patcher editor File Menu */

public class BangPopUpMenu extends JMenu
{
  static private BangPopUpMenu bangPopup = new BangPopUpMenu();
  JLabel flashLabel;
  JSlider flashSlider;
  Bang current;

  public BangPopUpMenu()
  {
    super("Bang");

    add(ColorPopUpMenu.getInstance());
    
    JMenu flashMenu = new JMenu("Flash Duration");

    flashLabel = new JLabel(" 125 ", JLabel.CENTER);
    flashLabel.setAlignmentX(Component.CENTER_ALIGNMENT);

    flashSlider = new JSlider(JSlider.VERTICAL, 10, 500, 125);
    flashSlider.setMajorTickSpacing(100);
    flashSlider.setMinorTickSpacing(10);
    flashSlider.setBorder(BorderFactory.createEmptyBorder(0,0,0,10));
    flashSlider.setPaintTicks(true);
    flashSlider.addChangeListener(new ChangeListener(){
	public void stateChanged(ChangeEvent e) {
	    JSlider source = (JSlider)e.getSource();
	    int flash = (int)source.getValue();
	    
	    if(!source.getValueIsAdjusting())
		{
		    if(current!=null)
			((FtsIntValueObject)current.getFtsObject()).setFlashDuration(flash);

		}

	    flashLabel.setText(""+flash);
	}
    });
    Box flashBox = new Box(BoxLayout.Y_AXIS);
    flashBox.add(flashSlider);
    flashBox.add(flashLabel);
    flashBox.validate();

    flashMenu.add(flashBox);
    add(flashMenu);
  }

  public static BangPopUpMenu getInstance()
  {
    return bangPopup;
  }

  public static void update(Bang obj)
  {
    int flash = ((FtsIntValueObject)obj.getFtsObject()).getFlashDuration();
    bangPopup.current = obj;

    bangPopup.flashSlider.setValue(flash);
  }
}








