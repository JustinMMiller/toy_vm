.PHONY : vm_lib all clean test create_dirs
BUILD_DIR = build
OUTPUT_DIR = output
INCLUDE_DIR = include
VM_LIB = libvm.so
CC = gcc
DISABLED_WARNINGS = -Wno-char-subscripts
FLAGS = -Wall -O3 $(DISABLED_WARNINGS)

ifdef DEBUG
DEBUGGER = valgrind --leak-check=full
else
DEBUGGER = 
endif

all : create_dirs $(OUTPUT_DIR)/test $(OUTPUT_DIR)/$(VM_LIB) $(OUTPUT_DIR)/loader $(OUTPUT_DIR)/assembler $(OUTPUT_DIR)/test

run_tests : all $(OUTPUT_DIR)/test
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=. $(DEBUGGER) ./test

run_loader : create_dirs $(OUTPUT_DIR)/loader run_assembler
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=. $(DEBUGGER) ./loader ../test.bin

run_assembler : create_dirs $(OUTPUT_DIR)/loader
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=. $(DEBUGGER) ./assembler ../test.toy ../test.bin

$(OUTPUT_DIR)/test : test.c $(OUTPUT_DIR)/$(VM_LIB)
	$(CC) -g test.c $(FLAGS) -I$(INCLUDE_DIR) -L./$(OUTPUT_DIR) -lvm -o $@

$(OUTPUT_DIR)/loader : $(OUTPUT_DIR)/$(VM_LIB) src/loader.c
	$(CC) -g src/loader.c $(FLAGS) -I$(INCLUDE_DIR) -L./$(OUTPUT_DIR) -lvm -o $@

$(OUTPUT_DIR)/assembler : $(OUTPUT_DIR)/$(VM_LIB) src/assembler.c src/parser.c
	$(CC) -g src/assembler.c src/parser.c $(FLAGS) -I$(INCLUDE_DIR) -L./$(OUTPUT_DIR) -lvm -o $@

$(OUTPUT_DIR)/$(VM_LIB) : src/vm.c src/opcodes.c
	$(CC) -c -fPIC src/vm.c $(FLAGS) -I$(INCLUDE_DIR) -o $(BUILD_DIR)/vm.o
	$(CC) -c -fPIC src/opcodes.c $(FLAGS) -I$(INCLUDE_DIR) -o $(BUILD_DIR)/opcodes.o
	$(CC) -shared $(BUILD_DIR)/vm.o $(BUILD_DIR)/opcodes.o -o $@

create_dirs :
	@if [ -d $(BUILD_DIR) ]; then echo ; else mkdir -p $(BUILD_DIR); fi
	@if [ -d $(OUTPUT_DIR) ]; then echo ; else mkdir -p $(OUTPUT_DIR); fi

clean :
	rm -rf $(BUILD_DIR)
	rm -rf $(OUTPUT_DIR)
