ifneq ($(KERNELRELEASE),)
	obj-m := log.o
else
	KERNELDIR ?= /home/sw/Workspace/packages/linux-4.10

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	@rm -rf *.o *.mod.c *.mod.o *.ko *.order *.symvers .*.cmd .tmp_versions
endif
