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

package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

/**
 * The table selecter tool. It uses the StripeSelecter Interaction Module.
 * @see StripeSelecter.
 */
public class TableSelecter extends TableTool implements GraphicSelectionListener{
  
  /**
   * Constructor */
  public TableSelecter(ImageIcon theImageIcon)
  {
    super("selecter", theImageIcon);

    itsSelecter = new StripeSelecter(this);
  }

  /**
   * the default InteractionModule is a StripeSelecter
   */
  public InteractionModule getDefaultIM() 
  {
    return itsSelecter;
  }

  /**
   * called by the Selecter UI module at mouse down
   */
  public void selectionPointChoosen(int x, int y, int modifiers) 
  {

    if ((modifiers & InputEvent.SHIFT_MASK) == 0)
      {
	getGc().getSelection().deselectAll();
	getGc().getSelection().setCaretPosition(getGc().getAdapter().getInvX(x));
      }
  }

  /**
   * called by the selecter UI module at mouseUp.
   */
  public void selectionChoosen(int x, int y, int w, int h) 
  {
    TableAdapter a = getGc().getAdapter();

    if (w==0) return;

    getGc().getSelection().setCaretPosition(TableSelection.NO_CARET);
    getGc().getSelection().select(a.getInvX(x), a.getInvX(x+w));
  }


    /**
     * GraphicSelectionListener implementation. Selection via doubleClick */
    public void selectionPointDoubleClicked(int x, int y, int modifiers)
    {
    }


  /**
   * Selects all the objects in a given rectangle
   */
  void selectArea(int x, int y, int w, int h) 
  { 
  }

  
  //--- Fields
  StripeSelecter itsSelecter;

  Point startingPoint = new Point();

}

