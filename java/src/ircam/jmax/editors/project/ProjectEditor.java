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

package ircam.jmax.editors.project; 

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.pkg.*;

public class ProjectEditor implements Editor {
    private ProjectWindow window = null;

    public ProjectWindow getWindow() 
    {
	return window;
    }

    private Project project = null;

    public Project getProject() 
    {
	return project;
    }

    public ProjectEditor(Project p) 
    {
	project = p;
	setProjectWindow(new ProjectWindow(this));
	//fireEditorReadyListeners();
    }

    public void setProjectWindow(ProjectWindow window) {
	this.window = window;
    }

    public Fts getFts() 
    {
	return project.getFts();
    }

    public EditorContainer getEditorContainer() 
    {
	return window;
    }

    public MaxDocument getDocument() 
    {
	throw new RuntimeException("Not implemeted yet");
    }

    public void Close(boolean doCancel) 
    {
	throw new RuntimeException("Not implemeted yet");
    }

    
}
