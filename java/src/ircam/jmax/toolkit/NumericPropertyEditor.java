package ircam.jmax.toolkit;

import java.beans.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.reflect.*;
import com.sun.java.swing.*;
import java.util.*;

/**
 * An editor for numeric (bean) properties. 
 * It edit one property of one object (at a time), 
 * in the specified edit flavour (for now, only EDIT_FIELD supported.) 
 * Note that this version takes care of the undo support of the object
 * it is editing, if any. The object is given a chance to store
 * its state before being changed. */
public class NumericPropertyEditor extends PropertyEditorSupport{
  
  public NumericPropertyEditor( Object target, String propertyName, int flavour)
    {
      
      try {
	this.descriptor = new PropertyDescriptor(propertyName, target.getClass()); 
      } catch (IntrospectionException e) {
	System.err.println("property "+propertyName+"not found for object "+target+"of the target class "+target.getClass().getName());
	return;
	// should rise an exception instead
      }

      param = new Integer[1];
      
      if (flavour == EDIT_FIELD_FLAVOUR) 
	{
	  textField = new JTextField();
	  textField.setEditable( true);
	  textField.setBackground( Color.white);
	  textField.addActionListener( new ActionListener() {
	    public void actionPerformed( ActionEvent e)
	      {
		if (beanInstance == null) return;
		boolean b = beanInstance instanceof UndoableData;
		
		if (b) ((UndoableData) beanInstance).beginUpdate();
		
		setAsText(((JTextField) e.getSource()).getText());

		if (b) ((UndoableData) beanInstance).endUpdate();
	      }
	  });
	}
      else 
	{
	  System.err.println("unsupported flavour");
	  return;
	  // this will be an exception
	}
    }
  

  /**
   * Re-reads and re-display the property value.
   * This method is to be used when the target can be modified
   * by sources other then this editor*/
  public void update()
  {
    textField.setText(getAsText());
  }

  /**
   * sets the TARGET of the editing, that is, the object instance
   * this editor is editing (not the value of the property) */
  public void setValue( Object obj)
  {
    beanInstance = obj;
    update();
  }

  public boolean isPaintable()
  {
    return true;
  }

  public Component getCustomComponent()
  {
    return textField;
  }


  public void paintValue(Graphics gfx,
                        Rectangle box)
  {
    textField.setText(getAsText());
  }

  /**
   * Parse the string we are editing, and invoke the write
   * method of the target object */
  public void setAsText(String text)
  {
    int temp = 0;

    try {
      temp = Integer.parseInt(text);
    } catch (Exception e) {}
    
    param[0] = new Integer(temp);

    if (descriptor.getWriteMethod() != null)
      {
	try {
	  descriptor.getWriteMethod().invoke(beanInstance, param);
	} 
	
	catch (IllegalAccessException iacc){System.err.println(iacc);} 
	catch (IllegalArgumentException iarg) {System.err.println(iarg);}
	catch (InvocationTargetException itexc) {System.err.println(itexc);}
	catch (Exception exce) {System.err.println(exce);}
	
      }
    else {
      System.err.println("warning: property "+descriptor.getDisplayName()+" does not have an accessible write method");
    }
  }

  /**
   * invoke the read method of the target, returning the string
   * representation of the property */
  public String getAsText()
  {
    if (beanInstance == null) return "";

    String value = "<unknown>";
    
    if ( descriptor.getPropertyType() == null)
      return value;
    
    Method readMethod = descriptor.getReadMethod();
    
    if ( readMethod != null && readMethod.getParameterTypes().length == 0)
      {
	try
	  {
	    Object ret = readMethod.invoke( beanInstance, (Object[])null);
	    if ( ret != null)
	      value = ret.toString();
	    else
	      value = "null";
	  }
	catch (IllegalArgumentException e) 
	  {
	  } 
	catch (IllegalAccessException e) 
	  {
	  } 
	catch (InvocationTargetException e) 
	  {
	  }
      }
    
    return value;
  }
  

  
  //--- Fields 
  protected Object beanInstance;
  protected PropertyDescriptor descriptor;
  protected JTextField textField;

  public static final int EDIT_FIELD_FLAVOUR = 0;
  private Object param[];
}




