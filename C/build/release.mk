#
# release.mk - DevOpsBroker makefile for creating a .deb package of libdevopsbroker-dev
#
# Copyright (C) 2019 Edward Smith <edwardsmith@devopsbroker.org>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#
# -----------------------------------------------------------------------------
# Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-25
#
# -----------------------------------------------------------------------------
#

################################### Includes ##################################

include /etc/devops/globals.mk

################################## Variables ##################################

PKG_NAME := libdevopsbroker-dev
VERSION := 1.0.0
ARCH := amd64
PKG_ARCHIVE := $(PKG_NAME)_$(VERSION)_$(ARCH)

BUILD_DIR := $(TMPDIR)/$(PKG_ARCHIVE)
APPLICATION_DIR = $(realpath $(CURDIR)/..)
RELEASE_DIR := $(CURDIR)/pkg-debian

HEADERS_DIR := /usr/include
LIBRARY_DIR := /usr/lib

EXEC_CP := /bin/cp --preserve=timestamps

################################### Targets ###################################

.ONESHELL:
.PHONY: default clean createdirs copyheaders copylib applysecurity package printenv

default: package

clean:
	echo
	$(call printInfo,Cleaning existing release artifacts)
	/bin/rm -rf $(BUILD_DIR)
	/bin/rm -f $(TMPDIR)/$(PKG_ARCHIVE).deb
	/bin/rm -rf $(RELEASE_DIR)

createdirs: clean
	echo
	$(call printInfo,Creating $(RELEASE_DIR) directory)
	/bin/mkdir -p --mode=0750 $(RELEASE_DIR)

	$(call printInfo,Creating $(BUILD_DIR) directory)
	/bin/mkdir -p --mode=0755 $(BUILD_DIR)

	$(call printInfo,Creating $(BUILD_DIR)/DEBIAN directory)
	/bin/mkdir -p --mode=0755 $(BUILD_DIR)/DEBIAN

	$(call printInfo,Creating $(HEADERS_DIR) directory)
	/bin/mkdir -p --mode=0755 $(BUILD_DIR)/$(HEADERS_DIR)

	$(call printInfo,Creating $(LIBRARY_DIR) directory)
	/bin/mkdir -p --mode=0755 $(BUILD_DIR)/$(LIBRARY_DIR)
	echo

copyheaders: createdirs
	$(call printInfo,Copying libdevopsbroker C header files)
	cd ../src
	find . -name '*.h' -exec install -D {} $(BUILD_DIR)/$(HEADERS_DIR)/{} \;

copylib: createdirs
	$(call printInfo,Copying libdevopsbroker.a static library)
	$(EXEC_CP) -r ../lib/libdevopsbroker.a $(BUILD_DIR)/$(LIBRARY_DIR)

applysecurity: copyheaders copylib
	echo
	$(call printInfo,Applying security settings to $(BUILD_DIR))
	sudo find $(BUILD_DIR) -type d -exec /bin/chmod -c 755 {} \;
	sudo find $(BUILD_DIR) -type f -exec /bin/chmod -c 644 {} \;

	$(call printInfo,Changing ownership of $(BUILD_DIR) to root)
	sudo /bin/chown -R root:root $(BUILD_DIR)

package: applysecurity
	echo
	$(call printInfo,Installing $(BUILD_DIR)/DEBIAN/control file)
	$(EXEC_CP) $(CURDIR)/DEBIAN/control $(BUILD_DIR)/DEBIAN

	/usr/local/bin/deb-control $(BUILD_DIR)

	$(call printInfo,Installing $(BUILD_DIR)/DEBIAN/conffiles file)
	$(EXEC_CP) $(CURDIR)/DEBIAN/conffiles $(BUILD_DIR)/DEBIAN

	$(call printInfo,Installing $(BUILD_DIR)/DEBIAN/preinst file)
	$(EXEC_CP) $(CURDIR)/DEBIAN/preinst $(BUILD_DIR)/DEBIAN

	$(call printInfo,Generating DEBIAN/md5sums file)
	/usr/local/bin/md5sums $(BUILD_DIR)

	$(call printInfo,Applying security settings to /DEBIAN files)
	/bin/chmod 644 $(BUILD_DIR)/DEBIAN/control $(BUILD_DIR)/DEBIAN/conffiles $(BUILD_DIR)/DEBIAN/md5sums
	/bin/chmod 755 $(BUILD_DIR)/DEBIAN/preinst

	echo
	$(call printInfo,Building $(PKG_ARCHIVE).deb)
	/usr/bin/dpkg-deb -b $(BUILD_DIR)
	/bin/mv $(TMPDIR)/$(PKG_ARCHIVE).deb $(RELEASE_DIR)

	echo
	$(call printInfo,Generating SHA256SUM and fileinfo.html)
	cd $(RELEASE_DIR) && \
	/usr/bin/sha256sum $(PKG_ARCHIVE).deb > SHA256SUM && \
	/usr/local/bin/venture fileinfo $(PKG_ARCHIVE).deb

	/bin/chown -R $${SUDO_USER}:$${SUDO_USER} $(RELEASE_DIR)
	/bin/chmod 640 $(RELEASE_DIR)/*

	/bin/rm -rf $(BUILD_DIR)

printenv:
	echo "  MAKEFILE_LIST: $(MAKEFILE_LIST)"
	echo "         TMPDIR: $(TMPDIR)"
	echo "         CURDIR: $(CURDIR)"
	echo "       PKG_NAME: $(PKG_NAME)"
	echo "        VERSION: $(VERSION)"
	echo "           ARCH: $(ARCH)"
	echo "    PKG_ARCHIVE: $(PKG_ARCHIVE)"
	echo "      BUILD_DIR: $(BUILD_DIR)"
	echo "APPLICATION_DIR: $(APPLICATION_DIR)"
	echo "  UTILITIES_DIR: $(UTILITIES_DIR)"
	echo "    RELEASE_DIR: $(RELEASE_DIR)"
	echo "    INSTALL_DIR: $(INSTALL_DIR)"
	echo
