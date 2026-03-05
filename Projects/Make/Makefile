IS_64_BIT = false
ifeq ($(shell getconf LONG_BIT), 64)
	IS_64_BIT = true
else
	IS_64_BIT = false
endif

IS_X86 = false
IS_ARM = false
ifneq (,$(findstring $(shell uname -m), x86_64 amd64))
	IS_X86 = true
else ifneq (,$(findstring $(shell uname -m), aarch64 arm64)))
	IS_ARM = true
endif

# Default overrides which can be provided by environment variables
ifeq (1, $(64_BIT))
	IS_64_BIT = true
endif
ifeq (1, $(32_BIT))
	IS_64_BIT = false
endif



CORRECTNESS_TESTS_PROGRAM = Build/CorrectnessTests

CORRECTNESS_TESTS_CXX_SRCS = \
	../../Code/CorrectnessTestsEntryPoint.cpp \
	../../Code/NaiveHuffman.cpp \
	../../Code/NaiveLZ77.cpp
CORRECTNESS_TESTS_CXX_OBJS = $(patsubst ../../Code/%.cpp,Build/Objects/%.o,$(CORRECTNESS_TESTS_CXX_SRCS))




PERFORMANCE_TESTS_INCLUDE_PATHS = \
	../../Dependencies/benchmark/include/
PERFORMANCE_TESTS_INCLUDE_PATHS_FLAGS = $(foreach d, $(PERFORMANCE_TESTS_INCLUDE_PATHS), -I$d)


PERFORMANCE_TESTS_PROGRAM = Build/PerformanceTests

PERFORMANCE_TESTS_CXX_SRCS = \
	../../Code/PerformanceTestsEntryPoint.cpp \
	../../Code/NaiveHuffman.cpp \
	../../Code/NaiveLZ77.cpp
PERFORMANCE_TESTS_CXX_OBJS = $(patsubst ../../Code/%.cpp,Build/Objects/%.o,$(PERFORMANCE_TESTS_CXX_SRCS))

ifeq ($(IS_64_BIT), true)
	TARGET_BIT_FLAG = -m64
else
	TARGET_BIT_FLAG = -m32
endif

CPPFLAGS += -std=c++23 -Werror -Wall -Wextra $(TARGET_BIT_FLAG)
# TODO: Add -Wpedantic -Wconversion -Wsign-conversion
PERFORMANCE_TESTS_CPPFLAGS = $(CPPFLAGS) $(PERFORMANCE_TESTS_INCLUDE_PATHS_FLAGS) -DBENCHMARK_STATIC_DEFINE
LINKER_FLAGS += $(LIBRARY_PATHS_FLAGS)




all: $(CORRECTNESS_TESTS_PROGRAM) $(PERFORMANCE_TESTS_PROGRAM)

$(CORRECTNESS_TESTS_PROGRAM): $(CORRECTNESS_TESTS_CXX_OBJS)
	$(CXX) $(TARGET_BIT_FLAG) -g $(CORRECTNESS_TESTS_CXX_OBJS) $(LINKER_FLAGS) -o CorrectnessTests

$(CORRECTNESS_TESTS_CXX_OBJS): $(CORRECTNESS_TESTS_CXX_SRCS)
	mkdir -p $(dir $@)
	$(CXX) -g $(CPPFLAGS) -c $(patsubst Build/Objects/%.o,../../Code/%.cpp,$@) -o $@

$(PERFORMANCE_TESTS_PROGRAM): $(PERFORMANCE_TESTS_CXX_OBJS)
	$(CXX) $(TARGET_BIT_FLAG) -g $(PERFORMANCE_TESTS_CXX_OBJS) $(LINKER_FLAGS) -o PerformanceTests

$(PERFORMANCE_TESTS_CXX_OBJS): $(PERFORMANCE_TESTS_CXX_SRCS)
	mkdir -p $(dir $@)
	$(CXX) -g $(PERFORMANCE_TESTS_CPPFLAGS) -c $(patsubst Build/Objects/%.o,../../Code/%.cpp,$@) -o $@

.PHONY: clean
distclean: clean
clean:
	for file in $(CORRECTNESS_TESTS_PROGRAM) $(CORRECTNESS_TESTS_CXX_OBJS) $(PERFORMANCE_TESTS_PROGRAM) $(PERFORMANCE_TESTS_CXX_OBJS) ; do \
		rm $${file} ; \
	done
