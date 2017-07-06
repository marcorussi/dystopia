# A sample Makefile
# This Makefile demonstrates and explains 
# Make Macros, Macro Expansions,
# Rules, Targets, Dependencies, Commands, Goals
# Artificial Targets, Pattern Rule, Dependency Rule.

# Comments start with a # and go to the end of the line.

# Here is a simple Make Macro.
LINK_TARGET = test.exe

#CURLPP_PATH := /home/marco/Documents/projects/http_curlpp/curlpp-0.8.1/

LIB_PATH := /usr/local/lib

# Here is a Make Macro that uses the backslash to extend to multiple lines.
OBJS = json11.o main.o 

CXX_FLAGS = -std=c++11
#-fno-rtti -fno-exceptions

LD_FLAGS = -lcurlpp -lcurl

#CANARY_ARGS = -DJSON11_ENABLE_DR1467_CANARY=0

# Here is a Make Macro defined by two Macro Expansions.
# A Macro Expansion may be treated as a textual replacement of the Make Macro.
# Macro Expansions are introduced with $ and enclosed in (parentheses).
REBUILDABLES = $(OBJS) $(LINK_TARGET)

#INC_PATHS = -I$(abspath /home/marco/Documents/projects/http_curlpp/json11-master)

LIB_PATHS = -L$(abspath $(LIB_PATH))

# There are two standard Targets your Makefile should probably have:
# "all" and "clean", because they are often command-line Goals.
# Also, these are both typically Artificial Targets, because they don't typically
# correspond to real files named "all" or "clean".  

# The rule for "all" is used to incrementally build your system.
# It does this by expressing a dependency on the results of that system,
# which in turn have their own rules and dependencies.
all : $(LINK_TARGET)
	echo done

# Here is a simple Rule (used for "cleaning" your build environment).
# It has a Target named "clean" (left of the colon ":" on the first line),
# no Dependencies (right of the colon),
# and two Commands (indented by tabs on the lines that follow).
# The space before the colon is not required but added here for clarity.
clean :
	rm -f $(REBUILDABLES)
	echo clean

# There is no required order to the list of rules as they appear in the Makefile.
# Make will build its own dependency tree and only execute each rule only once
# its dependencies' rules have been executed successfully.

# Here is a Rule that uses some built-in Make Macros in its command:
# $@ expands to the rule's target, in this case "test_me.exe".
# $^ expands to the rule's dependencies, in this case the three files
# main.o, test1.o, and  test2.o.
$(LINK_TARGET) : $(OBJS)
	g++ -o $@ $^ $(LIB_PATHS) $(LD_FLAGS) 

# Here is a Pattern Rule, often used for compile-line.
# It says how to create a file with a .o suffix, given a file with a .cpp suffix.
# The rule's command uses some built-in Make Macros:
# $@ for the pattern-matched target
# $< for the pattern-matched dependency
%.o : %.cpp
#	g++ -c -o $@ $< $(CANARY_ARGS) $(CXX_FLAGS) $(INC_PATHS)
	g++ -c -o $@ $< $(CXX_FLAGS)

json11.o :
#	g++ -c -o json11.o ../json11-master/json11.cpp -std=c++11
	g++ -c -o json11.o json11.cpp -std=c++11

# These are Dependency Rules, which are rules without any command.
# Dependency Rules indicate that if any file to the right of the colon changes,
# the target to the left of the colon should be considered out-of-date.
# The commands for making an out-of-date target up-to-date may be found elsewhere
# (in this case, by the Pattern Rule above).
# Dependency Rules are often used to capture header file dependencies.
#main.o : test01.h test02.h

# Alternatively to manually capturing dependencies, several automated
# dependency generators exist.  Here is one possibility (commented out)...
# %.dep : %.cpp
#   g++ -M $(FLAGS) $< > $@
# include $(OBJS:.o=.dep)
