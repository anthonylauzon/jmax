Summary: graphical programming environment for developing audio applications
Name: jMax
Version: 2.4.9e
Release: 1
Copyright: GPL
Group: Development/Languages
Source: ftp://ftp.ircam.fr/pub/IRCAM/equipes/temps-reel/jmax/releases/jmax-%{version}.tar.gz
#Requires: swing
BuildRoot: /var/tmp/jmax-root
%description
jMax (``Java Max'') is a graphical programming environment 
for developing real-time, interactive audio applications. 

%prep

%setup -n jmax-%{version}

%build
%ifarch i386
make all ARCH=i386-linux
%endif
%ifarch i686
make all ARCH=i686-linux
%endif

%install
rm -rf $RPM_BUILD_ROOT
%ifarch i386
make install ARCH=i386-linux prefix=$RPM_BUILD_ROOT/usr
%endif
%ifarch i686
make install ARCH=i686-linux prefix=$RPM_BUILD_ROOT/usr
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%post

%files
%defattr(-, root, root)
%doc LICENCE.fr LICENSE README VERSION doc/*
/usr/bin/jmax
/usr/lib/jmax
/usr/include/fts


