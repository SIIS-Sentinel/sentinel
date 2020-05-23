obj-m += mod_sentinel.o
mod_sentinel-objs += sentinel_helper.o sentinel_sysfs.o sentinel.o

ifeq ($(KVER),)
KVER := $(shell uname -r)
endif
MODDESTDIR := /lib/modules/${KVER}/extramodules

all:
	make -C /lib/modules/${KVER}/build M=$(shell pwd) modules

install: all
	@mkdir -p $(MODDESTDIR)
	@install -p -m 644 mod_sentinel.ko $(MODDESTDIR)/sentinel.ko
	@depmod -a ${KVER}

clean:
	make -C /lib/modules/${KVER}/build M=$(shell pwd) clean
