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

package ircam.jmax.editors.table;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

import ircam.jmax.editors.table.menus.*;

/** 
 * The table editor
 */
public class Tabler extends JFrame implements EditorContainer {
  
  //--- Fields 
  TablePanel itsPanel;

  private EditorMenu itsFileMenu;
  private EditMenu   itsEditMenu;	
  private ViewMenu   itsViewMenu;
  private JMenu   itsWindowsMenu;
  final static int CONTROLBAR_HEIGHT = 20;
  static public Font displayFont = new Font("SansSerif", Font.PLAIN, 10);

  /** Constructor */
  public Tabler( FtsObjectWithEditor ftsObj, TableDataModel tm) {
    
    super();
    getContentPane().setLayout( new BorderLayout());

    MaxWindowManager.getWindowManager().addWindow(this);
    
    // Make the title
    makeTitle();

    itsPanel = new TablePanel( this, ftsObj, tm);
    
    // Build The Menus and Menu Bar
    makeMenuBar();

    getContentPane().add(itsPanel, BorderLayout.CENTER);

    itsPanel.frameAvailable(); 

    //... the control bar
    makeControlBar();

    //--
    validate();
    pack();
    setVisible(true);

    addWindowListener(new WindowListener(){
	    public void windowOpened(WindowEvent e){}
	    public void windowClosed(WindowEvent e){}
	    public void windowClosing(WindowEvent e)
	    {
		itsPanel.close( false);
	    }
	    public void windowDeiconified(WindowEvent e){}
	    public void windowIconified(WindowEvent e){}
	    public void windowActivated(WindowEvent e){}
	    public void windowDeactivated(WindowEvent e){}
	});    
  }

  private final void makeTitle()
  { 
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Table"));
    
    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();

    // Build the file menu
    itsFileMenu = new ircam.jmax.toolkit.menus.DefaultFileMenu();
    itsFileMenu.setEnabled( false, 3);
    itsFileMenu.setEnabled( false, 4);
    mb.add( itsFileMenu); 
    
    // Build the edit menu
    itsEditMenu = new EditMenu(this); 
    mb.add( itsEditMenu); 
    
    // Build the view menu
    itsViewMenu = new ViewMenu(this);
    mb.add( itsViewMenu);

    // New Window Manager based Menu
    itsWindowsMenu = new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    setJMenuBar(mb);
  }

  JTextField maxValueField, minValueField;
  JPanel controlBar;
  JLabel displayMouseLabel, infoLabel;
  private void makeControlBar()
  {      
    controlBar = new JPanel(); 
    controlBar.setLayout( new BoxLayout( controlBar, BoxLayout.X_AXIS));
    controlBar.setBorder( BorderFactory.createEtchedBorder());
    controlBar.setBounds(0, 0, getSize().width, CONTROLBAR_HEIGHT);

    /* Mouse Display */
    displayMouseLabel = new JLabel();
    displayMouseLabel.setFont( displayFont);
    displayMouseLabel.setPreferredSize(new Dimension(102, 15));
    displayMouseLabel.setMaximumSize(new Dimension(102, 15));
    displayMouseLabel.setMinimumSize(new Dimension(102, 15));
    
    infoLabel = new JLabel("", JLabel.RIGHT);
    infoLabel.setFont( displayFont);
    infoLabel.setPreferredSize(new Dimension(100, 15));
    infoLabel.setMaximumSize(new Dimension(100, 15));
    infoLabel.setMinimumSize(new Dimension(100, 15));
	
    controlBar.add( displayMouseLabel);
    controlBar.add( Box.createRigidArea( new Dimension( 10, 0)));
    controlBar.add( infoLabel);

    itsPanel.getGraphicContext().setDisplay( displayMouseLabel, infoLabel);

    /* max Y */
    JLabel maxLabel = new JLabel("maxY", JLabel.CENTER);
    maxLabel.setFont( displayFont);
    maxValueField = new JTextField();
    maxValueField.setPreferredSize(new Dimension(50, 15));
    maxValueField.setMaximumSize(new Dimension(50, 15));
    maxValueField.setFont( displayFont);
    String maxText = ( itsPanel.getGraphicContext().isIvec()) ? ""+itsPanel.getGraphicContext().getVerticalMaximum() :
      ""+(int)(itsPanel.getGraphicContext().getVerticalMaximum()/100);
    maxValueField.setText( maxText);
    maxValueField.addActionListener( new ActionListener(){
	public void actionPerformed( ActionEvent e)
	{
	  try
	    {
	      int max = Integer.valueOf( maxValueField.getText()).intValue();
	      if( itsPanel.getGraphicContext().getVerticalMaximum() != max)
		itsPanel.setMaximumValue(max);
	      itsPanel.requestFocus();
	    }
	  catch (NumberFormatException e1)
	    {
	      System.err.println("Error:  invalid number format!");
	      return;
	    }
	}
      });
    
    JPanel maxPanel = new JPanel();
    maxPanel.setPreferredSize(new Dimension(90, 15));
    maxPanel.setLayout(new BoxLayout(maxPanel, BoxLayout.X_AXIS));    
    maxPanel.add(maxLabel);
    maxPanel.add( Box.createRigidArea( new Dimension( 5, 0)));
    maxPanel.add( maxValueField);

     /* min Y*/
    JLabel minLabel = new JLabel("minY", JLabel.CENTER);    
    minLabel.setFont( displayFont);
    minValueField = new JTextField();
    minValueField.setPreferredSize(new Dimension(50, 15));
    minValueField.setMaximumSize(new Dimension(50, 15));
    minValueField.setFont( displayFont);
    String minText = ( itsPanel.getGraphicContext().isIvec()) ? ""+itsPanel.getGraphicContext().getVerticalMinimum() :
      ""+(int)(itsPanel.getGraphicContext().getVerticalMinimum()/100);
    minValueField.setText( minText);
    minValueField.addActionListener( new ActionListener(){
	public void actionPerformed( ActionEvent e)
	{
	  try
	    {
	      int min = Integer.valueOf( minValueField.getText()).intValue();
	      if( itsPanel.getGraphicContext().getVerticalMinimum() != min)
		itsPanel.setMinimumValue( min);
	      minValueField.transferFocus();
	    }
	  catch (NumberFormatException e1)
	    {
	      System.err.println("Error:  invalid number format!");
	      return;
	    }
	}
      });
    
    JPanel minPanel = new JPanel();
    minPanel.setPreferredSize(new Dimension(90, 15));
    minPanel.setLayout(new BoxLayout( minPanel, BoxLayout.X_AXIS));    
    minPanel.add( minLabel);
    minPanel.add( Box.createRigidArea( new Dimension( 5, 0)));
    minPanel.add( minValueField);

    controlBar.add( Box.createHorizontalGlue());
    controlBar.add( maxPanel);
    controlBar.add( Box.createRigidArea( new Dimension( 5, 0)));
    controlBar.add( minPanel);

    getContentPane().add(controlBar, BorderLayout.SOUTH);
    validate();
  }
    
  // ------ editorContainer interface ---------------
  public Editor getEditor(){
    return itsPanel;
  }
  public Frame getFrame(){
    return this;
  }
  public Point getContainerLocation(){
    return getLocation();
  }
  public Rectangle getViewRectangle(){
    return getContentPane().getBounds();
  }
}








