# Nitroluks

Unlock LUKS disk by Nitrokey on boot using the password vault which is built in Nitrokey Pro and Nitrokey Storage.

Support for Nitrokey Start is coming shortly. It works by adding support for keyfiles, i.e., the LUKS password would be stored in an encrypted file within the initramfs which is decrypted during the boot by Nitrokey. 

This method will of course work with Nitrokey Pro and Nitrokey Storage also.

## Warning

This is an early version, and I have only tested it personally by running it on my computer.

__YOU CAN MAKE YOUR DEBIAN UNBOOTABLE IF YOU MESS UP THE crypttab or initramfs SO BE CAREFUL. You have been warned.__

Please take backups of any important files and make sure that you have backup initramfs which you can use as a fallback if the installation messes your boot up for some reason.

This package could potentially work with Ubuntu also, but I have not tested it. 

Moreover, the directory structure is not compliant with Debian project guidelines. I will be improving the package structure, code quality and make sure that this also works with Ubuntu shortly so stay tuned.

## Installation

Before the installation, you must do some preparation steps.

### Preparation
* First, add a new password to your Nitrokey, the easiest way is to use `nitro-app`. Go to password safe and add a new slot, name the slot as `LUKS` and generate the password for the slot and save it.

* Next, add the same password to your LUKS disk, i.e. `# cryptsetup luksAddKey /dev/<device> ` and make sure you use the same key as you stored into your Nitrokeys LUKS slot.

__Note that IT IS IMPORTANT THAT THE SLOT NAME IN THE NITROKEY IS__ `LUKS` __OTHERWISE YOU CAN NOT UNLOCK YOUR DISK BY NITROKEY!__


### Install
* Clone this repo and run `make debianize`.

* After this step, you will find `nitroluks_0.1-1_arch.deb` the package under `DEBUILD` directory.

* Install it by running `dpkg -i nitroluks_0.1-1_arch.deb`

* This package has dependency to `libhidapi-libusb0` which can be installed by running `apt install libhidapi-libusb0`

* After these steps, edit your `/etc/crypttab` file and add `keyscript=/usr/bin/keyscript.sh` to your luks entry. Please consult the [crypttab manual](https://linux.die.net/man/5/crypttab) for more information.

* See nitroluks_crypttab as an example how the crypttab file should look.

* Run `update-initramfs -u`

## Additional information

* If the Nitrokey is not connected during the boot, the PIN is locked, or something else blocks you for using the Nitrokey, you will get the default LUKS password prompt which you can use to unlock the disk.

* If you have multiple slots named as `LUKS` in your Nitrokey, the first one will be used.

* This repo also contains a copy of `libnitrokey` which can be found [here.](https://github.com/Nitrokey/libnitrokey)