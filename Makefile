# Copyright © 2020 Artur Maziarek
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

BINDIR   := bin
DEBUGDIR := debug
INCDIR   := inc
SRCDIR   := src
INTERMEDIATESDIR := intermediates
CODECOVERAGEDIR  := codecoverage
OBJDIR_RELEASE   := $(BINDIR)/$(INTERMEDIATESDIR)
OBJDIR_DEBUG     := $(DEBUGDIR)/$(INTERMEDIATESDIR)


TARGET_APP   := vtrick
SRCFILES_APP := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES_APP_RELEASE := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR_RELEASE)/%.o,$(SRCFILES_APP)) \
                                                   $(OBJDIR_RELEASE)/main/vtrick.o
OBJFILES_APP_DEBUG   := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR_DEBUG)/%.o,$(SRCFILES_APP)) \
                                                   $(OBJDIR_DEBUG)/main/vtrick.o
DEPFILES += $(OBJFILES_APP_RELEASE:.o=.d)
DEPFILES += $(OBJFILES_APP_DEBUG:.o=.d)

TARGET_UT   := unittests
SRCFILES_UT := $(wildcard $(SRCDIR)/ut/*.cpp)
OBJFILES_UT := $(patsubst $(SRCDIR)/ut/%.cpp,$(OBJDIR_DEBUG)/ut/%.o,$(SRCFILES_UT)) \
               $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR_DEBUG)/%.o,$(SRCFILES_APP))

DEPFILES += $(OBJFILES_UT:.o=.d)

CXXFLAGS := -Wall -c -fmessage-length=0 -std=c++17 -MMD -MP
LNKFLAGS := -pthread
RELEASE := \"$(shell echo `git log --date=short --pretty='%ad_%h' -1``git status -s` | sed s/\\s/_/g)\"


release: app_release
debug:   app_debug
clean:
	rm -rf $(BINDIR) $(DEBUGDIR)


app_release: CXXFLAGS += -DLOGLEVEL=3 -DRELEASE=$(RELEASE)
app_release: $(BINDIR)/$(TARGET_APP)
$(BINDIR)/$(TARGET_APP): $(OBJFILES_APP_RELEASE)
	g++ $(LNKFLAGS) -o $@ $^

app_debug: CXXFLAGS += -DLOGLEVEL=4
app_debug: $(DEBUGDIR)/$(TARGET_APP)
$(DEBUGDIR)/$(TARGET_APP): $(OBJFILES_APP_DEBUG)
	g++ $(LNKFLAGS) -o $@ $^

ut: CXXFLAGS += -DLOGLEVEL=4 -DUT_BUILD
ut: $(DEBUGDIR)/$(TARGET_UT)
$(DEBUGDIR)/$(TARGET_UT): $(OBJFILES_UT)
	g++ $(LNKFLAGS) -o $@ $^ -Lutil/googletest/lib/ -lgtest -lgmock

utcov: CXXFLAGS += --coverage
utcov: LNKFLAGS += --coverage
utcov: ut
	./debug/unittests
	@mkdir -p $(DEBUGDIR)/$(CODECOVERAGEDIR)
	lcov --rc lcov_branch_coverage=1 --capture --directory $(OBJDIR_DEBUG) --output-file $(DEBUGDIR)/$(CODECOVERAGEDIR)/coverage.info
	genhtml $(DEBUGDIR)/$(CODECOVERAGEDIR)/coverage.info --branch-coverage --output-directory $(DEBUGDIR)/$(CODECOVERAGEDIR)


$(OBJDIR_RELEASE)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	g++ -I$(INCDIR) -O3 $(CXXFLAGS) -MF$(@:%.o=%.d) -MT$@ -o $@ $<

$(OBJDIR_DEBUG)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	g++ -I$(INCDIR) -O0 -g3 $(CXXFLAGS) -MF$(@:%.o=%.d) -MT$@ -o $@ $<

-include $(DEPFILES)
