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

package ircam.jmax.editors.configuration;

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.util.*;

// import javax.swing.*;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

public class AudioConfigPanel extends JPanel implements Editor
{
  public AudioConfigPanel( Window win)  
  {
    window = win;

    setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));
    setBorder( BorderFactory.createEtchedBorder());
  }

  void update()
  {    
  }

  void Add(){}
  void Delete(){}

  /************* interface Editor ************************/
  public EditorContainer getEditorContainer()
  {
    return (EditorContainer)window;
  }

  public void close(boolean doCancel)
  {
    window.setVisible(false);
  }
  public void save(){}
  public void saveAs(){}
  public void print(){} 

  /********************************/
  private Window window;
  private final int DEFAULT_WIDTH = 450;
  private final int DEFAULT_HEIGHT = 280;
}




