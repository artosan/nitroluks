#!/bin/sh
echo "*** Trying to unlock the disk $CRYPTTAB_SOURCE ($CRYPTTAB_NAME) by nitrokey, waiting for the USB to be recognized..." >&2

# set exit status to fail so we fall back to default luks prompt if execution of nitro_luks does not work for some reason
exit_status=1

# disable echo
stty -echo

# sleep to wait for the usb device to be available
# TODO is there better way to do this?
sleep 5

# call nitroluks to get the LUKS key
/bin/nitro_luks

exit_status=$?

# in case nitroluks fails, fall back to the default LUKS password prompt
if [ $exit_status -eq 1 ]; then
    /lib/cryptsetup/askpass "Unlocking the disk $CRYPTTAB_SOURCE ($CRYPTTAB_NAME)\nEnter passphrase: "
else
    echo "*** LUKS setup successful by nitrokey" >&2
fi