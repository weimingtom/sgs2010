/**

 logic thread

*/

#include "config.h"




RESULT logic_begin(int test_mode);



RESULT logic_send_cmd(const char* cmd_str);

int  logic_check_input(char* prompt, int size);

char* logic_wait_cmd(const char* prompt, char* buf, int size);


YESNO logic_isrunning();

void logic_finish();

