# Лабораторная работа №3. Сборка: make [release | debug | run | clean]

CC     = gcc
CFLAGS = -std=c99 -Wall -Werror -Wpedantic -Wextra -Wvla -Iinc
LDLIBS = -lm

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
APP = app.exe

# по умолчанию собираем release
all: release

# make release - оптимизированная сборка
release: CFLAGS += -O2
release: clean $(APP)

# make debug - сборка с отладочной информацией
debug: CFLAGS += -g -O0
debug: clean $(APP)

# make run - собрать (если нужно) и запустить
run: $(APP)
	./$(APP)

# make clean - удалить результаты сборки
clean:
	rm -rf build $(APP)

$(APP): $(OBJ)
	$(CC) $^ -o $@ $(LDLIBS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

.PHONY: all release debug run clean
