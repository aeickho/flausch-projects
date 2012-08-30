#include "platform_config.h"

#include <peripheral/int.h>
#include <xc.h>

//__attribute__((section("xmem"))) 
void run()
{
  while(1) {
    mLED_1_Toggle ();
    }
}
