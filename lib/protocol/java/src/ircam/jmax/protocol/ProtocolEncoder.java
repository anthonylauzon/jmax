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

package ircam.jmax.protocol;

/**
 * The <code>ProtocolEncode</code> class provides support for encoding and decoding
 * primitive types (int, float, strings) into/from a character buffer.
 * This buffer can be retrieved for sending/receiving to/from a UDP socket.
 *
 * @author        François Déchelle (dechelle@ircam.fr)
 * @version       first one
 */

public class ProtocolEncoder {
  private static final int DEFSIZE = 1024;

  private byte[] buffer;
  private int current;

  /**
   *
   * @param bufferSize the size of the send/receive buffer
   */
  public ProtocolEncoder( int bufferSize)
  {
    buffer = new byte[ bufferSize];
    current = 0;
  }

  public ProtocolEncoder()
  {
    this( DEFSIZE);
  }

  /**
   * Returns a pointer to the current message, in order to send it on
   * a UDP port (for example).
   * @return  the current message as a byte array
   */
  public byte[] getMess()
  {
    return buffer;
  }

  /**
   * Returns the size of the current encoded message, in order to send it 
   * on a UDP port (for example).
   * @return the size of the current message
   */
  public int getSize()
  {
    return current;
  }

  /**
   * Starts encoding a new set of values
   */
  public void start()
  {
    current = 0;
  }

  /**
   * Finish encoding a new set of values.
   * It adds a 'end of message' token.
   */
  public void end()
  {
    buffer[current] = (byte)Protocol.EOM_CODE;
    current++;
  }

  /**
   * Append an int value to the current message
   */
  public void putInt( int value)
  {
    if (current + 5 < buffer.length)
      {
	buffer[current] = (byte)Protocol.INT_CODE;
	current++;
	buffer[current] = (byte)((value >> 24) & 0xff);
	current++;
	buffer[current] = (byte)((value >> 16) & 0xff);
	current++;
	buffer[current] = (byte)((value >> 8) & 0xff);
	current++;
	buffer[current] = (byte)((value >> 0) & 0xff);
	current++;
      }
  }

  /**
   * Append a string value to the current message
   */
  public void putString( String value)
  {
    if (current + 2 + value.length() < buffer.length)
      {
	int i;
	byte[] b = value.getBytes();

	buffer[current] = (byte)Protocol.STRING_START_CODE;
	current++;

	for ( i = 0; i < value.length(); i++)
	  {
	    buffer[current] = b[i];
	    current++;
	  }

	buffer[current] = (byte)Protocol.STRING_END_CODE;
	current++;
      }
  }
}

