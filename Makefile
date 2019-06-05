obj-m += hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/adrien/dev/hello modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/adrien/dev/hello clean