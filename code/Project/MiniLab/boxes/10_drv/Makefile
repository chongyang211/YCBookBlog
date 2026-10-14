obj-m += myecho.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install: all
	sudo insmod myecho.ko
	sudo chmod 666 /dev/myecho || true
	dmesg | tail -5

uninstall:
	sudo rmmod myecho || true
	dmesg | tail -3

test: install
	echo "hello myecho" > /dev/myecho
	cat /dev/myecho

.PHONY: all clean install uninstall test
