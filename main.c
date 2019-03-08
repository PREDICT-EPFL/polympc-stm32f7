#include <ch.h>
#include <hal.h>

static THD_WORKING_AREA(blinker, 128);
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

int main(void)
{
    halInit();
    chSysInit();

    sdStart(&SD3, NULL);

    chThdCreateStatic(blinker, sizeof(blinker), NORMALPRIO + 1, blinker_main,
                      NULL);

    while (true) {
        if (palReadLine(LINE_BUTTON)) {

        }
        chThdSleepMilliseconds(500);
    }
}
