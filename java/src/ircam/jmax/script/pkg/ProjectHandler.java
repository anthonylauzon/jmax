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

package ircam.jmax.script.pkg; 

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;

/**
 *  ProjectHandler
 *
 *  The project manager handles opening and closing a project.
 *
 */

public class ProjectHandler extends MaxDocumentHandler {
    /** Constructor */
    public ProjectHandler() 
    {
    }

    /** Return true if this Document Handler can load a new instance 
     *  from the given address. */
    public boolean canLoadFrom(File file)
    {
	return file.exists() && file.canRead() && file.getName().endsWith(".def");
    }

    /** Return true if this document handler can save to the given
     *  address; by default return true if the file exists, if it
     *  writable, and if we can load from it.  */
    public boolean canSaveTo(File file) {
	return file.canWrite() && canLoadFrom(file);
    }

    /** Load an document from a document file, in a given context.  If
     *  you want to call this, probabily you really want to call the
     *  static loadDocument.  */
    protected MaxDocument loadDocument(MaxContext context, File file) throws MaxDocumentException 
    {
	throw new RuntimeException("The MaxDocumentHandler interface isn't functionnal yet");
    }

    public void saveDocument(MaxDocument document, File file) throws MaxDocumentException
    {
	throw new RuntimeException("The MaxDocumentHandler interface isn't functionnal yet");
    }

    /** Return a string describing the kind of files this handler can
     * load It is used for the file chooser dialog */
    public String getDescription() {
	return "jMax project files";
    }

    public void openProject(Project proj, File dir) throws Exception {
	openProjectFile(proj, new File(dir, proj.getDefinitionFile()));
    }

    public void openProjectFile(Project proj, File projFile) throws Exception {
	/* Load the package */
	proj.loadFromFile(projFile, ""); 

	/* Boot the server. If the package didn't specify the
	 * server settings, use jMax default settings. */
	/* Before loading the package define the basic
	 * settings such as "root". */
	SettingsTable settings = proj.getSettings();
	String serverName = settings.getSetting("server-name");
	if (serverName.length() == 0) {
	    serverName = JMax.getSetting("server-name");
	}
	String serverPath = settings.getSetting("server-path");
	if (serverPath.length() == 0) {
	    serverPath = JMax.getSetting("server-path");
	}
	String connection = settings.getSetting("connection");
	if (connection.length() == 0) {
	    connection = JMax.getSetting("connection");
	}
	String host = settings.getSetting("host");
	if (host.length() == 0) {
	    host = JMax.getSetting("host");
	}
	String portStr = settings.getSetting("port");
	if (portStr.length() == 0) {
	    portStr = JMax.getSetting("port");
	}
	int port = Integer.parseInt(portStr);
	System.out.println("Start/connect to server:");
	System.out.println("Name: " + serverName);
	System.out.println("Path: " + serverPath);
	System.out.println("Connection: " + connection);
	System.out.println("Host: " + host);
	System.out.println("Port: " + port);

	/** In futur, we will also need to pass the command line
	 *  arguments to Fts since it needs to know the project
	 *  file and the additional settings. We also should pass
	 *  the server-root as value for the root. How are we
	 *  going to do that? */
	proj.itsFts = new Fts(serverPath, serverName, connection, host, port);	
	proj.itsFts.sync();

	/* Open the console */
	proj.itsConsole = new ircam.jmax.editors.console.ConsoleWindow();


	/* This is a hack: define "arch" and "mode" globally, but
	 * only AFTER the package (and most probably the jMax
	 * package) is loaded. Most of the packages depend on
	 * arch and mode to define their module path. Therefore we
	 * have to "export" them. However, we can't make them
	 * global before loading the package since the jMax
	 * package would erase the package settings. Therefore we
	 * set them global after loading the packages.*/
	// FIXME
	String arch = settings.getSetting("arch");
	if (arch.length() == 0) {
	    arch = JMax.getSetting("arch");
	}
	MaxApplication.getInterpreter().define("arch", arch);
	String mode = settings.getSetting("mode");
	if (mode.length() == 0) {
	    mode = JMax.getSetting("mode");
	}
	MaxApplication.getInterpreter().define("mode", mode);


	/* Tell the packages to load their modules. As soon as
	 * Fts can read the setting files, Fts will load the
	 * modules on its own */
	proj.loadModules(proj.itsFts);
	proj.itsFts.sync();


	/* Pass some of the settings to Fts. As long as Fts does
	 * not read the setting files, these settings will be *
	 * passed to Fts by the client. */


	/* Run the start hooks. Note however that the main reason
	 * why start hooks are used is to set the audio
	 * settings. These settings will now be set automatically
	 * by the project manager. */

	/* Open package window */ 
	new ircam.jmax.editors.project.ProjectEditor(proj);
    }

    public void closeProject() throws Exception {
	
    }


}
