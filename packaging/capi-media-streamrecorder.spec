Name:       capi-media-streamrecorder
Summary:    A Streamrecorder library in Tizen Native API
Version:    0.0.3
Release:    0
Group:      Multimedia/Other
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
Buildrequires:  pkgconfig(mm-streamrecorder)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-media-tool)
BuildRequires:  pkgconfig(capi-media-audio-io)
BuildRequires:  pkgconfig(libtbm)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
A StreamRecorder library in Tizen Native API to record live buffer sent by the application


%package devel
Summary:  A StreamRecorder library in Tizen Native API (Development)
Requires: %{name} = %{version}-%{release}


%description devel
A MediaStreamRecorder library in Tizen Native API Development Package to record live buffer


%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DFULLVER=%{version} -DMAJORVER=${MAJORVER}


make %{?jobs:-j%jobs}


%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}%{_datadir}/license
cp LICENSE.APLv2 %{buildroot}%{_datadir}/license/%{name}


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig

%files
%manifest capi-media-streamrecorder.manifest
%{_libdir}/libcapi-media-streamrecorder.so.*
%{_datadir}/license/%{name}
%{_bindir}/*

%files devel
%{_includedir}/media/streamrecorder.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-media-streamrecorder.so

