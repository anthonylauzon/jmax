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

package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;
import javax.swing.text.html.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

import java.net.URL;
import java.io.IOException;
/**
 * A class giving a set of standard questions dialogs.
 * Use it thru the static function; they provide 
 * the complete handling.
 */


public class HTMLDialog extends JDialog /*implements ActionListener, KeyListener*/
{
  static HTMLDialog dialog = null;
  JEditorPane editorPane;

  public static void showThisPackageDoc(String packageName, String title)
  {
    if(dialog==null){
      dialog = new HTMLDialog(getUrlName(packageName), title);
      dialog.setLocation(300, 300);
      dialog.setVisible(true);
    }
    else
      if(dialog.isVisible())
	dialog.toFront();
      else{
	dialog.reinit(getUrlName(packageName), title);      
	dialog.setLocation(300, 300);
	dialog.setVisible(true);
      }
  }

  public static void showThisPage(String path, String pageName, String title)
  {
    if(dialog==null){
      dialog = new HTMLDialog(path+pageName, title);
      dialog.setLocation(300, 300);
      dialog.setVisible(true);
    }    
    else
      if(dialog.isVisible())
	dialog.toFront();
      else{
	dialog.reinit(getUrlName(path+pageName), title);      
	dialog.setLocation(300, 300);
	dialog.setVisible(true);
      }
  }

  private static String getUrlName(String packageName){
    String separator = System.getProperty("file.separator");
    return "file:"+ MaxApplication.getProperty("jmaxRoot")+ 
      separator+"doc"+
      separator+"user"+
      separator+"packages"+
      separator+packageName+
      separator+"index.html";
  }

  public HTMLDialog(String urlName, String title)
  {
    super();

    //Create an editor pane.
    JEditorPane editorPane = createEditorPane(urlName);
    JScrollPane editorScrollPane = new JScrollPane(editorPane);
    editorScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
    editorScrollPane.setPreferredSize(new Dimension(650, 550));
    editorScrollPane.setMinimumSize(new Dimension(20, 20));

    JPanel panel = new JPanel();
    panel.setLayout(new GridLayout(1,1));
    JButton button = new JButton("Close");
    button.setPreferredSize(new Dimension(100, 30));
    button.setMaximumSize(new Dimension(100, 30));
    button.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e){
	setVisible(false); 
      }
    });
    panel.add(button);

    getContentPane().setLayout(new BorderLayout());
    
    getContentPane().add("Center", editorScrollPane);
    getContentPane().add("South", panel);

    editorPane.addHyperlinkListener(new Hyperactive()); 

    setTitle(title);
    //addKeyListener(this);

    pack();
  }
  
  private JEditorPane createEditorPane(String urlName) {
     editorPane = new JEditorPane();
     editorPane.setEditable(false);
     String s = null;
     try {
        URL helpURL = new URL(urlName);
       displayURL(helpURL, editorPane);
     } catch (Exception e) {
       System.err.println("Couldn't create help URL: " + urlName);
     }
     return editorPane;
   }

  private void displayURL(URL url, JEditorPane editorPane) {
    try {
      editorPane.setPage(url);
    } catch (IOException e) {
      System.err.println("Attempted to read a bad URL: " + url);
    }
  }

  void reinit(String urlName, String title){
    try {
      URL helpURL = new URL(urlName);
      displayURL(helpURL, editorPane);
      setTitle(title);
    } catch (Exception e) {
      System.err.println("Couldn't create help URL: " + urlName);
    }
  }
  
  /*public void actionPerformed(ActionEvent e){}
    public void keyTyped(KeyEvent e){}
    public void keyReleased(KeyEvent e){}
    public void keyPressed(KeyEvent e){}*/
  /***************************************************************/
  class Hyperactive implements HyperlinkListener {

    public void hyperlinkUpdate(HyperlinkEvent e) {
      if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
	JEditorPane pane = (JEditorPane) e.getSource();
	if (e instanceof HTMLFrameHyperlinkEvent) {
	  HTMLFrameHyperlinkEvent  evt = (HTMLFrameHyperlinkEvent)e;
	  HTMLDocument doc = (HTMLDocument)pane.getDocument();
	  doc.processHTMLFrameHyperlinkEvent(evt);
	} else {
	  try {
	    pane.setPage(e.getURL());
	  } catch (Throwable t) {
	    t.printStackTrace();
	  }
	}
      }
    }
  }
  /***************************************************************/
}



