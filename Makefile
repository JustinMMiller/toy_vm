.PHONY : vm_lib all clean test create_dirs
BUILD_DIR = build
OUTPUT_DIR = output
INCLUDE_DIR = include
VM_LIB = libvm.so
CC = gcc

all : create_dirs test vm_lib loader assembler test

run_tests : all test
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=. ./test

run_loader : create_dirs loader
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=. ./loader ../test.bin

test : test.c vm_lib
	$(CC) -g test.c -Wall -I$(INCLUDE_DIR) -L./$(OUTPUT_DIR) -lvm -o $(OUTPUT_DIR)/$@

loader : vm_lib src/loader.c
	$(CC) -g src/loader.c -Wall -I$(INCLUDE_DIR) -L./$(OUTPUT_DIR) -lvm -o $(OUTPUT_DIR)/$@

assembler : vm_lib src/assembler.c src/parser.c
	$(CC) -g src/assembler.c src/parser.c -Wall -I$(INCLUDE_DIR) -L./$(OUTPUT_DIR) -lvm -o $(OUTPUT_DIR)/$@

vm_lib : src/vm.c src/opcodes.c
	$(CC) -c -fPIC src/vm.c -Wall -I$(INCLUDE_DIR) -o $(BUILD_DIR)/vm.o
	$(CC) -c -fPIC src/opcodes.c -Wall -I$(INCLUDE_DIR) -o $(BUILD_DIR)/opcodes.o
	$(CC) -shared $(BUILD_DIR)/vm.o $(BUILD_DIR)/opcodes.o -o $(OUTPUT_DIR)/$(VM_LIB)

create_dirs :
	if [ -d $(BUILD_DIR) ]; then echo ; else mkdir -p $(BUILD_DIR); fi
	if [ -d $(OUTPUT_DIR) ]; then echo ; else mkdir -p $(OUTPUT_DIR); fi

clean :
	rm -rf $(BUILD_DIR)
	rm -rf $(OUTPUT_DIR)