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

package ircam.jmax.editors.console;

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;

// import javax.swing.*;
import javax.swing.Box;
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;

// import javax.swing.event.*;
import javax.swing.event.AncestorListener;
import javax.swing.event.AncestorEvent;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.widgets.ConsoleArea;
import ircam.fts.client.*;

/**
 * Window containing a console
 */
 
public class ConsoleWindow extends JFrame implements EditorContainer, Editor, Printable {

  private ConsoleArea consoleArea;
  private ControlPanel controlPanel;

  private void makeContent()
  {
    getContentPane().setLayout(new BorderLayout());

    consoleArea = new ConsoleArea( 1000, 80);

    JScrollPane jsp = new JScrollPane( consoleArea);
    jsp.setVerticalScrollBarPolicy( JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
    jsp.setPreferredSize( new Dimension( 500, 550));

    getContentPane().add( BorderLayout.CENTER, jsp);

    JPanel toolbarPanel = new JPanel();
    toolbarPanel.setLayout(new BorderLayout());
    toolbarPanel.setPreferredSize(new Dimension(500, 23));

    JToolBar toolbar = new JToolBar("Control Panel", JToolBar.HORIZONTAL);
    toolbar.setPreferredSize(new Dimension(250, 23));

    controlPanel = new ControlPanel();

    toolbar.add( controlPanel);

    toolbar.addAncestorListener(new AncestorListener(){
	public void ancestorAdded(AncestorEvent event)
	{
	  Frame frame = (Frame)SwingUtilities.getWindowAncestor(event.getAncestor());
	  if(!(frame instanceof ConsoleWindow))
	    MaxWindowManager.getWindowManager().addWindow(frame);
	}
	public void ancestorRemoved(AncestorEvent event)
	{
	  Frame frame = (Frame)SwingUtilities.getWindowAncestor(event.getAncestor());
	  if(!(frame instanceof ConsoleWindow))
	    MaxWindowManager.getWindowManager().removeWindow(frame);
	}
	public void ancestorMoved(AncestorEvent event){}
      });

    toolbarPanel.add( BorderLayout.NORTH, toolbar);

    getContentPane().add( BorderLayout.NORTH, toolbarPanel);
  }

  private void makeMenuBar()
  {
    JMenuBar mb = new JMenuBar(){        
        //BUG FIX on MacOSX to avoid QUIT on CMD-a
        public boolean processKeyBinding(KeyStroke ks, KeyEvent e, int condition, boolean pressed)
        {
            if( e.getKeyChar() == 'a')
                return false;
            else
                return super.processKeyBinding(ks, e, condition, pressed);
        }		
    };

    EditorMenu fileMenu = new DefaultFileMenu();
    fileMenu.setEnabled( false, 3);
    fileMenu.setEnabled( false, 4);
    fileMenu.setEnabled( false, 5);
    mb.add( fileMenu); 
    mb.add( new DefaultProjectMenu()); 
    mb.add( new MaxWindowJMenu( "Windows", this));
    mb.add(Box.createHorizontalGlue());
    mb.add( new DefaultHelpMenu());
    
    setJMenuBar( mb);
  }

  public ConsoleWindow( String title)
  {
    super( title);

    makeContent();
    makeMenuBar();
    MaxWindowManager.setTopFrame( this);
    addWindowListener( MaxWindowManager.getWindowManager());
    setDefaultCloseOperation( JFrame.DO_NOTHING_ON_CLOSE);
    validate();
    pack();
    setVisible( true);
  }

  public ControlPanel getControlPanel()
  {
    return controlPanel;
  }

  public ConsoleArea getConsoleArea()
  {
    return consoleArea;
  }

  public EditorContainer getEditorContainer()
  {
    return this;
  }

  public void close(boolean doCancel)
  {
  }
  public void save()
  {
  }
  public void saveAs()
  {
  }
  public void print()
  {
    PrinterJob printJob = PrinterJob.getPrinterJob();
    PageFormat format = printJob.pageDialog(printJob.defaultPage());    
    printJob.setPrintable( this, format);	    
    
    if (printJob.printDialog()) {
      try {
	printJob.print();
      } catch (Exception ex) {
	 ex.printStackTrace();
      }
    }
  }

  // Methods from interface EditorContainer
  public Editor getEditor()
  {
    return this;
  }

  public Frame getFrame()
  {
    return this;
  }

  public Point getContainerLocation()
  {
    return getLocation();
  }

  public Rectangle getViewRectangle()
  {
    return getContentPane().getBounds();
  }

  public int print( Graphics g, PageFormat pf, int pi) throws PrinterException 
  {
    Point consolePos = SwingUtilities.convertPoint(consoleArea, getLocation(), this);
    double onsetX = pf.getImageableX()+consolePos.x;
    double onsetY = pf.getImageableY()+consolePos.y;
      
    ((Graphics2D)g).translate(onsetX, onsetY);

    if (pi >= 1) {
      return Printable.NO_SUCH_PAGE;
    }
    consoleArea.print((Graphics2D) g);
    return Printable.PAGE_EXISTS;
  }
}

