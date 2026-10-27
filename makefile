# ==========================================
# SOFTWARE PACKAGE DEPENDENCY RESOLVER
# ==========================================

CC      ?= gcc

CSTD    := -std=c11

WARN    := -Wall -Wextra -Wpedantic \
           -Wconversion \
           -Wsign-conversion \
           -Wshadow \
           -Wpointer-arith \
           -Wundef

CFLAGS  ?= -O2 -g

INC     := -I.

ALL     := $(CSTD) $(WARN) $(CFLAGS) $(INC)

BUILD   := build

APP     := $(BUILD)/resolver

SRC     := main.c package_manager.c

HDR     := package_manager.h

TEST    := $(BUILD)/test_package_manager

.PHONY: all run test sanitize valgrind helgrind cppcheck coverage debug clean

# ==========================================
# Default Target
# ==========================================

all: $(APP)

# ==========================================
# Create Build Folder
# ==========================================

$(BUILD):
	mkdir -p $(BUILD)

# ==========================================
# Main Application Build
# ==========================================

$(APP): $(SRC) $(HDR) | $(BUILD)
	$(CC) $(ALL) -o $@ $(SRC)

# ==========================================
# Run Application
# ==========================================

run: $(APP)
	./$(APP)

# ==========================================
# Unit Test Build
# ==========================================

$(TEST): test_package_manager.c package_manager.c package_manager.h | $(BUILD)
	$(CC) $(ALL) -o $@ test_package_manager.c package_manager.c

# ==========================================
# Run Unit Tests
# ==========================================

test: $(TEST)
	$(TEST)

# ==========================================
# Debug Build
# ==========================================

debug:
	$(CC) -g $(WARN) $(SRC) -o $(BUILD)/resolver_debug

# ==========================================
# Address Sanitizer
# ==========================================

sanitize: | $(BUILD)
	$(CC) $(CSTD) $(WARN) \
	      -O1 -g \
	      -fsanitize=address,undefined \
	      -fno-omit-frame-pointer \
	      $(SRC) \
	      -o $(BUILD)/resolver_san

	$(BUILD)/resolver_san

# ==========================================
# Valgrind Memory Analysis
# ==========================================

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./resolver

# ==========================================
# Helgrind Thread Analysis
# ==========================================

helgrind:
	valgrind --tool=helgrind ./resolver

# ==========================================
# CPPCheck Static Analysis
# ==========================================

cppcheck:
	cppcheck --enable=warning,style,portability --std=c99 --platform=unix64 main.c package_manager.c
	cppcheck --enable=all --std=c99 main.c package_manager.c > cppcheck_report.txt 2>&1
	cat cppcheck_report.txt

# ==========================================
# Code Coverage
# ==========================================

coverage:
	gcc -O0 --coverage -c package_manager.c -o package_manager.o
	g++ -std=c++17 -O0 --coverage -x c++ -c test_package_manager.c -o test_package_manager.o
	g++ --coverage package_manager.o test_package_manager.o -lgtest_main -lgtest -pthread -o package_manager_test

# ==========================================
# Clean Files
# ==========================================

clean:
	rm -rf $(BUILD)
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov
	rm -f packages.txt
	rm -f resolver.log
