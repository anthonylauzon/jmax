Name: jmax
Summary: a visual programming environment for real-time, interactive multimedia applications
Version: 3.0.1
Release: 1
Copyright: GPL
Group: Applications/Multimedia
Source: ftp://ftp.ircam.fr/pub/IRCAM/equipes/temps-reel/jmax/releases/jmax-%{version}-src.tar.gz
BuildRoot: /var/tmp/jmax-%{version}-root
%description
jMax is a visual programming environment for developing real-time, interactive multimedia applications. 

%prep

%setup -n jmax-%{version}

%build
./configure
make
%endif

%install
make install
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%post

%files
%defattr(-, root, root)
%doc LICENCE.fr LICENSE README VERSION doc/*
/usr/bin/jmax
/usr/share/jmax
/usr/include/fts


