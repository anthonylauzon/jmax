
all: java scm

java:
	javac KawaMethodPrinter.java MethodPrinter.java MethodsToScheme.java SilkMethodPrinter.java

scm:
	java MethodsToScheme KawaMethodPrinter scheme_classes scheme_functions > kawa_interface.scm;
	java MethodsToScheme SilkMethodPrinter scheme_classes scheme_functions > silk_interface.scm;

