# Лабораторная работа 4
#   make app    - программа app.exe
#   make bench  - замеры времени и памяти (bench.exe)
#   make debug  - с отладочной информацией
#   make clean  - удалить результаты сборки

CC = gcc
CFLAGS = -std=c99 -Wall -Werror -Wpedantic -Wextra -Wvla -Iinc -O0
LDLIBS = -lm

# размер массива в bench, чтобы замерять большие стеки
BENCH_STACK_SIZE = 10000

SRC = $(filter-out src/bench.c src/tmp_%.c,$(wildcard src/*.c))
OBJ = $(SRC:src/%.c=build/%.o)
BENCH_OBJ = build/bench/bench.o build/bench/array_stack.o build/bench/list_stack.o build/bench/errors.o
APP = app.exe
BENCH = bench.exe

all: app

app: $(APP)

bench: $(BENCH)

debug: CFLAGS = -std=c99 -Wall -Werror -Wpedantic -Wextra -Wvla -Iinc -g -O0
debug: clean app

clean:
	rm -rf build
	rm -f $(APP) $(BENCH)

$(APP): $(OBJ)
	$(CC) $^ -o $@ $(LDLIBS)

$(BENCH): $(BENCH_OBJ)
	$(CC) $^ -o $@ $(LDLIBS)

build/bench/%.o : src/%.c
	mkdir -p build/bench
	$(CC) $(CFLAGS) -DSTACK_SIZE=$(BENCH_STACK_SIZE) -MMD -MP -c $< -o $@

build/%.o : src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(wildcard build/*.d build/bench/*.d)

.PHONY: all app bench debug clean
