KERN_DIR ?= /lib/modules/$(shell uname -r)/build

obj-m := proctest1.o

all:
	make -C ${KERN_DIR}  M=$(PWD) modules

clean:
	rm *.o *.mod.c *.order *.ko *.symvers
