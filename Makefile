DEPDIR=deps

CXX=g++
PEDANTIC_CFLAGS=-Wall -Wextra -Wold-style-cast -Werror -pedantic
CXXFLAGS=-std=c++1y -g $(PEDANTIC_CFLAGS)

SRCS := $(shell find . -name '*.cpp')
OBJS := ${SRCS:.cpp=.o}

BIN=demo

.PHONY: all
all: $(BIN)

-include $(DEPDIR)/*.d

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ -MT $@ -MF $(DEPDIR)/$(subst /,__,$@).d

$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJS)
