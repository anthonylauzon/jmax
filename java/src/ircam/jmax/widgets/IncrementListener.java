package ircam.jmax.widgets;

/**
 * The interface associated to the IncrementController objects */
public interface IncrementListener {
  /** called when the user pressed the increment button */
  public abstract void increment();
  /** called when the user pressed the decrement button */
  public abstract void decrement();

}
