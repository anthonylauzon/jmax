package ircam.jmax.widgets;

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;

public class ObjectSetViewer extends JPanel {

  static class ObjectCellRenderer extends JLabel implements ListCellRenderer
  {
    private static ImageIcon patcherIcon = null;
    private static ImageIcon objectIcon = null;
    private static ImageIcon errorIcon = null;
    private static ImageIcon commentIcon = null;
    private static ImageIcon messageIcon = null;

    private final static Color selectedColor = new Color( 51, 153, 204);

    private static void loadImages()
    {
      String fs = File.separator;
      String path = MaxApplication.getProperty( "root" ) + fs + "images" + fs;

      patcherIcon = new ImageIcon( path + "tool_patcher.gif");
      objectIcon = new ImageIcon( path + "tool_ext.gif");
      errorIcon = new ImageIcon( path + "tool_err.gif");
      commentIcon = new ImageIcon( path + "tool_text.gif");
      messageIcon = new ImageIcon( path + "tool_mess.gif");
    }

    public ObjectCellRenderer() 
    {
      if (patcherIcon == null)
	loadImages();
      
      setOpaque(true);
    }

    public Component getListCellRendererComponent( JList jlist, Object obj, int i, boolean selected, boolean hasFocus)
    {
      if (obj != null)
	{
	  if ( selected)
	    setBackground( selectedColor);
	  else
	    setBackground( jlist.getBackground());

	  if (((FtsObject) obj).isError())
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( errorIcon);
	    }
	  else if (obj instanceof FtsPatcherObject)
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( patcherIcon);
	    }
	  else if (obj instanceof FtsMessageObject)
	    {
	      setText( ((FtsMessageObject) obj).getMessage());
	      setIcon( messageIcon);
	    }
	  else if (obj instanceof FtsCommentObject)
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( commentIcon);
	    }
	  else
	    {
	      setText( ((FtsObject) obj).getDescription());
	      setIcon( objectIcon);
	    }
	}

      return this;
    }
  }

  class ObjectSetViewerMouseListener implements MouseListener
  { 
    public void mouseEntered(MouseEvent e) {} 
    public void mouseExited(MouseEvent e) {}
    public void mousePressed(MouseEvent e) {}
    public void mouseReleased(MouseEvent e) {}

    public void mouseClicked(MouseEvent e)
    {
      if (e.getClickCount() == 2)
	{
	  int index = jList.locationToIndex(e.getPoint());
	  
	  if (index < jList.getModel().getSize())
	    {
	      if (objectSelectedListener != null)
		{
		  FtsObject object = (FtsObject) jList.getModel().getElementAt(index);

		  objectSelectedListener.objectSelected(object);
		}
	    }
	  else
	    {
	      System.err.println("Problem in ObjectSetViewer " + ObjectSetViewer.this);
	      System.err.println(" point " + e.getPoint() + " index " + index);
	      System.err.println(" listener " + objectSelectedListener);
	    }
	}
    }
  }

  public ObjectSetViewer()
    {
      jList = new JList();

      jList.setBackground( Color.white);

      // Create the cell renderer
      jList.setCellRenderer( new ObjectCellRenderer());

      jList.addMouseListener(new ObjectSetViewerMouseListener());
 
      JScrollPane scrollPane = new JScrollPane();
      scrollPane.getViewport().setView( jList);

      scrollPane.setAlignmentX( LEFT_ALIGNMENT);
      scrollPane.setAlignmentY( TOP_ALIGNMENT);

      setLayout( new BoxLayout( this, BoxLayout.Y_AXIS));

      add( scrollPane);
    }

  public void setModel( ListModel model)
  {
    jList.setModel( model);
    repaint();  // sometimes, it seems that setModel() does not force a refresh ???
  }

  public void setObjectSelectedListener(ObjectSelectedListener objectSelectedListener)
  {
    this.objectSelectedListener = objectSelectedListener;
  }

  protected JList jList;
  private ObjectSelectedListener objectSelectedListener;
}


