/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "aht10.h"
#include "mqtt.h"
#include "spi_wifi_rw007.h"
#include "dev_sign_api.h"
#include "mqtt_api.h"
#include "wrappers_defs.h"

float humidity, temperature;


#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
//线程1  温湿度
#define THREAD1_PRIORITY         21
#define THREAD1_STACK_SIZE       1024
#define THREAD1_TIMESLICE        50
static void thread1_entry(void *para);

//线程二  mqtt
#define THREAD2_PRIORITY         25
#define THREAD2_STACK_SIZE       1024
#define THREAD2_TIMESLICE        200
static void thread2_entry(void *para);

//线程三  其它外设 LED 按键
#define THREAD3_PRIORITY         27
#define THREAD3_STACK_SIZE       512
#define THREAD3_TIMESLICE        80
static void thread3_entry(void *para);

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;
static rt_thread_t tid3 = RT_NULL;

char *ssid = "zjj827";
char *password = "z5685481";

int main(void)
{
    rt_wlan_scan();
    rt_err_t sign;
    sign=rt_wlan_connect(ssid,password);
    while(1){
        if(sign==RT_EOK) break;
    }
    if(sign!=RT_EOK){rt_kprintf("wifi connect error");return 0;}

    rt_kprintf("wifi successful connect\r\n");


    tid1=rt_thread_create("thread1", thread1_entry, RT_NULL, THREAD1_STACK_SIZE, THREAD1_PRIORITY, THREAD1_TIMESLICE);
    if(tid1!=RT_NULL){
        rt_thread_startup(tid1);
        rt_kprintf("start thread1\n");
    }else{return 1;}

    tid2=rt_thread_create("thread2", thread2_entry, RT_NULL, THREAD2_STACK_SIZE, THREAD2_PRIORITY, THREAD2_TIMESLICE);
    if(tid2!=RT_NULL){
        rt_thread_startup(tid2);
        rt_kprintf("start thread2\n");
        }else{return 1;}

    tid3=rt_thread_create("thread3", thread3_entry, RT_NULL, THREAD3_STACK_SIZE, THREAD3_PRIORITY, THREAD3_TIMESLICE);
    if(tid3!=RT_NULL){
          //rt_thread_startup(tid3);
          rt_kprintf("start thread3\n");
      }else{return 1;}

    return 0;
}

//线程1开启温湿度
static void thread1_entry(void *para){
    /*------------------温湿度---------------*/
    aht10_device_t dev;
   /* 总线名称 */
    const char *i2c_bus_name = "i2c3";
    /* 等待传感器正常工作 */
    rt_thread_mdelay(2000);
/* 初始化 aht10 */
    dev = aht10_init(i2c_bus_name);
    if (dev == RT_NULL)
    {
    rt_kprintf(" The sensor initializes failure");
    return 0;
    }
    /*--------------------结束------------------*/

        while (1)
        {
            /* 读取湿度 */
            humidity = aht10_read_humidity(dev);
            rt_kprintf("humidity   : %d.%d %%\r\n", (int)humidity, (int)(humidity * 10) % 10);
            /* 读取温度 */
            temperature = aht10_read_temperature(dev);
            rt_kprintf("temperature: %d.%d\r\n", (int)temperature, (int)(temperature * 10) % 10);
            rt_thread_mdelay(5000);
        }
}
//开启阿里云 mqtt
static void thread2_entry(void *para){
    char str1[]="ali_app";
    system(str1);
    while(1){

        rt_thread_mdelay(2000);
        }
}

static void thread3_entry(void *para){
    while(1){

        rt_thread_mdelay(1000);
    }

}
