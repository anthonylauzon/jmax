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

package ircam.jmax.editors.patcher;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.widgets.*;

// ^^^^ Same thing as for the other panels; how we find the server ?
// ^^^^ The panel itself is ready, the binding is missing.

class FpePanel extends JFrame implements WindowListener
{
  private static FpePanel fpePanel = null;
  private ObjectSetViewer objectSetViewer;
  private FtsObjectSet set;
  private Fts fts;

  static void registerFpePanel()
  {
    MaxWindowManager.getWindowManager().addToolFinder( new MaxToolFinder() {
      public String getToolName() { return "Fpe Panel";}
      public void open() { FpePanel.open(MaxApplication.getFts());}
    });
  }

  public static FpePanel open(Fts fts)
  {
    if (fpePanel == null)
      fpePanel = new FpePanel(fts);

    fpePanel.setVisible(true);

    return fpePanel;
  }

  protected FpePanel(Fts f)
  {
    super( "Fpe Panel");

    fts = f;
    addWindowListener(this);

    JPanel panel = new JPanel();
    panel.setBorder( new TitledBorder( "Floating-point errors"));
    panel.setLayout( new BorderLayout());

    JPanel p1 = new JPanel();
    p1.setBorder( new EmptyBorder( 3, 0, 3, 0));
    p1.setLayout( new BoxLayout( p1, BoxLayout.Y_AXIS));

    JCheckBox nanCheckBox = new JCheckBox( "Check NaN");
    nanCheckBox.addItemListener( new ItemListener() {
      public void itemStateChanged( ItemEvent e)
	{
	    if (e.getStateChange() == ItemEvent.DESELECTED)
	      fts.getDspController().setCheckNan( false);
	    else  if (e.getStateChange() == ItemEvent.SELECTED)
	      fts.getDspController().setCheckNan( true);
	}
    });
    p1.add( nanCheckBox);

    panel.add( p1, "North");

    objectSetViewer = new ObjectSetViewer();

    panel.add( objectSetViewer, "Center");

    JPanel p3 = new JPanel();
    p3.setBorder( new EmptyBorder( 3, 0, 3, 0));

    p3.setLayout( new BoxLayout( p3, BoxLayout.X_AXIS));

    JButton clearButton = new JButton("Clear");
    clearButton.setMargin( new Insets(2,2,2,2));
    clearButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
	{
	  fts.getDspController().clearFpeCollecting();
	}});

    p3.add( clearButton);

    p3.add( Box.createGlue());

    JButton closeButton = new JButton( "Close");
    closeButton.setMargin( new Insets(2,2,2,2));
    closeButton.addActionListener( new ActionListener() {
      public void actionPerformed( ActionEvent e)
	{
	  setVisible( false);
	}
    });
    p3.add( closeButton);

    panel.add( p3, "South");

    getContentPane().add( panel);
    pack();
    validate();

    set = (FtsObjectSet) fts.newRemoteData( "object_set_data", null);
    objectSetViewer.setModel( set.getListModel());

    fts.getDspController().startFpeCollecting(set);

    objectSetViewer.setObjectSelectedListener(new ObjectSelectedListener() {
      public void objectSelected(FtsObject object)
	{
	  final Cursor temp = FpePanel.this.getCursor();

	  FpePanel.this.setCursor( Cursor.getPredefinedCursor( Cursor.WAIT_CURSOR));

	  fts.editPropertyValue(object.getParent(), object,
				new MaxDataEditorReadyListener() {
	    public void editorReady(MaxDataEditor editor)
	      {	  FpePanel.this.setCursor(temp);}
	  });
	}
    });
    
  }

  public void windowClosing(WindowEvent e)
  {
  }

  public void windowOpened(WindowEvent e)
  {
    fts.getDspController().startFpeCollecting(set);
  }

  public void windowClosed(WindowEvent e)
  {
    fts.getDspController().stopFpeCollecting();
  }

  public void windowIconified(WindowEvent e)
  {
    fts.getDspController().stopFpeCollecting();
  }

  public void windowDeiconified(WindowEvent e)
  {
    fts.getDspController().startFpeCollecting(set);
  }

  public void windowActivated(WindowEvent e)
  {
  }

  public void windowDeactivated(WindowEvent e)
  {
  }
}
