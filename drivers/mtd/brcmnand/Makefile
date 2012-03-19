#
# Makefile for the Broadcom NAND MTD
#

obj-$(CONFIG_MTD_BRCMNAND)	+= brcmnand.o

brcmnand-objs-$(CONFIG_MTD_BRCMNAND_EDU) += edu.o brcmnand_isr.o
brcmnand-objs-$(CONFIG_MTD_BRCMNAND_CORRECTABLE_ERR_HANDLING) += brcmnand_cet.o

brcmnand-objs			+= bcm7xxx-nand.o brcmnand_base.o \
				   brcmnand_bbt.o $(brcmnand-objs-y)

#EXTRA_CFLAGS			+= -Werror
