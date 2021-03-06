#ifdef SIMULATOR

// Compiler appeasement
#undef disable_transmit
#undef enable_transmit
#define disable_transmit()
#define enable_transmit()
#undef USB_SERIAL

#undef BSS
#ifdef __MACH__  // Mac OS X
  #define BSS __attribute__ ((__section__ ("__DATA,.bss")))
#else
  #define BSS __attribute__ ((__section__ (".bss")))
#endif

#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "simulator/data_recorder.h"

#define ACD         7
#define OCIE1A      1

// TODO: Implement simulated EEMEM persistence
#define EEMEM
#define eeprom_read_dword(ptr32) (*(ptr32))
#define eeprom_read_word(ptr16) (*(ptr16))
#define eeprom_write_dword(ptr32, i32) (*(ptr32)=i32)
#define eeprom_write_word(ptr16, i16) (*(ptr16)=i16)


/**
  The following enum gives numeric values to each of the pin names which would
  normally be defined in the chip-specific include file.  All of the pins
  Teacup uses are listed here even though not all of them may be defined in the
  config.

  Let's consider the X_MIN_PIN as an example.  When the following code says

    enum { ... X_MIN_PIN, ...}

  it means one of these two different things:

  1. X_MIN_PIN is defined in the board config file. For example,

       #define    X_MIN_PIN     DIO7

     In this case the C pre-processor will replace X_MIN_PIN below with DIO7,
     and DIO7 will be enumerated (given a numeric value) as 2.

  2. X_MIN_PIN is not defined in the board config file, which Teacup interprets
     as the printer not having an X-min endstop. In this case X_MIN_PIN will
     be enumerated (given a numeric value) as 2.

  Importantly the C preprocessor interpret "defined(X_MIN_PIN)" the same even
  with this enum in place. So in the first case, defined(X_MIN_PIN) is true;
  in the second case, it is not (even though we have enumerated it a value).

  The simulator uses this trick to provide stand-in values for all the pins the
  config does or does not define, but the conditionally compiled code will
  react consistently between the simulated or the real controller compilation
  targets.

  Additionally, this trick ensures each pin has a known value when it is used.
  For example, X_MIN_PIN will always be '2' in the simulator when it is used.
*/
typedef enum {
  // Define pins used
  X_STEP_PIN,
  X_DIR_PIN,
  X_MIN_PIN,
  X_ENABLE_PIN,
  Y_STEP_PIN,
  Y_DIR_PIN,
  Y_MIN_PIN,
  Y_ENABLE_PIN,
  Z_STEP_PIN,
  Z_DIR_PIN,
  Z_MIN_PIN,
  Z_ENABLE_PIN,
  E_STEP_PIN,
  E_DIR_PIN,
  E_ENABLE_PIN,

  STEPPER_ENABLE_PIN,

  X_MAX_PIN,
  Y_MAX_PIN,
  Z_MAX_PIN,
  PS_ON_PIN,
  PS_MOSFET_PIN,

  SCK,
  MOSI,
  MISO,
  SS,

  RX_ENABLE_PIN,
  TX_ENABLE_PIN,
  DEBUG_LED_PIN,

  PIN_NB  /* End of PINS marker; Put all new pins before this one */
} pin_t;

// AVR stand-ins
typedef enum {
  WGM00 = 0,
  WGM01,
  WGM20,
  WGM21,
  CS00 = 0,
  CS02,
  CS20,
  CS21,
  CS22,
} masks_t;

#undef TEMP_PIN_CHANNEL
#define TEMP_PIN_CHANNEL 0

extern uint8_t ACSR;
extern uint8_t TIMSK1;
extern volatile bool sim_interrupts;

// Simulate AVR interrupts.
#define ISR(fn) void fn (void)
void TIMER1_COMPA_vect(void);
void TIMER1_COMPB_vect(void);

// Compare-timers for next interrupts.
extern uint16_t OCR1A, OCR1B;

// Interrupt control registers.
extern uint16_t TCCR1A, TCCR1B;
enum { CS10 = 1 , OCIE1B = 3 };

#define TCNT1 (sim_tick_counter())
void cli(void);
void sei(void);

#ifdef USE_WATCHDOG
#define wd_init()
#define wd_reset()
#endif

void sim_start(int argc, char ** argv);
void sim_info(const char fmt[], ...);
void sim_debug(const char fmt[], ...);
void sim_error(const char msg[]);
void sim_assert(bool cond, const char msg[]);
void sim_gcode_ch(char ch);
void sim_gcode(const char msg[]);
void sim_report_temptables(int sensor) ;

/**
 * Initialize simulator timer and set time scale.
 *
 * @param scale time slow-down factor; 0=warp-speed, 1=real-time, 2-half-time, etc.
 */
void sim_timer_init(uint8_t scale);

void sim_timer_stop(void);
void sim_timer_set(void);
uint16_t sim_tick_counter(void);
uint64_t sim_runtime_ns(void); ///< Simulated run-time in nanoseconds
void sim_time_warp(void); ///< skip ahead to next timer interrupt, when time_scale==0

#define DIO0_PIN "proof of life"

#endif /* _SIMULATOR_H */
#endif /* SIMULATOR */
