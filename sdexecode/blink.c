#include "platform_config.h"

#include <peripheral/int.h>
#include <xc.h>

void
run ()
{
  char buf[100];
  int i;
  for (i = 0; i < 100; i++)
      buf[i]=i;
  for (i = 0; i < 1000; i++)
    {
      mLED_1_Toggle ();
    }
  while (U_STAbits.UTXBF);      // wait when buffer is full
    U_TXREG = 'R';
  while (U_STAbits.UTXBF);      // wait when buffer is full
    U_TXREG = buf[10];
     
}
