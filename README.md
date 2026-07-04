# What's this?
Custom Kernel for kirin710
Currently tested on JDN2-AL50

# related quirks of tested device
When using GSI, wifi connection is unstable.

# how to build an available kernel.img?
```shell
 apt install device-tree-compiler build-essential clang bc git -y
 git clone https://github.com/dirt2022/huawei_kirin710_kernel
 cd huawei_kirin710_kernel
 make merge_kirin710_defconfig
 make -j$(nproc)
```

Use magiskboot to unpack KERNEL.IMG from your device's OTA package (update.app)

 
 ```shell
 mkdir temp
 cd temp
 magiskboot unpack path_to_kernel.img
 cp arch/arm64/boot/Image.gz ./kernel
 magiskboot repack path_to_kernel.img output.img
```

# Pay attention
The kernel version is 4.9.148
```markdown
selinux is always Permissive,(if you need security, make menuconfig , and disable selinux_develop)

(if the option is enabled,selinux would be always permissive and unable to return enforcing mode)

no KernelSU included (you need to flash magisk patched recovery_ramdisk to get root access)
```
# how to flash it?
You need an firmware lower than 9.1.0.126
If you can find it but dc-phoneix says the file is not suitable for flash :
Please see this : https://wuyou.net/forum.php?mod=viewthread&tid=436686

```shell
 fastboot flash kernel output.img
```

# thanks
[https://github.com/](https://github.com/Abdelhay-Ali/android_kernel_huawei_kirin710_KernelSU)https://github.com/Abdelhay-Ali/android_kernel_huawei_kirin710_KernelSU
