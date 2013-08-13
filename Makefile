# The system C compiler
CC = clang

# Compiler flags: -Wall turns on most, but not all, compiler warnings
CFLAGS = -Wall

# The build target executable name
TARGET = rrc

SRC_DIR = ./RemoteControlC
OUT_DIR = .
all: main

main: resources getopt.o mongoose.o routes.o mouse.o network.o $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -o "$(OUT_DIR)/$(TARGET)" "$(OUT_DIR)/mongoose.o" "$(OUT_DIR)/getopt.o" "$(OUT_DIR)/routes.o" "$(OUT_DIR)/mouse.o" "$(OUT_DIR)/network.o"  "$(SRC_DIR)/main.c" -ldl -lpthread

mongoose.o: $(SRC_DIR)/mongoose.c $(SRC_DIR)/mongoose.h
	$(CC) $(CFLAGS) -c -o "$(OUT_DIR)/mongoose.o" "$(SRC_DIR)/mongoose.c"

getopt.o: $(SRC_DIR)/getopt.c $(SRC_DIR)/getopt.h
	$(CC) $(CFLAGS) -c -o "$(OUT_DIR)/getopt.o" "$(SRC_DIR)/getopt.c"

mouse.o: $(SRC_DIR)/mouse.c $(SRC_DIR)/mouse.h
	$(CC) $(CFLAGS) -c -o "$(OUT_DIR)/mouse.o" "$(SRC_DIR)/mouse.c"

network.o: $(SRC_DIR)/network.c $(SRC_DIR)/network.h
	$(CC) $(CFLAGS) -c -o "$(OUT_DIR)/network.o" "$(SRC_DIR)/network.c"

routes.o: $(SRC_DIR)/routes.c $(SRC_DIR)/routes.h
	$(CC) $(CFLAGS) -c -o "$(OUT_DIR)/routes.o" "$(SRC_DIR)/routes.c"

resources: $(SRC_DIR)/resources
	cp -r "$(SRC_DIR)/resources" $(OUT_DIR)

clean:
	$(RM) $(OUT_DIR)/$(TARGET)
	$(RM) $(OUT_DIR)/network.o
	$(RM) $(OUT_DIR)/mouse.o
	$(RM) $(OUT_DIR)/mongoose.o
	$(RM) $(OUT_DIR)/routes.o
	$(RM) $(OUT_DIR)/getopt.o
	$(RM) -r  $(OUT_DIR)/resources
