//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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
   * Selects all the objects in a given rectangle
   */
  void selectArea(int x, int y, int w, int h) 
  { 
  }

  
  //--- Fields
  StripeSelecter itsSelecter;

  Point startingPoint = new Point();

}

