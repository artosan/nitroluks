#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include "include/NK_C_API.h"

#define STATUS_OK 0
#define WRONG_PASSWORD 4
#define SLOT_COUNT 16
#define ERROR 1

int error(char const *msg)
{
	fprintf(stderr, "%s \n*** Falling back to default LUKS password entry.\n", msg);
	return ERROR;
}

int main(int argc, char const *argv[])
{
	
	const char* LUKS_password;
	char password[26];
	uint8_t* slots;
	int login_status;
	int auth_status;
	int password_safe_status;
	int empty_slots = 0;
	uint8_t slot_number = 255;
	uint8_t retry_count;
	char ask[2];

	// Disable debug messages
    NK_set_debug(false);
	login_status  = NK_login_auto();
    
    if (login_status != 1) 
    {
    	return error("*** No devices detected.\n");

    } else 
    {
    	const char* serial = NK_device_serial_number();
    	fprintf(stderr, "*** Logged successfully into devide: %s.\n", serial);
    }
    
    do
    {
        do
        {
        	// Stop if user pin is locked
        	retry_count = NK_get_user_retry_count();
        	if (retry_count == 0)
    			return error("*** User PIN locked.");

    		fprintf(stderr, "*** %d PIN retries left. Enter N to quit or Y to continue.\n", retry_count);
    		fgets(ask, 2, stdin);
    		if (strcmp(ask, "N") == 0)
    			return error("*** Stopping nitrokey authentication.");
    		else
    			// clear the input buffer
    			while ( getchar() != '\n' );

        }while ((strcmp(ask, "Y") != 0));

        // Ask the password and unlock the nitrokey
    	fprintf(stderr, "Enter the device PIN:\n");
        fgets(password, 26, stdin);
        // remove the trailing newline
        password[strcspn(password, "\n")] = 0;
    	auth_status = NK_user_authenticate(password, password);

    	// handle the login results
		if(auth_status == WRONG_PASSWORD) {
			fprintf(stderr, "*** Wrong PIN!\n");
		}else if (auth_status == STATUS_OK) {
			fprintf(stderr, "*** Device PIN entry successful.\n");
		}else {
			return error("*** Error while authenticating user.\n");
		}
 
    } while(auth_status == WRONG_PASSWORD);

    /*
      Find a slot from the nitrokey where we fetch the LUKS key from.
    */
	password_safe_status = NK_enable_password_safe(password);
	if (password_safe_status != STATUS_OK)
		return error("*** Error while accessing password safe.\n");

	fprintf(stderr, "*** Scanning device slots...\n");
	slots = NK_get_password_safe_slot_status();
	for (uint8_t slot = 0; slot < SLOT_COUNT; ++slot) 
	{

		if (slots[slot] == 1)
		{
			const char* slotname = NK_get_password_safe_slot_name(slot);
			if(strcmp(slotname, "LUKS") == 0)
				slot_number = slot;

		} else {
			empty_slots++;
		}
	}

	if (empty_slots == SLOT_COUNT)
	{
		
		return error("*** No slots enabled, exiting.\n");

	} else if(slot_number == 255) {

		return error("*** No slot configured by name LUKS, exiting.\n");
	
	}

    /*
      At this point we found a valid slot, go ahead and fetch the password.
    */

    LUKS_password = NK_get_password_safe_slot_password(slot_number);
    // print password to stdout
    fprintf(stdout, "%s", LUKS_password);

	return 0;
}
