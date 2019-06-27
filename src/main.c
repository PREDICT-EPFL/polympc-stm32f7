#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <stdio.h>
#include "nmpc_test.h"

BaseSequentialStream * const uart = (BaseSequentialStream *) &SD3;

static THD_WORKING_AREA(blinker, 256);
static THD_FUNCTION(blinker_main, arg) {
    (void)arg;
    chRegSetThreadName("blinker");
    while (true) {
        palSetLine(LINE_LED1);
        chThdSleepMilliseconds(50);
        palSetLine(LINE_LED2);
        chThdSleepMilliseconds(50);
        palSetLine(LINE_LED3);
        chThdSleepMilliseconds(200);
        palClearLine(LINE_LED1);
        chThdSleepMilliseconds(50);
        palClearLine(LINE_LED2);
        chThdSleepMilliseconds(50);
        palClearLine(LINE_LED3);
        chThdSleepMilliseconds(200);
    }
}

void _unhandled_exception(void)
{
    chSysHalt(__FUNCTION__);
}

void __late_init(void)
{
    halInit();
    chSysInit();

    sdStart(&SD3, NULL);
    chprintf(uart, "boot\n");
}

void panic(const char* msg)
{
    chSysHalt(msg);
}

void my_assert(const char *f, int l, bool x)
{
    if (!x) {
        printf("%s: %d\n", f, l);
        chThdSleepMilliseconds(100);
        panic(__FUNCTION__);
    }
}

int main(void)
{
    chThdCreateStatic(blinker, sizeof(blinker), NORMALPRIO + 1, blinker_main, NULL);

    printf("start tests\n");
    while (true) {
        nmpc_test();
        chThdSleepMilliseconds(500);
    }
}
