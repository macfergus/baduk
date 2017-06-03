DEPDIR=deps

CXX=g++
PEDANTIC_CFLAGS=-Wall -Wextra -Wold-style-cast -Werror -pedantic
CXXFLAGS=-std=c++1y -g $(PEDANTIC_CFLAGS)
TEST_CXXFLAGS=-std=c++1y -Werror -I/usr/local/include

SRCS := $(shell find baduk -name '*.cpp')
APP_SRCS := $(shell find apps -name '*.cpp')
OBJS := ${SRCS:.cpp=.o}
APP_OBJS := ${APP_SRCS:.cpp=.o}
APPS := apps/demo

.PHONY: all
all: $(APPS)

-include $(DEPDIR)/*.d

.PHONY: test
test: out/testrunner
	out/testrunner

out/testrunner: tests/tests.cpp
	$(CXX) $(TEST_CXXFLAGS) -o out/testrunner tests/tests.cpp $(OBJS)

tests/tests.cpp: tests/*.h
	cxxtestgen --error-printer -o tests/tests.cpp tests/*.h

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ -MT $@ -MF $(DEPDIR)/$(subst /,__,$@).d

apps/demo: $(OBJS) $(APP_OBJS)
	$(CXX) $(LDFLAGS) -o apps/demo $(OBJS) apps/demo.o
