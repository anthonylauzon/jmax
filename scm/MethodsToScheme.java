import java.lang.reflect.*;
import java.util.*;
import java.io.*;

public class MethodsToScheme {

    public MethodsToScheme(MethodPrinter out, String fullClassName, Hashtable functions) throws Exception {
	fullClassName = normalizeClassName(fullClassName);
	String className = getClassName(fullClassName);
	String schemedClassName = schemify(className);
	Class claz = Class.forName(fullClassName);
	
	Constructor[] constructor = claz.getDeclaredConstructors();
	for (int i = 0; i < constructor.length; i++) {
	    String funName = "new-" + schemedClassName;
	    if (i > 0) {
		funName += i;
	    } 
	    if ((functions == null) || (functions.get(funName) != null)) {
		Constructor c = constructor[i];
		String cName = c.getName();
		Class[] argType = c.getParameterTypes();
		String[] argName = new String[argType.length];
		for (int k = 0; k < argType.length; k++) {
		    argName[k] = argType[k].getName();
		}
		out.printConstructor(funName, fullClassName, argName);
		if (functions != null) {
		    functions.put(funName, Boolean.TRUE);
		}
	    }
	}

	Hashtable table = new Hashtable();
	Method[] method = claz.getDeclaredMethods();
	for (int i = 0; i < method.length; i++) {
	    Method m = method[i];
	    String methodName = m.getName();
	    Integer num = (Integer) table.get(methodName);
	    String funName = schemedClassName + '-' + schemify(methodName);
	    if (num != null) {
		funName += num;
	    } else {
		num = new Integer(0);
	    }
	    if ((functions == null) || (functions.get(funName) != null)) {
		String type;
		if (Modifier.isStatic(m.getModifiers())) {
		    type = "static";
		} else if (claz.isInterface()) {
		    /* Modifier.isInterface(m.getModifiers()) does not
                     * seem to work the I expected */
		    type = "interface";
		} else {
		    type = "virtual";
		}		
		String retName = m.getReturnType().getName();
		Class[] argType = m.getParameterTypes();
		String[] argName = new String[argType.length];
		for (int k = 0; k < argType.length; k++) {
		    argName[k] = argType[k].getName();
		}
		out.printMethod(funName, fullClassName, methodName, type, retName, argName);
		if (functions != null) {
		    functions.put(funName, Boolean.TRUE);
		}
	    }
	    table.put(methodName, new Integer(num.intValue() + 1));
	}
    }

    String normalizeClassName(String s) {
	if (s.endsWith(".class")) {
	    s = s.substring(0, s.length() - 6);
	}
	if (s.endsWith(".java")) {
	    s = s.substring(0, s.length() - 5);
	}
	return s.replace(File.separatorChar, '.');
    }

    String schemify(String s) {
	int len = s.length();
	StringBuffer sb = new StringBuffer(len + 10);
	for (int i = 0; i < len; i++) {
	    char c = s.charAt(i);
	    if (Character.isUpperCase(c)) {
		c = Character.toLowerCase(c);
		if (i > 0) {
		    sb.append('-');
		}
	    } 
	    sb.append(c);
	}
	return sb.toString();
    }

    String getClassName(String s) {
	int index = s.lastIndexOf(".");
	return (index > 0) ? s.substring(index + 1) : s;
    }

    public static void main(String[] arg) throws Exception {
	if (arg.length < 2) {
	    System.out.println("Usage: java MethodsToScheme printerclass {classlist|-} [functionlist]");
	    return;
	} 
	MethodPrinter printer = (MethodPrinter) Class.forName(arg[0]).newInstance();
	BufferedReader in;
	in = (arg[1].equals("-")) ? new BufferedReader(new FileReader(FileDescriptor.in)) 
	                          : new BufferedReader(new FileReader(arg[1]));
	Hashtable functions = null;
	if (arg.length == 3) {
	    functions = new Hashtable();
	    BufferedReader r = new BufferedReader(new FileReader(arg[2]));
	    while (true) {
		String f = r.readLine();
		if (f == null) {
		    r.close();
		    break;
		}
		if ((f.length() == 0) || (f.charAt(0) == '#') || (f.charAt(0) == ';')) {
		    continue;
		}
		functions.put(f.trim(), Boolean.FALSE);
	    }
	}
	while (true) {
	    String classname = in.readLine();
	    if (classname == null) {
		break;
	    }
	    if ((classname.length() == 0) || (classname.charAt(0) == '#') || (classname.charAt(0) == ';')) {
		continue;
	    }
	    try {
		new MethodsToScheme(printer, classname, functions);
	    } catch (Exception e) {
		e.printStackTrace(System.err);
	    }
	}
	in.close();
	if (functions != null) {
	    Enumeration e = functions.keys();
	    while (e.hasMoreElements()) {
		String k = (String) e.nextElement();
		Object v = functions.get(k);
		if (v == Boolean.FALSE) {
		    System.err.println("Failed " + k);
		}
	    }
	}
	System.exit(0);
    }
}


