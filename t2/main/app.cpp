#include "delay.h"
#include "digital.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "serial.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define US_TRIG PIN16
#define US_ECHO PIN5

static inline uint32_t asm_ccount(void)
{
	uint32_t r;
	asm volatile("rsr %0, ccount" : "=r"(r));
	return r;
}

static inline void zera(void)
{
	uint32_t r;
	asm volatile("wsr    %0, ccount\n" : : "a"(0) : "memory");
	asm volatile("rsr %0, ccount" : "=r"(r));
}

uint64_t heigth_mesure()
{
	uint64_t time_start = 0;
	uint64_t time_end = 0;

	// Start signal
	digital.digitalWrite(US_TRIG, HIGH);
	delay_us(10);
	digital.digitalWrite(US_TRIG, LOW);

	// Wait for echo high
	while (1) {
		if (digital.digitalRead(US_ECHO) == HIGH) {
			time_start = esp_timer_get_time();
			break;
		}
	}

	// Wait for echo low
	while (1) {
		if (digital.digitalRead(US_ECHO) == LOW) {
			time_end = esp_timer_get_time();
			break;
		}
	}

	uint64_t time_delta = time_end - time_start;
	uint64_t distance = time_delta / 58;
	printf(
	    "time_start=%ld, time_end=%ld, time_delta=%ld, distance=%ld\n",
	    (long)time_start,
	    (long)time_end,
	    (long)time_delta,
	    (long)distance
	);
	return distance;
}

uint64_t heigth_config_baseline() { return heigth_mesure(); }

extern "C" void app_main();
void app_main()
{
	serial.begin(9600);
	printf("\n\nRunning...\n\n");

	digital.pinMode(US_TRIG, OUTPUT);
	digital.pinMode(US_ECHO, INPUT);
	/*
	    uint64_t time_start = 0;
	    uint64_t time_end = 0;
	    while (1) {
	        printf("begin");

	        printf("Dist: %llu", heigth_mesure());
	        time_start = esp_timer_get_time();

	        delay_ms(500);
	        time_end = esp_timer_get_time();
	        printf("Start: %ld, End: %ld, Diff: %ld\n", (long)time_start, (long)time_end, (long)(time_end -
	   time_start));
	    }
	*/

	uint64_t baseline = 0;

	while (1) {
		printf("--- MENU\n\n"
		       "[0] - Config baseline\n"
		       "[1] - Mesure heigth\n"
		       "---\n");

		char option = serial.readChar();

		delay_ms(100);

		switch (option) {
			case '0':
				baseline = heigth_config_baseline();
				printf("New baseline is: %ldcm\n", (long)baseline);
				break;
			case '1':
				printf("Heigth mesured is: %ldcm\n", (long)(baseline - heigth_mesure()));
				break;

			default:
				printf("ERRO: Unknown option\n");
				break;
		}
		printf("---\n");
		delay_ms(100);
	}
}
