//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;
import javax.swing.*;

/** An instance of this document handler can load MaxDocument from
 *  a bmax binary file.
 * Ask FTS to load the file, using the new
 * incremental Application Layer architecture.
 */

public class FtsDotPatRemoteDocumentHandler extends MaxDocumentHandler
{
  private ImageIcon patDocumentIcon;

  public FtsDotPatRemoteDocumentHandler()
  {
    patDocumentIcon = Icons.get("_max_patcher_file_");
  }

  /** We can load from a file start with the "max v2" string*/

  public boolean canLoadFrom(File file)
  {
    if (super.canLoadFrom(file))
      {
	try
	  {
	    FileReader fr = new FileReader(file);

	    char buf[] = new char[6];
    
	    fr.read(buf);
	    fr.close();

	    if ((new String(buf)).equals("max v2"))
	      return true;
	    else
	      return false;
	  }
	catch (FileNotFoundException e)
	  {
	    return false;
	  }
	catch (IOException e)
	  {
	    return false;
	  }
      }
    else
      return false;
  }

  /** Make the real document */

  protected MaxDocument loadDocument(MaxContext context, File file)
  {
    FtsServer server;
    FtsObject patcher;
    int id;

    if (! (context instanceof Fts))
      return null;

    Fts fts = (Fts) context;

    // Load the environment file if needed 

    MaxEnv.loadEnvFileFor(context, file);

    // ask fts to load the file 

    patcher = fts.loadPatFile(file);

    if (patcher != null)
      {
	FtsPatcherDocument obj = new FtsPatcherDocument(context);

	// Temporary hack to force the patcher uploading; really, MDA should allow for 
	// async edit of documents ...

	patcher.updateData();
	fts.sync();
	obj.setRootData((MaxData) patcher.getData());
	obj.setDocumentFile(file);
	obj.setDocumentHandler(this);
	    
	return obj;
      }
    else
      return null;
  }

  public void saveDocument(MaxDocument document, File file) throws MaxDocumentException
  {
    throw new MaxDocumentException("Cannot save in .pat file format");
  }

  // Overwrite upper class method; we cannot save to a .pat file.

  public boolean canSaveTo(File file)
  {
    return false;
  }

  public boolean canSaveTo(MaxDocument document, File file)
  {
    return false;
  }

  public String getDescription()
  {
    return "Max Patches";
  }

  public Icon getIcon()
  {
    return patDocumentIcon;
  }
}





