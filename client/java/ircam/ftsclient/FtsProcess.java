//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
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
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.ftsclient;

import java.io.*;

public class FtsProcess extends Process {

  public FtsProcess( int argc, String[] argv) throws IOException
  {
    String[] cmd = new String[argc];

    System.arraycopy( argv, 0, cmd, 0, argc);

    process = Runtime.getRuntime().exec( cmd);
  }

  public FtsProcess( String path) throws IOException
  {
    String[] cmd = new String[1];

    cmd[0] = path;

    process = Runtime.getRuntime().exec( cmd);
  }

  public void destroy()
  {
    process.destroy();
  }

  public int exitValue()
  {
    return process.exitValue();
  }

  public InputStream getErrorStream()
  {
    return null;
  }

  public InputStream getInputStream()
  {
    return process.getInputStream();
  }

  public OutputStream getOutputStream()
  {
    return process.getOutputStream();
  }

  public int waitFor() throws InterruptedException
  {
    return process.waitFor();
  }

  private Process process;
}
