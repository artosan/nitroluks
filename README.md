# Nitroluks

Unlock LUKS disk by Nitrokey on boot using the password vault which is built in Nitrokey Pro and Nitrokey Storage

## Warning

This is an early version, and I have only personally tested it by running it on my computer.

__YOU CAN MAKE YOUR DEBIAN UNBOOTABLE IF YOU MESS THE crypttab and initramfs SO BE CAREFUL__

Please take backups of any important files and make sure that you have backup initramfs which you can use as a fallback if the installation messes your boot up for some reason.

## Installation

Before the installation, you must do some preparation steps.

### Preparation
* First, add a new password into your nitrokey, the easiest way is to use `nitro-app`. Go to password safe and add a new slot, name the slot as `LUKS` and generate the password for the slot and save it.

* Next, add the same password to your luks disk, i.e. `# cryptsetup luksAddKey /dev/<device> ` and make sure you use the same key as you stored into your nitrokeys LUKS slot.

__Note that IT IS IMPORTANT THAT THE SLOT NAME IN THE NITROKEY IS__ `LUKS` __OTHERWISE YOU CAN NOT UNLOCK YOUR DISK BY NITROKEY!__


### Install
* Clone this repo and run `make debianize`.

* After this step, you will find `nitroluks_0.1-1_arch.deb` package under `DEBUILD` directory.

* Install it by running `dpkg -i nitroluks_0.1-1_arch.deb`

* This package has dependency to `libhidapi-libusb0` which can be installed by running `apt install libhidapi-libusb0`

* After these steps, edit your `/etc/crypttab` file and add `keyscript=/usr/bin/keyscript.sh` to your luks entry. 

* See nitroluks_crypttab as an example how the crypttab file should look.

* Run `update-initramfs -u`

## Additional information

* If the nitrokey is not connected during the boot, the PIN is locked, or something else blocks you for using the nitrokey, you will get the default luks password prompt which you can use to unlock the disk.

* If you have multiple slots named as `LUKS` in your nitrokey, the first one will be used.

* This repo also contains a copy of `libnitrokey` which can be found [here.](https://github.com/Nitrokey/libnitrokey)