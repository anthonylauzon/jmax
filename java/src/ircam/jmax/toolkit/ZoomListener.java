package ircam.jmax.toolkit;

/**
 * Utility interface, used by editors that
 * have a zoomable working area */
public interface ZoomListener
{
  public abstract void zoomChanged(float newZoom);
}
