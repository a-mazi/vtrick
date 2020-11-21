# Copyright © Artur Maziarek MMXX
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

RELDIR := bin
DEBDIR := debug
TRYDIR := tryouts
INCDIR := inc
SRCDIR := src
APPDIR := main
TMPDIR := intermediates
COVDIR := codecoverage

OBJ_APP_MODU_RELEASE := $(patsubst $(SRCDIR)/%.cpp,           $(RELDIR)/$(TMPDIR)/%.o,    $(wildcard $(SRCDIR)/*.cpp))
BIN_APP_RELEASE      := $(patsubst $(SRCDIR)/$(APPDIR)/%.cpp, $(RELDIR)/%,                $(wildcard $(SRCDIR)/$(APPDIR)/*.cpp))

OBJ_APP_MODU_DEBUG   := $(patsubst $(SRCDIR)/%.cpp,           $(DEBDIR)/$(TMPDIR)/%.o,    $(wildcard $(SRCDIR)/*.cpp))
BIN_APP_DEBUG        := $(patsubst $(SRCDIR)/$(APPDIR)/%.cpp, $(DEBDIR)/%,                $(wildcard $(SRCDIR)/$(APPDIR)/*.cpp))

OBJ_UT_ALL           := $(patsubst $(SRCDIR)/ut/%.cpp,        $(DEBDIR)/$(TMPDIR)/ut/%.o, $(wildcard $(SRCDIR)/ut/*.cpp))
BIN_UT               :=                                       $(DEBDIR)/ut/run

BIN_TRYOUTS          := $(patsubst $(SRCDIR)/$(TRYDIR)/%.cpp, $(DEBDIR)/$(TRYDIR)/%,      $(wildcard $(SRCDIR)/$(TRYDIR)/*.cpp))

OBJFILES := $(OBJ_APP_MODU_RELEASE) $(RELDIR)/$(TMPDIR)/$(APPDIR)/*.o \
            $(OBJ_APP_MODU_DEBUG)   $(DEBDIR)/$(TMPDIR)/$(APPDIR)/*.o \
            $(OBJ_UT_ALL)                                             \
                                    $(DEBDIR)/$(TMPDIR)/$(TRYDIR)/*.o
DEPFILES := $(OBJFILES:.o=.d)

RELEASE  := \"$(shell echo `git log --date=short --pretty='%ad_%h' -1``git status -s` | sed s/\\s/_/g | sed s/\>/_/g)\"

CXXFLAGS := -Wall -c -fmessage-length=0 -std=c++17 -MMD -MP
LNKFLAGS := -pthread


release: app_release
debug:   app_debug
clean:
	rm -rf $(RELDIR) $(DEBDIR)

-include $(DEPFILES)

app_release: CXXFLAGS += -DLOGLEVEL=3 -DRELEASE=$(RELEASE)
app_release: $(BIN_APP_RELEASE)

app_debug:   CXXFLAGS += -DLOGLEVEL=4
app_debug:   $(BIN_APP_DEBUG)
app_debug:   $(BIN_TRYOUTS)

ut:          CXXFLAGS += -DLOGLEVEL=4 -DUT_BUILD
ut:          $(BIN_UT)

cov:         CXXFLAGS += --coverage
cov:         LNKFLAGS += --coverage
cov:         app_debug

utcov:       CXXFLAGS += --coverage
utcov:       LNKFLAGS += --coverage
utcov:       ut
	$(BIN_UT)

covreport:
	@mkdir -p $(DEBDIR)/$(COVDIR)
	lcov --rc lcov_branch_coverage=1 --capture --directory $(DEBDIR)/$(TMPDIR) --output-file $(DEBDIR)/$(COVDIR)/coverage.info
	lcov --rc lcov_branch_coverage=1 --remove $(DEBDIR)/$(COVDIR)/coverage.info --output-file $(DEBDIR)/$(COVDIR)/coverage.info '*/inc/*' '/usr/*' '*/mo/*'
	genhtml $(DEBDIR)/$(COVDIR)/coverage.info --branch-coverage --output-directory $(DEBDIR)/$(COVDIR)

prof:       CXXFLAGS += -pg
prof:       LNKFLAGS += -pg
prof:       app_debug


$(RELDIR)/%: $(OBJ_APP_MODU_RELEASE) $(RELDIR)/$(TMPDIR)/$(APPDIR)/%.o
	g++ $(LNKFLAGS) -o $@ $^

$(DEBDIR)/%: $(OBJ_APP_MODU_DEBUG) $(DEBDIR)/$(TMPDIR)/$(APPDIR)/%.o
	g++ $(LNKFLAGS) -o $@ $^

$(BIN_UT): $(OBJ_APP_MODU_DEBUG) $(OBJ_UT_ALL)
	@mkdir -p $(@D)
	g++ $(LNKFLAGS) -o $@ $^ -lgtest -lgmock

$(DEBDIR)/$(TRYDIR)/%: $(DEBDIR)/$(TMPDIR)/$(TRYDIR)/%.o
	@mkdir -p $(@D)
	g++ $(LNKFLAGS) -o $@ $^


$(RELDIR)/$(TMPDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	g++ -I$(INCDIR) -O3 $(CXXFLAGS) -MF$(@:%.o=%.d) -MT$@ -o $@ $<
.PRECIOUS: $(RELDIR)/$(TMPDIR)/%.o

$(DEBDIR)/$(TMPDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	g++ -I$(INCDIR) -O0 -g3 $(CXXFLAGS) -MF$(@:%.o=%.d) -MT$@ -o $@ $<
.PRECIOUS: $(DEBDIR)/$(TMPDIR)/%.o
