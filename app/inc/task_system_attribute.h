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
 * @file   : task_system_attribute.h
 * @date   : Set 26, 2023
 * @author : Juan Manuel Cruz <jcruz@fi.uba.ar> <jcruz@frba.utn.edu.ar>
 * @version	v1.0.0
 */

#ifndef TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_
#define TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/* System Statechart - State Transition Table */
/* 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 * 	| Current               | Event                 |                       | Next                  |                       |
 * 	| State                 | (Parameters)          | [Guard]               | State                 | Actions               |
 * 	|=======================+=======================+=======================+=======================+=======================|
 * 	| ST_SYS_XX_IDLE        | EV_SYS_XX_ACTIVE      |                       | ST_SYS_XX_ACTIVE      | put_event_t.._actuator|
 * 	|                       |                       |                       |                       | (event, identifier)   |
 * 	|-----------------------+-----------------------+-----------------------+-----------------------+-----------------------|
 * 	| ST_SYS_XX_ACTIVE      | EV_SYS_XX_IDLE        |                       | ST_SYS_XX_IDLE        | put_event_t.._actuator|
 * 	|                       |                       |                       |                       | (event, identifier)   |
 * 	------------------------+-----------------------+-----------------------+-----------------------+------------------------
 */

/* Events to excite Task System */
typedef enum task_system_ev {
	EV_SYS_XX_IDLE,
	EV_SYS_XX_MENU,
	EV_SYS_XX_ENTER,
	EV_SYS_XX_NEXT,
	EV_SYS_XX_ESCAPE,
} task_system_ev_t;

/* State of Task System */
typedef enum task_system_st {
	ST_SYS_XX_MAIN,
	ST_SYS_XX_MENU_MOTORES,
	ST_SYS_XX_MENU_PROPIEDADES,
	ST_SYS_XX_MENU_VALORES_POWER,
	ST_SYS_XX_MENU_VALORES_SPEED,
	ST_SYS_XX_MENU_VALORES_SPIN,
} task_system_st_t;

typedef enum task_motores {
	MOTOR_1,
	MOTOR_2,

	MAX_MOTORES,
} task_motores_t;

typedef enum task_propiedades {
	POWER,
	SPEED,
	SPIN,

	MAX_PROPIEDADES,
} task_propiedades_t;

typedef enum task_power {
	POWER_OFF,
	POWER_ON,

	MAX_POWER,
} task_power_t;

typedef enum task_speed {
	SPEED_0,
	SPEED_1,
	SPEED_2,
	SPEED_3,
	SPEED_4,
	SPEED_5,
	SPEED_6,
	SPEED_7,
	SPEED_8,
	SPEED_9,

	MAX_SPEED,
} task_speed_t;

typedef enum task_spin {
	SPIN_LEFT,
	SPIN_RIGHT,

	MAX_SPIN,
} task_spin_t;

typedef struct motor {
	task_motores_t		motor_id;
	task_power_t		power;
	task_speed_t		speed;
	task_spin_t			spin;
} motor_t;

typedef struct {
	task_system_st_t	state;
	task_system_ev_t	event;
	motor_t				motores[MAX_MOTORES];
	uint32_t			indice_motores;
	uint32_t			indice_propiedades;
	uint32_t			indice_valores;
} task_system_dta_t;

/********************** external data declaration ****************************/
extern task_system_dta_t task_system_dta;

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_INC_TASK_SYSTEM_ATTRIBUTE_H_ */

/********************** end of file ******************************************/
