//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.qlist;

import java.lang.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import tcl.lang.*;
import javax.swing.*;

/**
 * A panel that is able to show the content of a FtsAtomList (qlist).
 * This component does not handle the communication with FTS, but it offers
 * a simple API (fillContent, getText) in order to be used from outside. */
public class QListPanel extends JPanel {
  
  JTextArea itsTextArea;
  int caretPosition;
  Dimension preferred = new Dimension(512, 412);

  String textToFind;
  int lastFindIndex;

  /**
   * Constructor */
  public QListPanel() 
  { 
    super();

    itsTextArea = new JTextArea(40, 40);
    itsTextArea.addKeyListener(KeyConsumer.controlConsumer());
    // SGI's JTextArea bug. See utils.KeyConsumer class for details

    setLayout(new BorderLayout());
    add(BorderLayout.CENTER, new JScrollPane(itsTextArea));
    
    caretPosition = 0;
    validate();

    textToFind = "";
    lastFindIndex = 0;

    setBackground(Color.white);
  }

  /**
   * Sets the content to the given FtsAtomList object */
  public void fillContent(FtsAtomList theContent) 
  {
    caretPosition = itsTextArea.getCaretPosition();
    itsTextArea.setText( theContent.getValuesAsText());
    itsTextArea.requestFocus();
    // (em) added a control to avoid setting impossible caret positions.
    // FtsAtomList.getValueAsText() can infact reformat the text,
    // removing CR's and then shortening its length
    if (caretPosition <= itsTextArea.getText().length())
      itsTextArea.setCaretPosition( caretPosition);
    else itsTextArea.setCaretPosition(itsTextArea.getText().length());
  }
 
  public Dimension getPreferredSize() 
  {
    return preferred;
  }

  public Dimension getMinimumSize() 
  {
    return preferred;
  }

  /**
   * part of API offered to the findDialog */
  int getCaretPosition()
  {
    return itsTextArea.getCaretPosition();
  }

  /**
   * returns the text containing in this panel. */
  public String getText()
  {
    return itsTextArea.getText();
  }

  /**
   * part of API offered to the findDialog */
  int find( String textToFind, int fromIndex)
  {
    int index = itsTextArea.getText().indexOf( textToFind, fromIndex);

    if ( index >= 0)
      {
	itsTextArea.select( index, index + textToFind.length());
      }

    return index;
  }
}



