package ircam.jmax.editors.project;

import java.awt.*;
import ircam.jmax.utils.*;

/**
 * A panel containing the "filename, filetype, filesize" bar
 * in a project.
 */
public class ProjectBar extends Panel {
	//ErmesSketchPad itsSketchPad;
	//int itsPressedButton = -1;
	//boolean locked = false;
	//final static int NUM_BUTTONS = 11;
	//final static int LOCK_BUTTON = NUM_BUTTONS-1;

	//--------------------------------------------------------
    //	CONSTRUCTOR
    //
    //--------------------------------------------------------
	public ProjectBar() {
		
		setBackground(Color.lightGray);
		setLayout(new FlowLayout(FlowLayout.LEFT));
		Label fName = new Label("File name  ");
		Label rType = new Label("Resource Type  ");
		Label fSize = new Label("size"  );
		//Toolkit aToolkit = Toolkit.getDefaultToolkit();
       	//String aFontName = (Toolkit.getDefaultToolkit().getFontList())[0];
		Font newFont = new Font(ircam.jmax.utils.Platform.FONT_NAME, Font.BOLD, 12);
		fName.setFont(newFont);
		rType.setFont(newFont);
		fSize.setFont(newFont);
		add (fName);
		add (rType);
		add (fSize);	
	}
	
	public void paint (Graphics g) {
	
		g.setColor(getBackground());
	}	
}





