DEPDIR=deps

CXX=g++-7
PEDANTIC_CFLAGS=-Wall -Wextra -Wold-style-cast -Werror -pedantic
CXXFLAGS=-std=c++1z -g $(PEDANTIC_CFLAGS)
TEST_CXXFLAGS=-std=c++1z -Werror -I/usr/local/include

SRCS := $(shell find baduk -name '*.cpp')
APP_SRCS := $(shell find apps -name '*.cpp')
OBJS := ${SRCS:.cpp=.o}
APP_OBJS := ${APP_SRCS:.cpp=.o}
APPS := apps/demo

.PHONY: all
all: $(APPS)

.PHONY: clean
clean:
	find baduk/ -name \*.o -delete
	find tests/ -name \*.o -delete
	rm -f tests/tests.cpp
	rm -f $(APPS)
	rm -f out/testrunner

-include $(DEPDIR)/*.d

.PHONY: test
test: out/testrunner
	out/testrunner

out/testrunner: tests/tests.cpp $(OBJS)
	$(CXX) $(TEST_CXXFLAGS) -o out/testrunner tests/tests.cpp $(OBJS)

tests/tests.cpp: tests/*.h
	cxxtestgen --error-printer -o tests/tests.cpp tests/*.h

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ -MT $@ -MF $(DEPDIR)/$(subst /,__,$@).d

apps/demo: $(OBJS) $(APP_OBJS)
	$(CXX) $(LDFLAGS) -o apps/demo $(OBJS) apps/demo.o
