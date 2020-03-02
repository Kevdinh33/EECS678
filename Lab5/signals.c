#include <stdio.h>     /* standard I/O functions                         */
#include <stdlib.h>    /* exit                                           */
#include <stdbool.h>   /* using bool data type			                     */
#include <unistd.h>    /* standard unix functions, like getpid()         */
#include <signal.h>    /* signal name macros, and the signal() prototype */
#include <sys/types.h> /*needed for pid_t                                */

/* first, define the Ctrl-C-Ctrl-Z counter, initialize it with zero. */
int ctrl_c_ctrl_z_count = 0;

/* boolean flags to keep track of the ^C and ^Z combinations */
bool ctrl_c_pressed = false;
bool ctrl_z_pressed = false;

/* flag to keep track of user response after exit prompt */
int got_response = 0;

#define CTRL_C_CTRL_Z_THRESHOLD  5

pid_t my_pid;

/* Implement alarm handler - (You should terminate the program with "kill" system call and SIGQUIT signal if the user has not responded */
//Your code
void alarm_handler(int sig_num){
  printf("\n\nUser taking too long to respond. Exiting...\n\n");
  fflush(stdout);
  kill(my_pid, SIGINT);
  exit(0);
}

void check_count(){
  /* Check if threshold was reached */
  if (ctrl_c_ctrl_z_count >= CTRL_C_CTRL_Z_THRESHOLD) {
    char answer[30];

    /* Prompt the user to tell us if to really exit or not - if user hasn't responded in 5 seconds, "kill" the program */
    //Your code
    alarm(5);
    printf("\nReally exit? [Y/n]: ");
    fflush(stdout);
    fgets(answer, sizeof(answer), stdin);
    if (answer[0] == 'n' || answer[0] == 'N') {
      printf("\nContinuing\n");
      fflush(stdout);

      /* Keep track of the fact that the user has responded */
      //Your code
      alarm(0);
      /* Reset the ctrl_c_ctrl_z_count counter */
      //Your code
      ctrl_c_ctrl_z_count= 0;
    }
    else {
      printf("\nExiting...\n");
      fflush(stdout);
      exit(0);
    }
  }
}

/* the Ctrl-C signal handler */
void catch_int(int sig_num)
{
  /* Check for the following combinations:
      a. ^C ^C (reset counter)
      b. ^Z ^C (increment counter)
      c. ^C
     and write your code for each of these combinations */
  //Your code
  if(ctrl_c_pressed == false && ctrl_z_pressed == false)
  {
    ctrl_c_pressed = true;
  }
  else if(ctrl_z_pressed == true && ctrl_c_pressed == false)
  {
    ctrl_c_ctrl_z_count++;
    printf(" - The current count: '%d'\n", ctrl_c_ctrl_z_count);
    ctrl_z_pressed = false;
    ctrl_c_pressed = false;
  }
  else
  {
    printf("\nResetting the counter\n");
    ctrl_c_ctrl_z_count = 0;
    ctrl_c_pressed = false;
    ctrl_z_pressed = false;
  }

  check_count();
}

/* the Ctrl-Z signal handler */
void catch_tstp(int sig_num)
{
  /* Check for the following combinations:
      a. ^Z ^Z (reset counter)
      b. ^C ^Z (increment counter)
      c. ^Z
     and write your code for each of these combinations */
  //Your code
  if(ctrl_c_pressed == false && ctrl_z_pressed == false)
  {
    ctrl_z_pressed = true;
  }
  else if(ctrl_c_pressed)
  {
    ctrl_c_pressed = false;
    ctrl_z_pressed = false;
    ctrl_c_ctrl_z_count++;
    printf(" - The current count: '%d'\n", ctrl_c_ctrl_z_count);
  }
  else
  {
    printf("\nResetting the counter\n");
    ctrl_c_ctrl_z_count= 0;
    ctrl_c_pressed = false;
    ctrl_z_pressed = false;
  }

  check_count();
}

int main(int argc, char* argv[])
{
  my_pid = getpid();
  printf("The PID: %d\n", my_pid);
  printf("\nYou may start.\n");

  struct sigaction sa;
  sigset_t mask_set;  /* used to set a signal masking set. */

  /* setup mask_set */
  //Your code
  sigfillset(&mask_set);
  sa.sa_mask = mask_set;
  sa.sa_flags = 0;
  while(1){
  /* set signal handlers */
  //Your code
  sigdelset(&mask_set, SIGALRM);
  sa.sa_mask = mask_set;

  sa.sa_handler = catch_tstp;
  sigaction(SIGTSTP, &sa, NULL);

  sa.sa_handler = catch_int;
  sigaction(SIGINT, &sa, NULL);

  sa.sa_handler = alarm_handler;
  sigaction(SIGALRM, &sa, NULL);
  }

  return 0;
}
