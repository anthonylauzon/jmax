
noinst_PROGRAMS = package
package_SOURCES = \
Closure.java					\
Continuation.java				\
Environment.java				\
InputPort.java					\
JavaMethod.java					\
Macro.java					\
Pair.java					\
Primitive.java					\
Procedure.java					\
Scheme.java					\
SchemePrimitives.java				\
SchemeUtils.java

JAVACX=@JAVACX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

clean-generic:
	rm -f *~
