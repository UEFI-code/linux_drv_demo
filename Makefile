# Multi-file module
obj-m += mydrv.o
mydrv-objs := main.o fs.o

# Get the kernel version and build directory
KVERSION = $(shell uname -r)
KDIR = /lib/modules/$(KVERSION)/build

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean target
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# Install target (load the module)
install:
	sudo insmod mydrv.ko

# Uninstall target (unload the module)
uninstall:
	sudo rmmod mydrv

# Show kernel messages
dmesg:
	dmesg | tail -20

# Test the driver
test:
	@echo "Testing the driver..."
	@if [ -c /dev/mydrv ]; then \
		echo "Device file exists. Reading from /dev/mydrv:"; \
		cat /dev/mydrv; \
	else \
		echo "Device file /dev/mydrv does not exist!"; \
	fi

.PHONY: all clean install uninstall dmesg test
