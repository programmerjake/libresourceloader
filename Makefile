# Copyright (C) 2012-2016 Jacob R. Lifshay
# This file is part of Voxels.
#
# Voxels is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Voxels is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Voxels; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
#

.PHONY: all clean FORCE

BUILDDIR:=$(abspath build)
SOURCEDIRS:=. io util
SOURCES:=$(wildcard $(addsuffix /*.cpp,$(SOURCEDIRS)))
OBJECTS:=$(addprefix $(BUILDDIR)/,res.o $(SOURCES:%.cpp=%.o))
$(info $(OBJECTS))

all: $(BUILDDIR)/test

$(BUILDDIR)/%.o: %.cpp
	mkdir -p $(addprefix $(BUILDDIR)/,$(SOURCEDIRS)) && g++ -c -Wall -std=c++11 -o $@ $< `pkg-config libzip --cflags`

clean:
	rm -f $(OBJECTS) $(BUILDDIR)/res.zip $(BUILDDIR)/test

$(BUILDDIR)/res.zip: FORCE
	mkdir -p $(BUILDDIR) && { cd res; zip -r - .; } > $(BUILDDIR)/res.zip

$(BUILDDIR)/res.o: $(BUILDDIR)/res.zip
	{ cd $(BUILDDIR) && ld -r -b binary -o res.o res.zip; }

$(BUILDDIR)/test: $(OBJECTS)
	g++ -o $(BUILDDIR)/test $(OBJECTS) `pkg-config libzip --libs`