#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <termios.h>
#include <unistd.h>
#include "include/NK_C_API.h"


#define STATUS_OK 0
#define WRONG_PASSWORD 4

#define SLOT_COUNT 16
#define ERROR 1
#define MAX_PIN_LENGTH 20

struct termios saved_attributes;

// Make the function pointer volatile to ensure that the compiler
// actually calls it and doesn't optimize it.
typedef void *(*memset_t)(void *, int, size_t);
static volatile memset_t memset_func = memset;

static int error(char const *msg)
{
    fprintf(stderr, "%s \n*** Falling back to default LUKS password entry.\n", msg);
    return ERROR;
}

static void disable_echo(void)
{
  struct termios tattr;
  tcgetattr(STDIN_FILENO, &saved_attributes);

  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

static void reset_input_mode (void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

int main(int argc, char const *argv[])
{
    const char* LUKS_password;
    char password[MAX_PIN_LENGTH + 1];
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
        return error("*** No nitrokey detected.\n");
    } 
    else 
    {
        const char* serial = NK_device_serial_number();
        fprintf(stderr, "*** Nitrokey : %s found!\n", serial);
    }
    
    do
    {
        do
        {
            // Stop if user pin is locked
            retry_count = NK_get_user_retry_count();
            if (retry_count == 0)
                return error("*** User PIN locked.");

            fprintf(stderr, "*** %d PIN retries left. Type N to quit or Y to continue and press enter.\n", retry_count);
            fgets(ask, 2, stdin);
            ask[strcspn(ask, "\n")] = 0;
            if (strcmp(ask, "N") == 0)
                return error("*** Exiting...");
            else
                // clear the input buffer
                while (getchar() != '\n');

        }while ((strcmp(ask, "Y") != 0));

        // Ask the password and unlock the nitrokey
        fprintf(stderr, "Enter the (user) PIN:\n");
        disable_echo();
        fgets(password, sizeof(password), stdin);
        reset_input_mode();
        // remove the trailing newline
        password[strcspn(password, "\n")] = 0;
        auth_status = NK_user_authenticate(password, password);

        // handle the login results
        if(auth_status == WRONG_PASSWORD) {
            memset_func(password, 0, sizeof(password));
            fprintf(stderr, "*** Wrong PIN!\n");
        }else if (auth_status == STATUS_OK) {
            fprintf(stderr, "*** PIN entry successful.\n");
        }else {
            memset_func(password, 0, sizeof(password));
            return error("*** Error in PIN entry.\n");
        }
    } while(auth_status == WRONG_PASSWORD);

    //  Find a slot from the nitrokey where we fetch the LUKS key from.
    password_safe_status = NK_enable_password_safe(password);
    memset_func(password, 0, sizeof(password));
    if (password_safe_status != STATUS_OK)
        return error("*** Error while accessing password safe.\n");

    fprintf(stderr, "*** Scanning the nitrokey slots...\n");
    slots = NK_get_password_safe_slot_status();
    for (uint8_t slot = 0; slot < SLOT_COUNT; ++slot) 
    {
        if (slots[slot] == 1)
        {
            const char* slotname = NK_get_password_safe_slot_name(slot);
            if(strcmp(slotname, "LUKS") == 0)
                slot_number = slot;
        } 
        else 
        {
            empty_slots++;
        }
    }

    if (empty_slots == SLOT_COUNT)
    {
        return error("*** No slots enabled.\n");
    } 
    else if(slot_number == 255) 
    {
        return error("*** No slot configured by name LUKS.\n");
    }

    // At this point we found a valid slot, go ahead and fetch the password.
    LUKS_password = NK_get_password_safe_slot_password(slot_number);
    
    // print password to stdout
    fprintf(stdout, "%s", LUKS_password);

    return 0;
}
