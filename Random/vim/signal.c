#include <signal.h>

#include "signal.h"
#include "term.h"

void sighandler_sigcont(int s)
{
  if (s == SIGCONT)
  {
    setup_term();
  }
}
