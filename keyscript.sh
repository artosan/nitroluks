#!/bin/sh

echo "*** Trying to unlock the disk $CRYPTTAB_SOURCE ($CRYPTTAB_NAME) by nitrokey, waiting for the USB to be recognized..." >&2

# set exit status to fail so we fall back to default luks prompt if execution of nitro_luks does not work for some reason
exit_status=1

# sleep to wait for the usb device to be available
# TODO is there better way to do this?
sleep 5

KEYFILE=/etc/gpg_luks/luks.key.gpg
GPG_HOME_DIR=/etc/gpg_luks/.gnupg

if [ -f $KEYFILE ]; then
	# Decrypt by using the keyfile if it exists
	passphrase="$(gpg --homedir=$GPG_HOME_DIR --quiet --decrypt $KEYFILE)"
	exit_status=$?

	if [ $exit_status -eq 0 ]; then
		echo $passphrase | tr -d '\n' 
	fi
	
else
	# call nitroluks to get the LUKS key if no keyfile is present
	/bin/nitro_luks
	exit_status=$?
fi


# in case nitroluks fails, fall back to the default LUKS password prompt
if [ $exit_status -ne 0 ]; then
    /lib/cryptsetup/askpass "Unlocking the disk $CRYPTTAB_SOURCE ($CRYPTTAB_NAME)\nEnter passphrase: "
else
    echo "*** LUKS setup successful by nitrokey" >&2
fi