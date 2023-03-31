#include "Process_Control.h"

#include <signal.h>

Process_Control process_control;

void signal_handler(int signo){
    if(signo == SIGINT){
        process_control.should_exit = true;
    }
}

void init_process_control(){
    process_control.should_exit = false;

    signal(SIGINT, signal_handler);
}