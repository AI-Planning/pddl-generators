HEADERS = solver.h open_list.h search_space.h state.h globals.h successor_generator.h

SOURCES = $(HEADERS:%.h=%.cc)
OBJECTS = $(SOURCES:%.cc=%.o)
PROFILE_OBJECTS = $(SOURCES:%.cc=%.profile.o)
RELEASE_OBJECTS = $(SOURCES:%.cc=%.release.o)

CC = g++
DEPEND = g++ -MM
CCOPT =      -Wall -W -Wno-sign-compare -ansi -pedantic -O3 -Wno-deprecated
RELEASEOPT = -Wall -W -Wno-sign-compare -ansi -pedantic -DNDEBUG -O3 -m32 -Wno-deprecated
LINKOPT =

## debug, don't optimize
CCOPT += -g
LINKOPT += -g

## debug, optimize
# CCOPT += -O3 -g
# LINKOPT += -g

## no debug, optimize a lot
#CCOPT += -O3 -DNDEBUG -march=athlon-xp

PROFOPT = -pg -O2 -DNDEBUG

PROFILE_ARGS = fFxXo fF fFxX fFxXD xX
PROFILE_TASK = driverlog/pfile20

PROFILES = $(PROFILE_ARGS:%=PROFILE-%)

# PROFLINKOPT = -lc_p

vpath %.o obj

search: $(OBJECTS)
	$(CC) $(LINKOPT) $(OBJECTS:%=obj/%) -o nomystery

profile-search: $(PROFILE_OBJECTS)
	$(CC) $(LINKOPT) $(PROFLINKOPT) $(PROFOPT) $(PROFILE_OBJECTS:%=obj/%) \
          -o profile-search

release: release-search

release-search: $(RELEASE_OBJECTS)
	$(CC) $(RELEASEOPT) $(RELEASE_OBJECTS:%=obj/%) \
	  -o release-search

PROFILES = $(PROFILE_ARGS:%=PROFILE-%)

$(PROFILES): PROFILE-%: profile-search
	cp ../../results/translate/$(PROFILE_TASK)/all.groups .
	./profile-search $* < ../../results/preprocess/$(PROFILE_TASK)/output
	gprof profile-search | ./cleanup-profile > $@

$(OBJECTS): %.o: %.cc
	mkdir -p obj
	$(CC) $(CCOPT) -c $< -o obj/$@

$(PROFILE_OBJECTS): %.profile.o: %.cc
	mkdir -p obj
	$(CC) $(CCOPT) $(PROFOPT) -c $< -o obj/$@

$(RELEASE_OBJECTS): %.release.o: %.cc
	mkdir -p obj
	$(CC) $(RELEASEOPT) -c $< -o obj/$@

clean:
	rm -f $(OBJECTS:%=obj/%) $(PROFILE_OBJECTS:%=obj/%) $(RELEASE_OBJECTS:%=obj/%) *~ *.pyc
	rm -f Makefile.depend profile-search gmon.out PROFILE core
	rm -f sas_plan

veryclean: clean
	rm -f nomystery

Makefile.depend: $(SOURCES) $(HEADERS)
	$(DEPEND) $(SOURCES) > Makefile.temp
	sed -e "s/\(.*\)\.o:\(.*\)/\1.profile.o:\2/" Makefile.temp >> Makefile.temp2
	cat Makefile.temp Makefile.temp2 > Makefile.depend
	rm -f Makefile.temp Makefile.temp2

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),veryclean)
-include Makefile.depend
endif
endif

.PHONY: release clean veryclean
