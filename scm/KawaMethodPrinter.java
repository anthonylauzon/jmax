import java.io.*;

public class KawaMethodPrinter implements MethodPrinter {

    public void printConstructor(String f, String c, String[] a) {
	System.out.print("(define (" + f );
	int alen = a.length;
	for (int i = 0; i < alen; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println(')');
	System.out.println("  ((primitive-constructor");
	System.out.println("    <" + c + '>');
	System.out.print("    (");
	if (alen > 0) {
	    for (int i = 0; i < alen - 1; i++) {
		System.out.print("<"+ a[i] + "> ");
	    }
	    System.out.print("<"+ a[alen - 1] + ">");
	}
	System.out.println("))");
	System.out.print("  ");
	for (int i = 0; i < alen; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println("))");	
	System.out.println();	
    }

    public void printMethod(String f, String c, String m, String t, String r, String[] a) {
	System.out.print("(define (" + f );
	int alen = a.length;
	int numArg = (t.equals("static")) ? alen : alen + 1;
	for (int i = 0; i < numArg; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println(')');
	System.out.println("  ((primitive-" + t + "-method");
	System.out.println("    <" + c + '>');
	System.out.print("    \"" + m + "\" ");
	System.out.print("<" + r + "> (");
	if (alen > 0) {
	    for (int i = 0; i < alen - 1; i++) {
		System.out.print("<"+ a[i] + "> ");
	    }
	    System.out.print("<"+ a[alen - 1] + ">");
	}
	System.out.println("))");
	System.out.print("  ");
	for (int i = 0; i < numArg; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println("))");	
	System.out.println();	
    }
}


