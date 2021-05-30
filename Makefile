.PHONY : vm_lib all clean test create_dirs
BUILD_DIR = build
OUTPUT_DIR = output
INCLUDE_DIR = include
VM_LIB = vm.so

all : create_dirs test vm_lib

test : test.c vm_lib
	gcc test.c -I$(INCLUDE_DIR) -o $(OUTPUT_DIR)/run

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