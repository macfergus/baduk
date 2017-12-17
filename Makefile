DEPDIR=deps

CXX=g++-7
PEDANTIC_CFLAGS=-Wall -Wextra -Wold-style-cast -Werror -pedantic
CXXFLAGS=-O3 -std=c++1z -g $(PEDANTIC_CFLAGS)
TEST_CXXFLAGS=-std=c++1z -Werror -I/usr/local/include
LDFLAGS=-L/usr/local/Cellar//gperftools/2.6.1/lib/ -lprofiler

SRCS := $(shell find cppsrc/baduk -name '*.cpp')
APP_SRCS := $(shell find cppsrc/apps -name '*.cpp')
OBJS := ${SRCS:.cpp=.o}
APP_OBJS := ${APP_SRCS:.cpp=.o}
APPS := cppsrc/apps/randomplay cppsrc/apps/demo

TESTDIR = cppsrc/tests

.PHONY: all
all: $(APPS)

.PHONY: clean
clean:
	find cppsrc/baduk/ -name \*.o -delete
	find $(TESTDIR) -name \*.o -delete
	find cppsrc/apps/ -name \*.o -delete
	rm -f $(TESTDIR)/tests.cpp
	rm -f out/testrunner
	rm -f $(APPS)

-include $(DEPDIR)/*.d

.PHONY: test
test: out/testrunner
	out/testrunner

out/testrunner: $(TESTDIR)/tests.cpp $(OBJS)
	$(CXX) $(TEST_CXXFLAGS) -o out/testrunner $(TESTDIR)/tests.cpp $(OBJS)

$(TESTDIR)/tests.cpp: $(TESTDIR)/*.h
	cxxtestgen --error-printer -o $(TESTDIR)/tests.cpp $(TESTDIR)/*.h

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ -MT $@ -MF $(DEPDIR)/$(subst /,__,$@).d

cppsrc/apps/randomplay: $(OBJS) $(APP_OBJS)
	$(CXX) $(LDFLAGS) -o cppsrc/apps/randomplay $(OBJS) cppsrc/apps/randomplay.o

cppsrc/apps/demo: $(OBJS) $(APP_OBJS)
	$(CXX) $(LDFLAGS) -o cppsrc/apps/demo $(OBJS) cppsrc/apps/demo.o
