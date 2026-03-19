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


ZLIB_SRCS = \
	../../Dependencies/zlib/adler32.c \
	../../Dependencies/zlib/compress.c \
	../../Dependencies/zlib/crc32.c \
	../../Dependencies/zlib/deflate.c \
	../../Dependencies/zlib/gzclose.c \
	../../Dependencies/zlib/gzlib.c \
	../../Dependencies/zlib/gzread.c \
	../../Dependencies/zlib/gzwrite.c \
	../../Dependencies/zlib/infback.c \
	../../Dependencies/zlib/inffast.c \
	../../Dependencies/zlib/inflate.c \
	../../Dependencies/zlib/inftrees.c \
	../../Dependencies/zlib/trees.c \
	../../Dependencies/zlib/uncompr.c \
	../../Dependencies/zlib/zutil.c
ZLIB_OBJS = $(patsubst ../../Dependencies/zlib/%.c,Build/Objects/Dependencies/zlib/%.o,$(ZLIB_SRCS))

GOOGLE_BENCHMARK_INCLUDE_PATHS = \
	../../Dependencies/benchmark/include/
GOOGLE_BENCHMARK_INCLUDE_PATHS_FLAGS = $(foreach d, $(GOOGLE_BENCHMARK_INCLUDE_PATHS), -I$d)

GOOGLE_BENCHMARK_SRCS = \
	../../Dependencies/benchmark/src/benchmark.cc \
	../../Dependencies/benchmark/src/benchmark_api_internal.cc \
	../../Dependencies/benchmark/src/benchmark_name.cc \
	../../Dependencies/benchmark/src/benchmark_register.cc \
	../../Dependencies/benchmark/src/benchmark_runner.cc \
	../../Dependencies/benchmark/src/check.cc \
	../../Dependencies/benchmark/src/colorprint.cc \
	../../Dependencies/benchmark/src/commandlineflags.cc \
	../../Dependencies/benchmark/src/complexity.cc \
	../../Dependencies/benchmark/src/console_reporter.cc \
	../../Dependencies/benchmark/src/counter.cc \
	../../Dependencies/benchmark/src/csv_reporter.cc \
	../../Dependencies/benchmark/src/json_reporter.cc \
	../../Dependencies/benchmark/src/perf_counters.cc \
	../../Dependencies/benchmark/src/reporter.cc \
	../../Dependencies/benchmark/src/statistics.cc \
	../../Dependencies/benchmark/src/string_util.cc \
	../../Dependencies/benchmark/src/sysinfo.cc \
	../../Dependencies/benchmark/src/timers.cc
GOOGLE_BENCHMARK_OBJS = \
	$(patsubst ../../Dependencies/benchmark/src/%.cc,Build/Objects/Dependencies/benchmark/%.o,$(GOOGLE_BENCHMARK_SRCS))


CORRECTNESS_TESTS_PROGRAM = Build/CorrectnessTests

CORRECTNESS_TESTS_INCLUDE_PATHS = \
	../../Dependencies/zlib
CORRECTNESS_TESTS_INCLUDE_PATHS_FLAGS = $(foreach d, $(CORRECTNESS_TESTS_INCLUDE_PATHS), -I$d)

CORRECTNESS_TESTS_SRCS = \
	../../Code/BitReader.cpp \
	../../Code/CorrectnessTestsEntryPoint.cpp \
	../../Code/NaiveDeflate.cpp \
	../../Code/NaiveHuffman.cpp \
	../../Code/NaiveLempelZiv.cpp \
	../../Code/NaiveZlib.cpp
CORRECTNESS_TESTS_OBJS = $(patsubst ../../Code/%.cpp,Build/Objects/Code/%.o,$(CORRECTNESS_TESTS_SRCS))



PERFORMANCE_TESTS_INCLUDE_PATHS = \
	../../Dependencies/benchmark/include/
PERFORMANCE_TESTS_INCLUDE_PATHS_FLAGS = $(foreach d, $(PERFORMANCE_TESTS_INCLUDE_PATHS), -I$d)


PERFORMANCE_TESTS_PROGRAM = Build/PerformanceTests

PERFORMANCE_TESTS_SRCS = \
	../../Code/PerformanceTestsEntryPoint.cpp \
	../../Code/NaiveHuffman.cpp \
	../../Code/NaiveLempelZiv.cpp
PERFORMANCE_TESTS_OBJS = \
	$(patsubst ../../Code/%.cpp,Build/Objects/Code/%.o,$(PERFORMANCE_TESTS_SRCS))


ifeq ($(IS_64_BIT), true)
	TARGET_BIT_FLAG = -m64
else
	TARGET_BIT_FLAG = -m32
endif

CPPFLAGS += -std=c++23 -Werror -Wall -Wextra $(TARGET_BIT_FLAG)
# TODO: Add -Wpedantic -Wconversion -Wsign-conversion
GOOGLE_BENCHMARK_CPPFLAGS = $(CPPFLAGS) $(GOOGLE_BENCHMARK_INCLUDE_PATHS_FLAGS) -DBENCHMARK_STATIC_DEFINE
CORRECTNESS_TESTS_CPPFLAGS = $(CPPFLAGS) $(CORRECTNESS_TESTS_INCLUDE_PATHS_FLAGS)
PERFORMANCE_TESTS_CPPFLAGS = $(CPPFLAGS) $(PERFORMANCE_TESTS_INCLUDE_PATHS_FLAGS) -DBENCHMARK_STATIC_DEFINE
LINKER_FLAGS += $(LIBRARY_PATHS_FLAGS)




all: $(CORRECTNESS_TESTS_PROGRAM) $(PERFORMANCE_TESTS_PROGRAM)

$(CORRECTNESS_TESTS_PROGRAM): $(CORRECTNESS_TESTS_OBJS) $(ZLIB_OBJS)
	$(CXX) $(TARGET_BIT_FLAG) -g $(CORRECTNESS_TESTS_OBJS) $(ZLIB_OBJS) $(LINKER_FLAGS) -o CorrectnessTests

$(CORRECTNESS_TESTS_OBJS): $(CORRECTNESS_TESTS_SRCS)
	mkdir -p $(dir $@)
	$(CXX) -g $(CORRECTNESS_TESTS_CPPFLAGS) -c $(patsubst Build/Objects/Code/%.o,../../Code/%.cpp,$@) -o $@

$(PERFORMANCE_TESTS_PROGRAM): $(PERFORMANCE_TESTS_OBJS) $(GOOGLE_BENCHMARK_OBJS) $(ZLIB_OBJS)
	$(CXX) $(TARGET_BIT_FLAG) -g $(PERFORMANCE_TESTS_OBJS) $(GOOGLE_BENCHMARK_OBJS) $(ZLIB_OBJS) $(LINKER_FLAGS) -o PerformanceTests

$(PERFORMANCE_TESTS_OBJS): $(PERFORMANCE_TESTS_SRCS)
	mkdir -p $(dir $@)
	$(CXX) -g $(PERFORMANCE_TESTS_CPPFLAGS) -c $(patsubst Build/Objects/Code/%.o,../../Code/%.cpp,$@) -o $@

$(GOOGLE_BENCHMARK_OBJS): $(GOOGLE_BENCHMARK_SRCS)
	mkdir -p $(dir $@)
	$(CXX) -g $(GOOGLE_BENCHMARK_CPPFLAGS) -c $(patsubst Build/Objects/Dependencies/benchmark/%.o,../../Dependencies/benchmark/src/%.cc,$@) -o $@

$(ZLIB_OBJS): $(ZLIB_SRCS)
	mkdir -p $(dir $@)
	$(CC) -g -c $(patsubst Build/Objects/Dependencies/zlib/%.o,../../Dependencies/zlib/%.c,$@) -o $@

.PHONY: clean
distclean: clean
clean:
	for file in $(CORRECTNESS_TESTS_PROGRAM) $(CORRECTNESS_TESTS_OBJS) $(PERFORMANCE_TESTS_PROGRAM) $(PERFORMANCE_TESTS_OBJS) $(GOOGLE_BENCHMARK_OBJS) $(ZLIB_OBJS) ; do \
		rm $${file} ; \
	done
