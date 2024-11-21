/*
 * Copyright (c) 2023 Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : task_system.c
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/
/* Project includes. */
#include "main.h"

/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "board.h"
#include "app.h"
#include "task_system_attribute.h"
#include "task_system_interface.h"
#include "display.h"

/********************** macros and definitions *******************************/
#define G_TASK_SYS_CNT_INI			0ul
#define G_TASK_SYS_TICK_CNT_INI		0ul

#define DEL_SYS_XX_MIN				0ul
#define DEL_SYS_XX_MED				50ul
#define DEL_SYS_XX_MAX				500ul

#define MAX_LINEA_DISPLAY			20

/********************** internal data declaration ****************************/
task_system_dta_t task_system_dta = {
	ST_SYS_XX_MAIN, EV_SYS_XX_IDLE,
	{
		{ MOTOR_1, POWER_OFF, SPEED_0, SPIN_LEFT },
		{ MOTOR_2, POWER_OFF, SPEED_0, SPIN_LEFT },
	}, 0, 0, 0,
};

#define SYSTEM_DTA_QTY	(sizeof(task_system_dta)/sizeof(task_system_dta_t))

/********************** internal functions declaration ***********************/
void decoracion();

void pantalla_main(task_system_dta_t* p_task_system_dta);

void pantalla_menu_motores(task_system_dta_t* p_task_system_dta);

void pantalla_menu_propiedades(task_system_dta_t* p_task_system_dta);

void pantalla_menu_valores_power(task_system_dta_t* p_task_system_dta);
void pantalla_menu_valores_speed(task_system_dta_t* p_task_system_dta);
void pantalla_menu_valores_spin(task_system_dta_t* p_task_system_dta);


/********************** internal data definition *****************************/
const char *p_task_system 		= "Task System (System Statechart)";
const char *p_task_system_ 		= "Non-Blocking & Update By Time Code";

/********************** external data declaration ****************************/
uint32_t g_task_system_cnt;
volatile uint32_t g_task_system_tick_cnt;

/********************** external functions definition ************************/
void task_system_init(void *parameters)
{
	task_system_dta_t 	*p_task_system_dta;
	task_system_st_t	state;
	task_system_ev_t	event;

	/* Print out: Task Initialized */
	LOGGER_LOG("  %s is running - %s\r\n", GET_NAME(task_system_init), p_task_system);
	LOGGER_LOG("  %s is a %s\r\n", GET_NAME(task_system), p_task_system_);

	g_task_system_cnt = G_TASK_SYS_CNT_INI;

	/* Print out: Task execution counter */
	LOGGER_LOG("   %s = %lu\r\n", GET_NAME(g_task_system_cnt), g_task_system_cnt);

	init_queue_event_task_system();

	/* Update Task Actuator Configuration & Data Pointer */
	p_task_system_dta = &task_system_dta;

	/* Print out: Task execution FSM */
	state = p_task_system_dta->state;
	LOGGER_LOG("   %s = %lu", GET_NAME(state), (uint32_t)state);

	event = p_task_system_dta->event;
	LOGGER_LOG("   %s = %lu\n", GET_NAME(event), (uint32_t)event);


	g_task_system_tick_cnt = G_TASK_SYS_TICK_CNT_INI;

    displayInit( DISPLAY_CONNECTION_GPIO_4BITS );

    decoracion();
	pantalla_main(p_task_system_dta);
}


void task_system_update(void *parameters)
{
	task_system_dta_t *p_task_system_dta;
	bool b_time_update_required = false;

	/* Update Task System Counter */
	g_task_system_cnt++;

	/* Protect shared resource (g_task_system_tick) */
	__asm("CPSID i");	/* disable interrupts*/
    if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt)
    {
    	g_task_system_tick_cnt--;
    	b_time_update_required = true;
    }
    __asm("CPSIE i");	/* enable interrupts*/

    while (b_time_update_required)
    {
		/* Protect shared resource (g_task_system_tick) */
		__asm("CPSID i");	/* disable interrupts*/
		if (G_TASK_SYS_TICK_CNT_INI < g_task_system_tick_cnt)
		{
			g_task_system_tick_cnt--;
			b_time_update_required = true;
		}
		else
		{
			b_time_update_required = false;
		}
		__asm("CPSIE i");	/* enable interrupts*/

    	/* Update Task System Data Pointer */
		p_task_system_dta = &task_system_dta;
		if (!any_event_task_system()){
			continue;
		}
		p_task_system_dta->event = get_event_task_system();

		switch (p_task_system_dta->state) {
			case ST_SYS_XX_MAIN:
				if (p_task_system_dta->event == EV_SYS_XX_MENU) {
					p_task_system_dta->state = ST_SYS_XX_MENU_MOTORES;
					p_task_system_dta->indice_motores = 0;
					pantalla_menu_motores(p_task_system_dta);

				}
				break;

			case ST_SYS_XX_MENU_MOTORES:
				if (p_task_system_dta->event == EV_SYS_XX_ESCAPE) {
					p_task_system_dta->state = ST_SYS_XX_MAIN;
					pantalla_main(p_task_system_dta);

				} else if (p_task_system_dta->event == EV_SYS_XX_NEXT) {
					p_task_system_dta->indice_motores = (p_task_system_dta->indice_motores + 1) % MAX_MOTORES;
					pantalla_menu_motores(p_task_system_dta);

				} else if (p_task_system_dta->event == EV_SYS_XX_ENTER) {
					p_task_system_dta->state = ST_SYS_XX_MENU_PROPIEDADES;
					p_task_system_dta->indice_propiedades = 0;
					pantalla_menu_propiedades(p_task_system_dta);

				}

				break;
			case ST_SYS_XX_MENU_PROPIEDADES:
				if (p_task_system_dta->event == EV_SYS_XX_ESCAPE) {
					p_task_system_dta->state = ST_SYS_XX_MENU_MOTORES;
					pantalla_menu_motores(p_task_system_dta);

				} else if (p_task_system_dta->event == EV_SYS_XX_NEXT) {
					p_task_system_dta->indice_propiedades = (p_task_system_dta->indice_propiedades + 1) % MAX_PROPIEDADES;
					pantalla_menu_propiedades(p_task_system_dta);

				} else if (p_task_system_dta->event == EV_SYS_XX_ENTER) {
					motor_t* motor = p_task_system_dta->motores + p_task_system_dta->indice_motores;

					switch (p_task_system_dta->indice_propiedades) {
						case POWER:
							p_task_system_dta->state = ST_SYS_XX_MENU_VALORES_POWER;
							p_task_system_dta->indice_valores = (uint32_t)motor->power;
							pantalla_menu_valores_power(p_task_system_dta);
							break;

						case SPEED:
							p_task_system_dta->state = ST_SYS_XX_MENU_VALORES_SPEED;
							p_task_system_dta->indice_valores = (uint32_t)motor->speed;
							pantalla_menu_valores_speed(p_task_system_dta);
							break;

						case SPIN:
							p_task_system_dta->state = ST_SYS_XX_MENU_VALORES_SPIN;
							p_task_system_dta->indice_valores = (uint32_t)motor->spin;
							pantalla_menu_valores_spin(p_task_system_dta);
							break;

						default: // ASSERT("Indice propiedades no deberia tener un valor mayor a %i\n", MAX_PROPIEDADES);
					}
				}

				break;
			case ST_SYS_XX_MENU_VALORES_POWER:
				if (p_task_system_dta->event == EV_SYS_XX_ESCAPE) {
					p_task_system_dta->state = ST_SYS_XX_MENU_PROPIEDADES;
					pantalla_menu_propiedades(p_task_system_dta);

				} else if(p_task_system_dta->event == EV_SYS_XX_NEXT) {
					p_task_system_dta->indice_valores = (p_task_system_dta->indice_valores + 1) % MAX_POWER;
					pantalla_menu_valores_power(p_task_system_dta);

				} else if (p_task_system_dta->event == EV_SYS_XX_ENTER) {
					p_task_system_dta->state = ST_SYS_XX_MENU_PROPIEDADES;
					motor_t* motor = p_task_system_dta->motores + p_task_system_dta->indice_motores;
					motor->power = (task_power_t)p_task_system_dta->indice_valores;
					pantalla_menu_propiedades(p_task_system_dta);
				}

				break;
			case ST_SYS_XX_MENU_VALORES_SPEED:
				if (p_task_system_dta->event == EV_SYS_XX_ESCAPE) {
					p_task_system_dta->state = ST_SYS_XX_MENU_PROPIEDADES;
					pantalla_menu_propiedades(p_task_system_dta);

				} else if(p_task_system_dta->event == EV_SYS_XX_NEXT) {
					p_task_system_dta->indice_valores = (p_task_system_dta->indice_valores + 1) % MAX_SPEED;
					pantalla_menu_valores_speed(p_task_system_dta);

				} else if (p_task_system_dta->event == EV_SYS_XX_ENTER) {
					p_task_system_dta->state = ST_SYS_XX_MENU_PROPIEDADES;
					motor_t* motor = p_task_system_dta->motores + p_task_system_dta->indice_motores;
					motor->speed = (task_speed_t)p_task_system_dta->indice_valores;
					pantalla_menu_propiedades(p_task_system_dta);
				}

				break;
			case ST_SYS_XX_MENU_VALORES_SPIN:
				if (p_task_system_dta->event == EV_SYS_XX_ESCAPE) {
					p_task_system_dta->state = ST_SYS_XX_MENU_PROPIEDADES;
					pantalla_menu_propiedades(p_task_system_dta);

				} else if(p_task_system_dta->event == EV_SYS_XX_NEXT) {
					p_task_system_dta->indice_valores = (p_task_system_dta->indice_valores + 1) % MAX_SPIN;
					pantalla_menu_valores_spin(p_task_system_dta);

				} else if (p_task_system_dta->event == EV_SYS_XX_ENTER) {
					p_task_system_dta->state = ST_SYS_XX_MENU_PROPIEDADES;
					motor_t* motor = p_task_system_dta->motores + p_task_system_dta->indice_motores;
					motor->spin = (task_spin_t)p_task_system_dta->indice_valores;
					pantalla_menu_propiedades(p_task_system_dta);
				}

				break;
			default:

				break;
		}
	}
}

void decoracion() {
	for (int i = 0; i < 2; i++) {
		displayCharPositionWrite(0, 3 * i);
		displayStringWrite("--------------------");
	}
}

void limpiar_linea(uint32_t inicio, uint32_t linea) {
	char test_str[20];
	displayCharPositionWrite(inicio, linea);

	for (uint32_t posicion = inicio, i = 0; posicion < MAX_LINEA_DISPLAY; posicion++, i++)
		test_str[i] = ' ';
	test_str[MAX_LINEA_DISPLAY - inicio] = '\0';
	displayStringWrite(test_str);
}

void pantalla_main(task_system_dta_t* p_task_system_dta) {
	char test_str[20];
	for (int i = 0; i < 2; i++) {
		motor_t motor = p_task_system_dta->motores[i];

		char* power = (motor.power == POWER_OFF) ? "OFF" : "ON";
		int speed = (int)(motor.speed);
		char* spin = (motor.spin == SPIN_LEFT) ? "L" : "R";

		snprintf(test_str, sizeof(test_str), "Motor %i: %s, %i, %s", i + 1, power, speed, spin);
		displayCharPositionWrite(0, i + 1);
		displayStringWrite(test_str);
	}
}

void pantalla_menu_motores(task_system_dta_t* p_task_system_dta) {
	char test_str[20];

	displayCharPositionWrite(0, 1);
	displayStringWrite("Enter/Next/Escape   ");

	limpiar_linea(0, 2);

	snprintf(test_str, sizeof(test_str), " > Motor %i", (int)p_task_system_dta->indice_motores + 1);
	displayCharPositionWrite(0, 2);
	displayStringWrite(test_str);
}

void pantalla_menu_propiedades(task_system_dta_t* p_task_system_dta) {
	char test_str[17];
	switch (p_task_system_dta->indice_propiedades) {
		case POWER: snprintf(test_str, sizeof(test_str), "%s", "Power"); break;
		case SPEED: snprintf(test_str, sizeof(test_str), "%s", "Speed"); break;
		case SPIN: snprintf(test_str, sizeof(test_str), "%s", "Spin"); break;
	}

	limpiar_linea(3, 2);

	displayCharPositionWrite(3, 2);
	displayStringWrite(test_str);
}

void pantalla_menu_valores_power(task_system_dta_t* p_task_system_dta) {
	char test_str[17];
	switch (p_task_system_dta->indice_valores) {
		case POWER_ON: snprintf(test_str, sizeof(test_str), "%s", "ON"); break;
		case POWER_OFF: snprintf(test_str, sizeof(test_str), "%s", "OFF"); break;
	}

	limpiar_linea(3, 2);

	displayCharPositionWrite(3, 2);
	displayStringWrite(test_str);
}

void pantalla_menu_valores_speed(task_system_dta_t* p_task_system_dta) {
	char test_str[17];
	snprintf(test_str, sizeof(test_str), "%i", (int)p_task_system_dta->indice_valores);

	limpiar_linea(3, 2);

	displayCharPositionWrite(3, 2);
	displayStringWrite(test_str);
}

void pantalla_menu_valores_spin(task_system_dta_t* p_task_system_dta) {
	char test_str[17];
	switch (p_task_system_dta->indice_valores) {
		case SPIN_LEFT: snprintf(test_str, sizeof(test_str), "%s", "LEFT"); break;
		case SPIN_RIGHT: snprintf(test_str, sizeof(test_str), "%s", "RIGHT"); break;
	}

	limpiar_linea(3, 2);

	displayCharPositionWrite(3, 2);
	displayStringWrite(test_str);
}

/********************** end of file ******************************************/
