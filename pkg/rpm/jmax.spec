Summary: graphical programming environment for developing audio applications
Name: jMax
Version: 2.4.8
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

%setup -q

%build
make all ARCH=linuxpc

%install
rm -rf $RPM_BUILD_ROOT
make install ARCH=linuxpc prefix=$RPM_BUILD_ROOT/usr

%clean
rm -rf $RPM_BUILD_ROOT

%post

%files


