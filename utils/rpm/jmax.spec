Name: jmax
Summary: graphical programming environment for developing audio applications
Version: 2.4.13
Release: 1
Copyright: GPL
Group: Applications/Multimedia
Source: ftp://ftp.ircam.fr/pub/IRCAM/equipes/temps-reel/jmax/releases/jmax-%{version}-src.tar.gz
BuildRoot: /var/tmp/jmax-%{version}-root
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
%ifarch ppc
make all ARCH=ppc-linux
%endif

%install
rm -rf $RPM_BUILD_ROOT
%ifarch i386
make install ARCH=i386-linux prefix=$RPM_BUILD_ROOT/usr
%endif
%ifarch i686
make install ARCH=i686-linux prefix=$RPM_BUILD_ROOT/usr
%endif
%ifarch ppc
make install ARCH=ppc-linux prefix=$RPM_BUILD_ROOT/usr
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


