package ircam.jmax.fts;

import java.io.*;
import ircam.jmax.mda.*;

/** Interface (class later ??) for all the FtsDataObject,
 * i.e. object representing editable data stored in FTS
 */

public interface FtsDataObject extends MaxData
{
  abstract public void saveAsTcl(PrintWriter pw);
}
