# queue-traffic
#
#   make          -> compila ./app
#   make run      -> compila e executa
#   make debug    -> recompila com AddressSanitizer/UBSan (detecta vazamentos)
#   make clean    -> remove artefatos de build

CXX      ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -pedantic
CPPFLAGS += -Iinclude

SRCS := $(shell find src -name '*.cpp') main.cpp
OBJS := $(SRCS:%.cpp=build/%.o)
BIN  := app

DEBUG_FLAGS := -std=c++20 -O0 -g -Wall -Wextra -pedantic -fsanitize=address,undefined

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(OBJS:.o=.d)

run: $(BIN)
	./$(BIN)

debug:
	$(MAKE) clean
	$(MAKE) CXXFLAGS="$(DEBUG_FLAGS)" LDFLAGS="-fsanitize=address,undefined"

clean:
	rm -rf build $(BIN)

.PHONY: all run debug clean
