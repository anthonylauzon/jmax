//
//  JMaxWrapper.java
//  jMaxProject
//
//  Created by riccardo borghesi on Fri Sep 06 2002.
//  Copyright (c) 2002 __Ircam__. All rights reserved.
//

import java.awt.*;
import java.io.*;
import com.apple.eawt.*;
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
   Application jMaxApp = new Application();
   jMaxApp.addApplicationListener( new ApplicationAdapter(){
     public void handleOpenApplication(ApplicationEvent e){}
     public void handleOpenFile(ApplicationEvent e){
       toOpenFile = new File( e.getFilename());
       if( JMaxApplication.getFtsServer() != null)
         openFile( toOpenFile);
     }
     public void handleQuit(ApplicationEvent e){
       JMaxApplication.Quit();
       e.setHandled( true);
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
