Name: jMax-Swing
Summary: Swing classes for jMax
Version: 1.1.1
Release: 1
Copyright: Commercial
Group: Applications/Multimedia
Source: jmax-swing-%{version}-src.tar.gz
BuildRoot: /var/tmp/jmax-swing-%{version}-root
%description
The Swing classes for jMax

%prep

%setup -n jmax-swing-%{version} -c

%build

%install
rm -rf $RPM_BUILD_ROOT
install -d --mode=0755 $RPM_BUILD_ROOT/usr/lib/jmax/java/lib/swing
for f in CHANGES.txt LICENSE.txt README.html README.txt swingall.jar
do
    install --mode=0644 $f $RPM_BUILD_ROOT/usr/lib/jmax/java/lib/swing
done

%clean
rm -rf $RPM_BUILD_ROOT

%post

%files
%defattr(-, root, root)
/usr/lib/jmax/java/lib/swing


