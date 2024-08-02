/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-07-30     27644       the first version
 */
#ifndef APPLICATIONS_MQTT_H_
#define APPLICATIONS_MQTT_H_


int mqtt_example_main();
int example_subscribe(void *handle);
void mqtt_init();
int example_publish(void *handle);
int example_subscribe(void *handle);

#endif /* APPLICATIONS_MQTT_H_ */
