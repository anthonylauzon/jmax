package ircam.jmax.mda; 


/**
 * A MaxTclExecutedDocument is an empty document resulting from
 * the execution of a simple .tcl file; it exists so that we
 * can execute .tcl files with "Open", and also we can keep
 * track of the executed files (later:).
 * 
 */

public class MaxTclExecutedDocument extends MaxDocument 
{
  public MaxTclExecutedDocument()
  {
    super(Mda.getDocumentTypeByName(".tcl"));
  }
}






