#include "platform_config.h"
#include "uart.h"
#include "spi.h"
#include "diskio.h"
#include "systick.h"

#include <peripheral/int.h>

/* low priority interrupt as a life indicator */
void
  __attribute__ ((nomips16, interrupt (ipl1),
		  vector (_TIMER_2_VECTOR))) timer2_interrupt (void)
{
  static int step = 400;
  static int duty = 20000;

  mLED_2_Toggle ();
  if (duty)
    {
      if (duty == 20000 || duty == 400)
	step = -step;
      PR2 = duty + step;
      duty = 0;
    }
  else
    {
      duty = PR2;
      PR2 = 64000 - duty;
    }

  IFS0bits.T2IF = 0;		/* reset interrupt flag */
}

static enum
{
  EXCEP_IRQ = 0,		// interrupt
  EXCEP_AdEL = 4,		// address error exception (load or ifetch)
  EXCEP_AdES,			// address error exception (store)
  EXCEP_IBE,			// bus error (ifetch)
  EXCEP_DBE,			// bus error (load/store)
  EXCEP_Sys,			// syscall
  EXCEP_Bp,			// breakpoint
  EXCEP_RI,			// reserved instruction
  EXCEP_CpU,			// coprocessor unusable
  EXCEP_Overflow,		// arithmetic overflow
  EXCEP_Trap,			// trap (possible divide by zero)
  EXCEP_IS1 = 16,		// implementation specfic 1
  EXCEP_CEU,			// CorExtend Unuseable
  EXCEP_C2E			// coprocessor 2
} _excep_code;

static unsigned int _epc_code;
static unsigned int _excep_addr;

  // this function overrides the normal _weak_ generic handler
void
_general_exception_handler (void)
{
  register unsigned int i;

  asm volatile ("mfc0 %0,$13":"=r" (_excep_code));
  asm volatile ("mfc0 %0,$14":"=r" (_excep_addr));

  _excep_code = (_excep_code & 0x0000007C) >> 2;

  unsigned int val = _excep_code;

  while (U_STAbits.UTXBF);	// wait when buffer is full
  U_TXREG = 'E';
  while (U_STAbits.UTXBF);	// wait when buffer is full
  U_TXREG = ':';
  for (i = 0; i < 8; i++)
    {
      int bla = ((val & 0xF0000000) >> 28);
      while (U_STAbits.UTXBF);

      if (bla > 9)
	U_TXREG = (('A' - 10) + bla);
      else
	U_TXREG = ('0' + bla);
      val <<= 4;
    }

  while (U_STAbits.UTXBF);	// wait when buffer is full
  U_TXREG = ' ';

  val = _excep_addr;
  for (i = 0; i < 8; i++)
    {
      int bla = ((val & 0xF0000000) >> 28);
      while (U_STAbits.UTXBF);

      if (bla > 9)
	U_TXREG = (('A' - 10) + bla);
      else
	U_TXREG = ('0' + bla);
      val <<= 4;
    }

  while (U_STAbits.UTXBF);	// wait when buffer is full
  U_TXREG = '\r';
  while (U_STAbits.UTXBF);	// wait when buffer is full
  U_TXREG = '\n';

  while (1)
    {
      ;
    }
}

int
main (void)
{
  PF_BYTE *buff = 0xA0001000;
  mInitAllLEDs ();

  /* set up timer 2 for life indicator */
  PR2 = 20000;
  T2CON = 0x8030;		// enable, prescaler=1/8
  IPC2bits.T2IP = 1;
  INTEnableSystemMultiVectoredInt ();
  IEC0bits.T2IE = 1;



  systick_init ();

  uart_init_highspeed ();

  spi_sd_init ();

  int counter = 0;
//while(1) { mLED_1_Toggle(); }
  while (1)
    {
      unsigned int i;
      int buf;

      // void (* buff)(void)  = 0xA0001000;
      uart_puts ("Run ");
      uart_puthex (counter++);
      uart_puts ("\r\n");

      delay100usec (30000);


      mLED_1_Toggle ();
      unsigned int stat = disk_initialize ();

      uart_puts ("disk_initialize: ");
      uart_puthex (stat);
      uart_puts ("\r\n");

      disk_ioctl (0, GET_SECTOR_COUNT, &buf);
      uart_puts ("GET_SECTOR_COUNT: ");
      uart_puthex (buf);
      uart_puts ("\r\n");



      BMXDKPBA = 0x1000;
      BMXDUDBA = 0x2000;
      BMXDUPBA = 0x2000;

      unsigned int ret = disk_read (0,	/* Physical drive nmuber (0) */
				    buff,	/* Pointer to the data buffer to store read data */
				    70000,	/* Start sector number (LBA) */
				    1	/* Sector count (1..255) */
	);
      uart_puts ("xxxxxxxx");
      uart_puts ("\r\n");

      ret = disk_write (0,	/* Physical drive nmuber (0) */
			buff,	/* Pointer to the data buffer to stor */
                        80000,  /* Start sector number (LBA) */
			1	/* Sector count (1..255) */
	);
      uart_puthex (ret);
      uart_puts ("xxxxxxxx");
      uart_puts ("\r\n");


      for (i = 0; i < 10; i++)
	{
	  uart_puthex (buff[i]);
	  uart_puts (" ");
	  delay100usec (300);
	}

      uart_puts ("yyyyy");
      uart_puts ("\r\n");



      delay100usec (10000);




      mLED_1_Off ();
      delay100usec (10000);
      mLED_1_On ();
      delay100usec (10000);
      mLED_1_Off ();
      delay100usec (10000);

      ((void (*)()) buff) ();
      _nop ();
      _nop ();
      _nop ();
      _nop ();
      _nop ();
      _nop ();
      _nop ();
      mLED_1_Off ();
      delay100usec (10000);
      mLED_1_On ();
      delay100usec (10000);
      mLED_1_Off ();
      delay100usec (10000);


      _nop ();
      _nop ();
      _nop ();
      _nop ();
      _nop ();


    }
}
