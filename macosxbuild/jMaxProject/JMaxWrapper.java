//
//  JMaxWrapper.java
//  jMaxProject
//
//  Created by riccardo borghesi on Fri Sep 06 2002.
//  Copyright (c) 2002 __Ircam__. All rights reserved.
//

import java.awt.*;
import java.io.*;
import com.apple.mrj.*;
import ircam.jmax.*;

public class JMaxWrapper {

 static File toOpenFile = null;
 public static void main( String args[]) 
  {            
    initMacOSXApplication();    
    JMaxApplication.main( args);
    
    if( toOpenFile != null)
        openFile( toOpenFile);
  }
  
 public static void initMacOSXApplication()
  {
    MRJApplicationUtils.registerQuitHandler( new MRJQuitHandler(){
        public void handleQuit() {
            JMaxApplication.Quit();
            throw new IllegalStateException("Quit Pending User Confirmation");
        }	
    });
    MRJApplicationUtils.registerOpenDocumentHandler( new MRJOpenDocumentHandler(){
        public void handleOpenFile( File file)
        {        
            if( JMaxApplication.getFtsServer() == null)
                toOpenFile = file;
            else
                openFile( file);
        }
    });
  }
  
  public static void openFile( File file)
  {
    RecentFileHistory recentFileHistory = JMaxApplication.getRecentFileHistory();
    recentFileHistory.addFile( file);
    try
    {	
        JMaxApplication.load( file.getAbsolutePath());
    }
    catch(IOException e)
    {
        System.err.println("[OpenAction]: I/O error loading file "+file.getAbsolutePath());
    }
  }
}
