Summary: graphical programming environment for developing audio applications
Name: jMax
Version: 2.4.8b
Release: 1
Copyright: GPL
Group: Development/Languages
#Requires: swing
BuildRoot: /var/tmp/jmax-root
%description
jMax (``Java Max'') is a graphical programming environment 
for developing real-time, interactive audio applications. 

%prep

%build
make all ARCH=linuxpc

%install
rm -rf $RPM_BUILD_ROOT
make install ARCH=linuxpc prefix=$RPM_BUILD_ROOT/usr

%clean
rm -rf $RPM_BUILD_ROOT

%post

%files
%defattr(-, root, root)
%doc LICENCE.fr LICENSE README VERSION doc/*
/usr/bin/jmax
/usr/lib/jmax
/usr/include/fts


