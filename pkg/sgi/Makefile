
include ../../VERSION

version=$(MAJOR).$(MINOR).$(PATCH_LEVEL)

all: jmax.spec

jmax.spec: ../../VERSION
	awk -v version=$(version) '\
$$1 == "version" { printf( "        version \"%s\"\n", version ); } \
$$1 != "version" { print} \
' jmax.spec > jmax.spec.out
	mv jmax.spec.out jmax.spec

