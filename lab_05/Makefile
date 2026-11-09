# Лабораторная работа 5, вариант 2
# make app   — основная программа
# make bench — сравнение времени и памяти
# make clean — удаление результатов сборки

CC = gcc
CFLAGS = -std=c99 -Wall -Werror -Wpedantic -Wextra -Wvla -O0
CPPFLAGS = -Iinc
LDLIBS = -lm

COMMON_OBJ = build/arr_queue.o build/list_queue.o build/model.o build/io.o
APP_OBJ = build/main.o $(COMMON_OBJ)
BENCH_OBJ = build/bench.o $(COMMON_OBJ)

all: app

app: app.exe

bench: bench.exe

app.exe: $(APP_OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

bench.exe: $(BENCH_OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf build
	rm -f app.exe bench.exe

-include $(wildcard build/*.d)

.PHONY: all app bench clean
