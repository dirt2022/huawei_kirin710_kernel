ccflags-y += -Idrivers/power/hisi/coul/
obj-$(CONFIG_HISI_COUL_HI6421V500)	+= hi6421v500/
obj-$(CONFIG_HISI_COUL_HI6555V100)	+= hi6555v100/
obj-$(CONFIG_HISI_COUL_HI6555V200)      += hi6xxx/
obj-$(CONFIG_HISI_COUL_HI6555V300)      += hi6xxx/
obj-$(CONFIG_HISI_COUL_HI6421V600)      += hi6xxx/
obj-$(CONFIG_HISI_COUL_HI6421V700)      += hi6xxx/
obj-$(CONFIG_HISI_COUL_HI6421V800)      += hi6xxx/
EXTRA_CFLAGS += -Idrivers/hisi/tzdriver/libhwsecurec/
ifeq ($(wildcard $(srctree)/drivers/huawei_platform/inputhub/$(TARGET_BOARD_PLATFORM)),)
ccflags-y  += -I$(srctree)/drivers/huawei_platform/inputhub/default/
else
ccflags-y  += -I$(srctree)/drivers/huawei_platform/inputhub/$(TARGET_BOARD_PLATFORM)/
endif

ifneq ($(TARGET_BUILD_VARIANT),user)
ccflags-y += -DISC_TEST
else
ifeq ($(TARGET_VERSION_MODE),factory)
ccflags-y += -DISC_TEST
endif
endif

obj-$(CONFIG_HISI_COUL)             += hisi_coul_core.o hisi_coul_drv.o
obj-$(CONFIG_HISI_COUL)             += hisi_coul_event.o hisi_coul_imonitor.o
obj-$(CONFIG_HISI_COUL_POLAR)       += hisi_coul_polar.o
obj-$(CONFIG_HISI_COUL_TEST)        += hisi_coul_drv_test.o

