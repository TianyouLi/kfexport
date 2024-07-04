obj-m := kfexport.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
WARN_FLAGS += -Wall

.PHONY: default
defualt:
	$(MAKE) -C $(KDIR) M=$(PWD) 
clean:
	rm -rf *.ko *.o modules.* *.mod* *.symvers

