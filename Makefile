obj-m += mod_sentinel.o
mod_sentinel-objs += sentinel_helper.o sentinel_procfs.o sentinel.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/adrien/dev/sentinel modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/adrien/dev/sentinel clean
	