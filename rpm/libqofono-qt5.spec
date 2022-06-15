Name:       libqofono-qt5

Summary:    A library of Qt 5 bindings for ofono
Version:    0.114
Release:    1
License:    LGPLv2
URL:        https://github.com/sailfishos/libqofono
Source0:    %{name}-%{version}.tar.bz2

BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Test)

# license macro requires rpm >= 4.11
BuildRequires: pkgconfig(rpm)
%define license_support %(pkg-config --exists 'rpm >= 4.11'; echo $?)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
This package contains Qt bindings for ofono cellular service
interfaces.

%package declarative
Summary:    Declarative plugin for libqofono
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}

%description declarative
This package contains declarative plugin for libofono.

%package devel
Summary:    Development files for ofono Qt bindings
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}

%description devel
This package contains the development header files for the ofono Qt bindings.

%package tests
Summary:    qml test app for the ofono Qt bindings
Requires:   %{name} = %{version}-%{release}
Requires:   blts-tools
Requires:   phonesim
Requires:   mce-tools

%description tests
This package contains qml test for ofono Qt bindings.

%package examples
Summary:    Examples for libqofono
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}
Requires:   %{name}-declarative = %{version}

%description examples
This package contains examples for declarative plugin for libofono.

%prep
%setup -q -n %{name}-%{version}

%build
export QT_SELECT=5
%qmake5 "VERSION=$(sed 's/+.*//' <<<"%{version}")"
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
export QT_SELECT=5
%qmake5_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/%{name}.so.*
%if %{license_support} == 0
%license COPYING
%endif

%files declarative
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/MeeGo/QOfono/*

%files devel
%defattr(-,root,root,-)
%{_libdir}/%{name}.prl
%{_libdir}/%{name}.so
%{_libdir}/pkgconfig/qofono-qt5.pc
%{_includedir}/qofono-qt5/*.h
%{_includedir}/qofono-qt5/dbus/ofono*.xml
%{_datadir}/qt5/mkspecs/features/qofono-qt5.prf

%files tests
%defattr(-,root,root,-)
/opt/tests/%{name}/*

%files examples
%defattr(-,root,root,-)
/opt/examples/%{name}
