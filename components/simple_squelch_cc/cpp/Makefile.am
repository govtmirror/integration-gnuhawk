#
# This file is protected by Copyright. Please refer to the COPYRIGHT file 
# distributed with this source distribution.
# 
# This file is part of GNUHAWK.
# 
# GNUHAWK is free software: you can redistribute it and/or modify is under the 
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later 
# version.
# 
# GNUHAWK is distributed in the hope that it will be useful, but WITHOUT ANY 
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with 
# this program.  If not, see http://www.gnu.org/licenses/.
#

# Manually set the GNURadio namespace here 
GR_NAMESPACE = gnuhawk/gr/level

namespacedir = $(prefix)/dom/components/$(GR_NAMESPACE)/simple_squelch_cc
versiondir = $(namespacedir)/$(PACKAGE_VERSION)

ossieName = simple_squelch_cc
bindir = $(versiondir)/cpp
bin_PROGRAMS = simple_squelch_cc

xmldir = $(versiondir)
dist_xml_DATA = ../simple_squelch_cc.scd.xml ../simple_squelch_cc.prf.xml ../simple_squelch_cc.spd.xml

# Create a "current" link to the version just installed
install-exec-hook:
	ln -fns $(versiondir) $(DESTDIR)$(namespacedir)/current

distclean-local:
	rm -f config.*
	rm -rf autom4te.cache
	rm -f acinclude.m4
	rm -f aclocal.m4
	rm -f configure
	rm -f depcomp
	rm -f install-sh
	rm -f ltmain.sh
	rm -f Makefile.in
	rm -f missing
	rm -rf .deps

# Sources, libraries and library directories are auto-included from a file
# generated by the REDHAWK IDE. You can remove/modify the following lines if
# you wish to manually control these options.
include $(srcdir)/Makefile.am.ide
simple_squelch_cc_SOURCES = $(redhawk_SOURCES_auto)
simple_squelch_cc_CXXFLAGS = -Wall $(GNUHAWK_CFLAGS) $(PROJECTDEPS_CFLAGS) $(BOOST_CPPFLAGS) $(INTERFACEDEPS_CFLAGS) $(redhawk_INCLUDES_auto)
simple_squelch_cc_LDADD = $(GNUHAWK_LIBS) $(PROJECTDEPS_LIBS) $(BOOST_LDFLAGS) $(BOOST_SYSTEM_LIB) $(BOOST_THREAD_LIB) $(INTERFACEDEPS_LIBS) $(redhawk_LDADD_auto)
simple_squelch_cc_LDFLAGS = -Wall $(redhawk_LDFLAGS_auto)
