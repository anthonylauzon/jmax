package ircam.jmax.widgets;

import java.io.*;
import java.awt.*;
import com.sun.java.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


class ObjectCellRenderer extends JLabel implements ListCellRenderer {

  private static ImageIcon patcherIcon = null;
  private static ImageIcon objectIcon = null;

  private final static Color selectedColor = new Color( 51, 153, 204);

  private static void loadImages()
  {
    String fs = MaxApplication.getProperty( "file.separator");
    String path = MaxApplication.getProperty( "root" ) + fs + "images" + fs;

    patcherIcon = new ImageIcon( path + "tool_patcher.gif");
    objectIcon = new ImageIcon( path + "tool_ext.gif");
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
	setText( ((FtsObject) obj).getDescription());

	if ( selected)
	  setBackground( selectedColor);
	else
	  setBackground( jlist.getBackground());

	if ( ((FtsObject) obj) instanceof FtsPatcherObject)
	  setIcon( patcherIcon);
	else
	  setIcon( objectIcon);
      }

    return this;
  }
}

public class ObjectSetViewer extends JScrollPane {

  public ObjectSetViewer()
    {
      super();

      jList = new JList() {
	public Dimension getMaximumSize() 
	  {
	    return new Dimension(400, super.getMaximumSize().height);
	  }
      };

      jList.setBackground( Color.white);

      getViewport().setView( jList);

      setAlignmentX( LEFT_ALIGNMENT);
      setAlignmentY( TOP_ALIGNMENT);

      // Create the cell renderer
      jList.setCellRenderer( new ObjectCellRenderer());
    }

  public void setModel( ListModel model)
  {
    jList.setModel( model);
    repaint();  // sometimes, it seems that setModel() does not force a refresh ???
  }

  protected JList jList;
}
