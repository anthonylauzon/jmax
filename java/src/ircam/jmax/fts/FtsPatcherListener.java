//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

package ircam.jmax.fts;

/** This interface define a listener of the changes
  inside a patcher. It is installed on a FtsPatcherData instance
  */

public interface FtsPatcherListener
{
  public void objectAdded(FtsPatcherData data, FtsObject object);
  public void objectRemoved(FtsPatcherData data, FtsObject object);

  public void connectionAdded(FtsPatcherData data, FtsConnection connection);
  public void connectionRemoved(FtsPatcherData data, FtsConnection connection);

  public void patcherChangedNumberOfInlets(FtsPatcherData data, int nins);
  public void patcherChangedNumberOfOutlets(FtsPatcherData data, int nouts);
  public void patcherChanged(FtsPatcherData data);

  public void patcherHaveMessage(String msg);
}
