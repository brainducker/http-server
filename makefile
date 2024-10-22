SOURCES_DIR=src
BUILD_DIR=build
OUTPUT_DIR=bin

CC=gcc
CCFLAGS=-g
ALL_CCFLAGS=$(CCFLAGS) -Wall -Wextra -Werror
LDFLAGS=-lws2_32

TARGET=$(OUTPUT_DIR)/server.exe
HFILES=$(wildcard $(SOURCES_DIR)/*.h)
CPPFILES=$(wildcard $(SOURCES_DIR)/*.c)
OBJFILES=$(subst $(SOURCES_DIR)/, $(BUILD_DIR)/, $(CPPFILES:.c=.o))

.SUFFIXES:
.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(OBJFILES) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SOURCES_DIR)/%.c $(HFILES)
	$(CC) $< -c -o $@ $(ALL_CCFLAGS)

run: $(TARGET)
	$(TARGET)

clean:
	del /q $(BUILD_DIR)\* $(OUTPUT_DIR)\*
