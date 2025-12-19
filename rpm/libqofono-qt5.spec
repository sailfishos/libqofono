Name:       libqofono-qt5

Summary:    A library of Qt 5 bindings for ofono
Version:    0.124
Release:    1
License:    LGPLv2
URL:        https://github.com/sailfishos/libqofono
Source0:    %{name}-%{version}.tar.bz2

BuildRequires:  cmake
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Test)

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
%cmake . -DLIBQOFONO_VERSION=$(sed 's/+.*//' <<<"%{version}")
%cmake_build

%install
%cmake_install
# MeeGo.QOfono legacy import
mkdir -p %{buildroot}%{_libdir}/qt5/qml/MeeGo/QOfono
ln -sf ../../QOfono/libQOfonoQtDeclarative.so %{buildroot}%{_libdir}/qt5/qml/MeeGo/QOfono/
sed 's/module QOfono/module MeeGo.QOfono/' < %{buildroot}%{_libdir}/qt5/qml/QOfono/qmldir > %{buildroot}%{_libdir}/qt5/qml/MeeGo/QOfono/qmldir

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%{_libdir}/%{name}.so.*
%license COPYING

%files declarative
%{_libdir}/qt5/qml/QOfono
%{_libdir}/qt5/qml/MeeGo/QOfono

%files devel
%{_libdir}/%{name}.so
%{_libdir}/pkgconfig/qofono-qt5.pc
%{_includedir}/qofono-qt5/*.h
%{_includedir}/qofono-qt5/dbus/ofono*.xml

%files tests
/opt/tests/%{name}/*

%files examples
/opt/examples/%{name}
