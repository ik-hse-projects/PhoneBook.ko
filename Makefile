ifneq ($(KERNELRELEASE),)
obj-m += phonebook.o
CFLAGS_phonebook.o := -O0
else

KDIR ?= $(LINUX_BUILD)
BUILD_DIR ?= $(PWD)/build
BUILD_DIR_MAKEFILE ?= $(PWD)/build/Makefile

default: $(BUILD_DIR_MAKEFILE)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) modules

modules_install: $(BUILD_DIR_MAKEFILE)
	make -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) modules_install INSTALL_MOD_PATH=$(BUILDS)/initramfs

$(BUILD_DIR):
	mkdir -p "$@"

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	touch "$@"

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) clean

endif
