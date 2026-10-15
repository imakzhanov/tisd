# Лабораторная работа 3
#   make app    - программа app.exe
#   make bench  - замеры времени и памяти
#   make debug  - с отладочной информацией
#   make clean  - удалить результаты сборки

CC = gcc
CFLAGS = -std=c99 -Wall -Werror -Wpedantic -Wextra -Wvla -Iinc -O0
LDLIBS = -lm

SRC = $(filter-out src/bench.c,$(wildcard src/*.c))
OBJ = $(SRC:src/%.c=build/%.o)
BENCH_OBJ = build/bench.o build/struct_func.o
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

build/%.o : src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(wildcard build/*.d)

.PHONY: all app bench debug clean
