package ircam.jmax.utils;

import java.io.*;
import java.lang.*;

// a "generic" filename filter, accepts the name that is passed during cration
// (instead of having a different class foreach different suffix to recognize, 
// we will have just different instances of the same class).
// would it be better to provide this class with a "SetFilter" method?
// (this allows us to re-use the same instance more then once)

// another solution would be to have a derivation of the fileselector
// who implements the FilenameFilter interface...

public class ExtensionFilter implements FilenameFilter {
	String itsString;
	public ExtensionFilter (String toAccept) {
		itsString = toAccept;
		}
		
	public boolean accept(File dir, String name) {
		return name.endsWith(itsString);
	}
}
