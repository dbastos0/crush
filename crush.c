#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "TestU01.h"

void usage(void);
int is_valid_battery(char *optarg);
int min(int a, int b);
char *program_name; /* a pointer to argv[0] */

unsigned int generator(void)
{
  static unsigned int buffer[8192 / sizeof (unsigned int)];
  static unsigned int pos; /* where is our number? */
  static unsigned int limit; /* where does the data in the buffer end? */
  
  if (pos >= limit) {
    /* refill the buffer and continue by restarting at 0 */
    limit = fread(buffer, sizeof (unsigned int), (sizeof buffer) / sizeof(unsigned int), stdin);

    if (limit == 0) {
      // We read 0 bytes.  This either means we found EOF or we have 
      // an error.  A decent generator is infinite, so this should never
      // happen.
      if (ferror(stdin) != 0) {
        perror("fread"); exit(-1);
      }
      if (feof(stdin) != 0) {
        printf("generator produced eof\n");
        exit(0);
      }
    }
    
    pos = 0;

  }
  
  unsigned int random = buffer[pos]; /* get one */
  pos = pos + 1;
  return random;
}
int main(int argc, char **argv)
{
  char battery[8]; /* values small, medium or big */
  char name[1000]; /* the name of the generator given by user */
  int option; /* the option produced by getopt() */
  program_name = *argv;
  
  memset(battery, '\0', sizeof battery);
  memset(name, '\0', sizeof name);

    static struct option long_options[] = {
      {"battery", required_argument, 0,  'b' },
      {"name", required_argument, 0,  'n' },
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}
    };

    int digit_optind = 0;
    int option_index = 0;
    
    while ((option = getopt_long(argc, argv, "b:n:h", long_options, &option_index)) != -1)
      switch(option) {
        case 'b': /* b as in battery (of tests) */
        if (is_valid_battery(optarg) < 0) {
            fprintf(stdout, "Error: valid batteries are small, medium, big.\n");
            usage();
          }
          
          /* I assume optarg will never be non-null-terminated. */
          strncpy(battery, optarg, sizeof battery);
          break;

        case 'n': /* n as in name (of the generator) */
          strncpy(name, optarg, sizeof name);
          break;
          
        case 'h':
        default:
          usage();
      }
    argc -= optind;
    argv += optind;
      
    /* check if user has given all necessary things */
    if (is_valid_battery(battery) < 0) {
      fprintf(stdout, "Error: valid batteries are small, medium, big\n");
      usage();
    }
    
    if (strlen(name) == 0) {
      fprintf(stdout, "Error: you must give a name to your generator\n");
      usage();
    }


  unif01_Gen* g = unif01_CreateExternGenBits(name, generator);

  if (strncmp("small", battery, sizeof battery) == 0) {
    bbattery_SmallCrush(g);
  } 
  else 
  if (strncmp("medium", battery, sizeof battery) == 0) {
    bbattery_Crush(g);
  }
  else
  if (strncmp("big", battery, sizeof battery) == 0) {
    bbattery_BigCrush(g);
  }
  else {
    fprintf(stdout, "never reached\n");
  }
  
  unif01_DeleteExternGenBits(g);
  return 0;
}

int is_valid_battery(char *optarg) {
  if (strncmp("small", optarg, sizeof "small") == 0) return 0;
  if (strncmp("medium", optarg, sizeof "medium") == 0) return 0;
  if (strncmp("big", optarg, sizeof "big") == 0) return 0;
  return -1;
}

void usage(void) {
  fprintf(stdout, "Usage: %s [options]\n", program_name);
  fprintf(stdout, " Tests your data for randomness against TestU01.\n\n");
  fprintf(stdout, "Examples:\n");
  fprintf(stdout, "cat /dev/urandom | crush -b small -n 'the local generator'\n");
  fprintf(stdout, "xorshift32 | crush -b small -n xorshift32\n\n");
  fprintf(stdout, " The options are:\n"
  "-b, --battery    Your choice of battery (small, medium, big)\n"
  "-n, --name       The name of your generator\n"
  "-h, --help       Display this information\n");
  exit(0);
}


