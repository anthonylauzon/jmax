
include ../../VERSION

version=$(MAJOR).$(MINOR).$(PATCH_LEVEL)$(SNAPSHOT)

all: jmax.spec

jmax.spec: ../../VERSION
	awk -v version=$(version) '\
$$1 == "Version:" { printf( "Version: %s\n", version ); } \
$$1 != "Version:" { print} \
' jmax.spec > jmax.spec.out
	mv jmax.spec.out jmax.spec

