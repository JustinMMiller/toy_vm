.PHONY : vm_lib all clean test create_dirs
BUILD_DIR = build
OUTPUT_DIR = output
INCLUDE_DIR = include
VM_LIB = libvm.so

all : create_dirs test vm_lib

run_tests : all
	cd $(OUTPUT_DIR) && LD_LIBRARY_PATH=. ./run

test : test.c vm_lib
	gcc -g test.c -I$(INCLUDE_DIR) -L./$(OUTPUT_DIR) -lvm -o $(OUTPUT_DIR)/run

vm_lib : src/vm.c src/opcodes.c
	gcc -c -fPIC src/vm.c -I$(INCLUDE_DIR) -o $(BUILD_DIR)/vm.o
	gcc -c -fPIC src/opcodes.c -I$(INCLUDE_DIR) -o $(BUILD_DIR)/opcodes.o
	gcc -shared $(BUILD_DIR)/vm.o $(BUILD_DIR)/opcodes.o -o $(OUTPUT_DIR)/$(VM_LIB)

create_dirs :
	if [ -d $(BUILD_DIR) ]; then echo ; else mkdir -p $(BUILD_DIR); fi
	if [ -d $(OUTPUT_DIR) ]; then echo ; else mkdir -p $(OUTPUT_DIR); fi

clean :
	rm -rf $(BUILD_DIR)
	rm -rf $(OUTPUT_DIR)