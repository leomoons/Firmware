/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "BMI088.hpp"

/** driver 'main' command */
extern "C" { __EXPORT int bmi088_main(int argc, char *argv[]); }

namespace bmi088
{

BMI088    *g_dev_int; // on internal bus
BMI088    *g_dev_ext; // on external bus

void    start(bool, enum Rotation);
void    stop(bool);
void    info(bool);
void    regdump(bool);
void    usage();

/**
 * Start the driver.
 *
 * This function only returns if the driver is up and running
 * or failed to detect the sensor.
 */
void
start(bool external_bus, enum Rotation rotation)
{
	BMI088 **g_dev_ptr = external_bus ? &g_dev_ext : &g_dev_int;

	if (*g_dev_ptr != nullptr) {
		/* if already started, the still command succeeded */
		errx(0, "bmi088 sensor already started");
	}

	/* create the driver */
	*g_dev_ptr = new BMI088(PX4_SPI_BUS_SENSORS3, rotation);

	if (*g_dev_ptr == nullptr) {
		goto fail;
	}

	if (OK != (*g_dev_ptr)->init()) {
		goto fail;
	}

	// start automatic data collection
	(*g_dev_ptr)->start();

	exit(PX4_OK);

fail:

	if (*g_dev_ptr != nullptr) {
		delete (*g_dev_ptr);
		*g_dev_ptr = nullptr;
	}

	PX4_WARN("No BMI088 found");

	exit(PX4_ERROR);
}

void
stop(bool external_bus)
{
	BMI088 **g_dev_ptr = external_bus ? &g_dev_ext : &g_dev_int;

	if (*g_dev_ptr != nullptr) {
		delete *g_dev_ptr;
		*g_dev_ptr = nullptr;

	} else {
		/* warn, but not an error */
		warnx("bmi088 accel sensor already stopped.");
	}


	exit(0);
}

/**
 * Print a little info about the driver.
 */
void
info(bool external_bus)
{
	BMI088 **g_dev_ptr = external_bus ? &g_dev_ext : &g_dev_int;

	if (*g_dev_ptr == nullptr) {
		errx(1, "bmi088 driver not running");
	}

	printf("state @ %p\n", *g_dev_ptr);
	(*g_dev_ptr)->print_info();

	exit(0);
}

/**
 * Dump the register information
 */
void
regdump(bool external_bus)
{
	BMI088 **g_dev_ptr = external_bus ? &g_dev_ext : &g_dev_int;

	if (*g_dev_ptr == nullptr) {
		errx(1, "bmi088 driver not running");
	}

	printf("regdump @ %p\n", *g_dev_ptr);
	(*g_dev_ptr)->print_registers();

	exit(0);
}

void
usage()
{
	warnx("missing command: try 'start', 'info', 'stop', 'regdump'");
	warnx("options:");
	warnx("    -X    (external bus)");
	warnx("    -R    rotation");
	warnx("    -A    (Enable Accelerometer)");
	warnx("    -G    (Enable Gyroscope)");
}

} // namespace bmi088

int
bmi088_main(int argc, char *argv[])
{
	bool external_bus = false;
	int ch;
	enum Rotation rotation = ROTATION_NONE;
	int myoptind = 1;
	const char *myoptarg = NULL;

	/* jump over start/off/etc and look at options first */
	while ((ch = px4_getopt(argc, argv, "XR:AG", &myoptind, &myoptarg)) != EOF) {
		switch (ch) {
		case 'X':
			external_bus = true;
			break;

		case 'R':
			rotation = (enum Rotation)atoi(myoptarg);
			break;

		default:
			bmi088::usage();
			exit(0);
		}
	}

	if (myoptind >= argc) {
		bmi088::usage();
		exit(0);
	}


	const char *verb = argv[myoptind];

	/*
	 * Start/load the driver.
	 */
	if (!strcmp(verb, "start")) {
		bmi088::start(external_bus, rotation);
	}

	/*
	 * Stop the driver.
	 */
	if (!strcmp(verb, "stop")) {
		bmi088::stop(external_bus);
	}

	/*
	 * Print driver information.
	 */
	if (!strcmp(verb, "info")) {
		bmi088::info(external_bus);
	}

	/*
	 * Print register information.
	 */
	if (!strcmp(verb, "regdump")) {
		bmi088::regdump(external_bus);
	}

	bmi088::usage();
	exit(1);
}