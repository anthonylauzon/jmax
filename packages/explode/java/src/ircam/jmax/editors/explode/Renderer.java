package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.Enumeration;

/**
 * the interface of the classes that are able to give
 * a graphic representation of a list of events.
 * To Represent the single event, the Renderer uses
 * a (specialization of) EventRenderer class.
 */
public interface Renderer {

  /**
   * graphically represent the given subset of events.
   * Events are supposed to be organized into a vector-like
   * structure, accessible by index.
   * The order in this structure is up to data the model
   */
  abstract public void render(Graphics g, int startEvent, int endEvent);

  /**
   * returns the current event renderer
   */
  abstract public EventRenderer getEventRenderer();
  
  /**
   * returns the events whose graphic representation contains
   * the given point.
   */
  abstract public ScrEvent eventContaining(int x, int y);
  

  /**
   * returns an enumeration of all the events whose graphic representation
   * intersects the given rectangle
   */
  abstract public Enumeration eventsIntersecting(int x, int y, int width, int height);

}
