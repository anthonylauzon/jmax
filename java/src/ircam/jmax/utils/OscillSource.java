
package ircam.jmax.utils;

/**
 * The interface of the objects that can be connected to an oscilloscope.
 * The getValue method will be called each time the oscilloscope needs a 
 * new value to display (the rate depends from the freqence of the oscilloscope
 * 
 */
public interface OscillSource {
  abstract public int getValue();
}
