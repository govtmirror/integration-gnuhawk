# By default, the RPM will install to the standard REDHAWK SDR root location (/var/redhawk/sdr)
# You can override this at install time using --prefix /new/sdr/root when invoking rpm (preferred method, if you must)
%{!?_sdrroot: %define _sdrroot /var/redhawk/sdr}
%define _sdrroot /var/redhawk/sdr
%define _prefix %{_sdrroot}
Prefix: %{_prefix}

# Point install paths to locations within our target SDR root
%define _sysconfdir    %{_prefix}/etc
%define _localstatedir %{_prefix}/var
%define _mandir        %{_prefix}/man
%define _infodir       %{_prefix}/info

Name: add_const_ss
Summary: Component %{name}
Version: 1.0.0
Release: 1%{?dist}
License: None
Group: REDHAWK/Components
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root

Requires: redhawk >= 1.8
Requires: gnuhawk >= 1.8.3
BuildRequires: redhawk-devel >= 1.8
BuildRequires: gnuhawk >= 1.8.3
BuildRequires: fftw-devel
BuildRequires: autoconf automake libtool

# Interface requirements
Requires: bulkioInterfaces
BuildRequires: bulkioInterfaces

%description
Component %{name}


%prep
%setup


%build
# Implementation cpp
pushd cpp
export SDRROOT=%{_sdrroot}
./reconf
%define _bindir %{_prefix}/dom/components/add_const_ss/cpp
%configure
make %{?_smp_mflags}
popd


%install
rm -rf $RPM_BUILD_ROOT
# Implementation cpp
pushd cpp
export SDRROOT=%{_sdrroot}
%define _bindir %{_prefix}/dom/components/add_const_ss/cpp
make install DESTDIR=$RPM_BUILD_ROOT
popd


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,redhawk,redhawk)
%dir %{_prefix}/dom/components/gnuhawk/gr/%{name}/1.0.0
%{_prefix}/dom/components/gnuhawk/gr/%{name}/1.0.0/add_const_ss.scd.xml
%{_prefix}/dom/components/gnuhawk/gr/%{name}/1.0.0/add_const_ss.prf.xml
%{_prefix}/dom/components/gnuhawk/gr/%{name}/1.0.0/add_const_ss.spd.xml
%{_prefix}/dom/components/gnuhawk/gr/%{name}/1.0.0/cpp

%{_prefix}/dom/components/gnuhawk/gr/%{name}/current
