obj-m += sentinel.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/adrien/dev/sentinel modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/adrien/dev/sentinel clean