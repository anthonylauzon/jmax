//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
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
public class Tabler extends JMaxEditor{
  
  //--- Fields 
  TablePanel itsPanel;
  TableRuler tableRuler;
  
  private EditorMenu itsFileMenu;
  private EditMenu   itsEditMenu;	
  private ViewMenu   itsViewMenu;
  private JMenu   itsWindowsMenu;
  //final static int CONTROLBAR_HEIGHT = 20;
  static public Font displayFont = new Font("SansSerif", Font.PLAIN, 10);

  /** Constructor */
  public Tabler( FtsObjectWithEditor ftsObj, TableDataModel tm) {
    
    super();
    
    getContentPane().setLayout( new BorderLayout());
    itsPanel = new TablePanel( this, ftsObj, tm);
    getContentPane().add(itsPanel, BorderLayout.CENTER);
    itsPanel.frameAvailable(); 

    tableRuler = new TableRuler( itsPanel.getGraphicContext());
    getContentPane().add(tableRuler, BorderLayout.NORTH);
    
    makeTablerWindow();
    
    setSize(itsPanel.getPreferredSize().width, itsPanel.getPreferredSize().height + 10);
  }
  
  public Tabler( Tabler copyTabler) 
  {
    super();
    
    getContentPane().setLayout( new BorderLayout());
    Rectangle bounds = copyTabler.getBounds();
    
    itsPanel = copyTabler.itsPanel;
    itsPanel.setContainer(this);
    copyTabler.getContentPane().remove(itsPanel);
    getContentPane().add(itsPanel, BorderLayout.CENTER);
    
    tableRuler = copyTabler.tableRuler;
    copyTabler.getContentPane().remove(tableRuler);
    getContentPane().add(tableRuler, BorderLayout.NORTH);
    copyTabler.dispose();
    System.gc();
    
    makeTablerWindow();
    
    setBounds(bounds);
  }  
  
  private void makeTablerWindow()
  {    
    addWindowListener( new WindowListener(){
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
    
    makeTitle();
    if(JMaxApplication.getProperty("no_menus") == null)
      makeMenuBar();   
    else
      makeSimpleMenuBar();
    validate();
  }
  
  private final void makeTitle()
  { 
    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Table"));
    MaxWindowManager.getWindowManager().windowChanged(this);
  }

  private final void makeMenuBar(){
    JMenuBar mb = new JMenuBar();

    itsFileMenu = new ircam.jmax.toolkit.menus.DefaultFileMenu();
    itsFileMenu.setEnabled( false, 3);
    itsFileMenu.setEnabled( false, 4);
    mb.add( itsFileMenu); 
    
    itsEditMenu = new EditMenu(this); 
    mb.add( itsEditMenu); 
    
    itsViewMenu = new ViewMenu(this);
    mb.add( itsViewMenu);

    itsWindowsMenu = new ircam.jmax.toolkit.menus.MaxWindowJMenu("Windows", this); 
    mb.add(itsWindowsMenu);

    setJMenuBar(mb);
  }

  EditorMenu simpleMenu = null;
  public EditorMenu getSimpleMenu()
  {
    if(simpleMenu == null)
      simpleMenu = new TableSimpleMenu(this);
    
    return simpleMenu; 
  }  
  
  /*JTextField maxValueField, minValueField;
  JPanel controlBar;
  JLabel displayMouseLabel, infoLabel;
  private void makeControlBar()
  {      
    controlBar = new JPanel(); 
    controlBar.setLayout( new BoxLayout( controlBar, BoxLayout.X_AXIS));
    controlBar.setBorder( BorderFactory.createEtchedBorder());
    controlBar.setBounds(0, 0, getSize().width, CONTROLBAR_HEIGHT);

    // Mouse Display 
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

    // max Y 
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

    // min Y
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
  }*/

  // ------ JMaxEditor ---------------
  public Editor getEditor(){
    return itsPanel;
  }
  public Rectangle getViewRectangle(){
    return getContentPane().getBounds();
  }
}








