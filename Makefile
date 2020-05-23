obj-m += mod_sentinel.o
mod_sentinel-objs += sentinel_helper.o sentinel_sysfs.o sentinel.o

MODDESTDIR := /lib/modules/$(shell uname -r)/extramodules

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

install: all
	@mkdir -p $(MODDESTDIR)
	@install -p -m 644 mod_sentinel.ko $(MODDESTDIR)/sentinel.ko
	@depmod -a $(shell uname -r)

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
