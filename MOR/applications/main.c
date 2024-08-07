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
#include <drv_matrix_led.h>
#include <stdlib.h>
#include <time.h>

/* define LED  */
enum{
    EXTERN_LED_0,
    EXTERN_LED_1,
    EXTERN_LED_2,
    EXTERN_LED_3,
    EXTERN_LED_4,
    EXTERN_LED_5,
    EXTERN_LED_6,
    EXTERN_LED_7,
    EXTERN_LED_8,
    EXTERN_LED_9,
    EXTERN_LED_10,
    EXTERN_LED_11,
    EXTERN_LED_12,
    EXTERN_LED_13,
    EXTERN_LED_14,
    EXTERN_LED_15,
    EXTERN_LED_16,
    EXTERN_LED_17,
    EXTERN_LED_18,
};

#define KEY_LEFT         GET_PIN(C,0)
#define KEY_DOWM         GET_PIN(C,1)
#define KEY_RIGHT        GET_PIN(C,4)
#define KEY_UP           GET_PIN(C,5)
#define BUZZER           GET_PIN(B,0)


rt_thread_t led_matrix_thread;
rt_thread_t key_thread;
rt_thread_t moving_thread;
rt_thread_t justic_right;

static struct rt_mailbox mb;
static char mb_pool[128];

static struct rt_mailbox mb2;
static char mb_pool2[128];

static int16_t array[3][12]={{12,13,14,15,16,17,12,13,14,15,16,17},{0,1,2,3,4,5,6,7,8,9,10,11},{18,18,18,18,18,18,18,18,18,18,18,18}
};
static uint16_t save_array[3]={0,0,0};
static uint16_t score_right[3]={0,0,0};
uint16_t i=2;
int j=0;
uint16_t record1,record2,record3;
uint16_t m2=0;
static int16_t start=1;
static int16_t right=0;

static char mb_str1[] = "key_left";
static char mb_str2[] = "key_right";
static char mb_str3[] = "key_down";
static char mb_str4[] = "key_up";

static char record_str1[] = "start";
static char record_str2[] = "end";
static char record_str3[] = "right";
// 初始化随机数发生器
void init_random(void)
{
    srand((unsigned int)rt_tick_get());
}

// 生成一个 1 到 18 的随机数
int generate_random_number(void)
{
    return rand() % 18;
}


static void led_matrix_example_entry()
{
    int count = 0;
    int score=0;
    static int random1,random2,random3;
    init_random();
    int m=0;
    char str2;

    while (1)
    {

        if(start++<4){
                led_matrix_clear();
                srand((unsigned int)time(NULL));
                random1=generate_random_number();
                //random2=generate_random_number();
                //random3=generate_random_number();
                //led_matrix_set_color(18,LT_WHITE);
                /*if(random1==random2){
                    random1=random2-random1+1;
                }else if (random1==random3) {
                    random1=random3-random1+1;
                }else if (random2==random3) {
                    random2=random3-random2+1;
                }*/
                rt_kprintf("random1 is %d\r\n",random1);
                //rt_kprintf("random2 is %d\r\n",random2);
                //rt_kprintf("random3 is %d\r\n",random3);
                led_matrix_set_color(random1, LT_GREEN);
                //led_matrix_set_color(random2, LT_GREEN);
                //led_matrix_set_color(random3, LT_GREEN);
                save_array[start-2]=random1;
                //save_array[1]=random2;
                //save_array[2]=random3;
                led_matrix_reflash();
                random1=random2=random3=0;

        }else {
            led_matrix_clear();
        }
        led_matrix_set_color(array[i][j],LT_BLUE);
        lcd_show_string(0,20,24,"your choose one:");
        lcd_show_string(200, 20, 24, "%d", score_right[0]);

        lcd_show_string(0,48,24,"your choose two:");
        lcd_show_string(200, 48, 24, "%d", score_right[1]);

        lcd_show_string(0,74,24,"your choose tre:");
        lcd_show_string(200, 74, 24, "%d", score_right[2]);
        rt_thread_mdelay(1000);
    }
}

static void right_thread(){
    char str3;
    while(1){
        /*if (rt_mb_recv(&mb2, (rt_uint32_t *)&str3, RT_WAITING_FOREVER) == RT_EOK){
            if(str3==record_str3){
                save_array[0]
            }
        }*/

        rt_thread_mdelay(200);
    }
}



static void key_thread_entry(){
    while(1){
        if(rt_pin_read(KEY_DOWM)==PIN_LOW){
            rt_thread_delay(100);
            if(rt_pin_read(KEY_DOWM)==PIN_LOW){
                //rt_kprintf("key_DOWN press\r\n");
                rt_mb_send(&mb, (rt_uint32_t)&mb_str3);
                rt_thread_delay(30);
            }
        }else if (rt_pin_read(KEY_UP)==PIN_LOW) {
            rt_thread_delay(200);
            if(rt_pin_read(KEY_UP==PIN_LOW)){
                //rt_kprintf("key_UP press\r\n");
                rt_mb_send(&mb, (rt_uint32_t)&mb_str4);
                rt_thread_delay(50);
            }
        }else if (rt_pin_read(KEY_RIGHT)==PIN_LOW) {
            rt_thread_delay(200);
            if(rt_pin_read(KEY_RIGHT==PIN_LOW)){
                //rt_kprintf("KEY_RIGHT press\r\n");
                rt_mb_send(&mb, (rt_uint32_t)&mb_str2);
                rt_thread_delay(50);
            }
        }else if (rt_pin_read(KEY_LEFT)==PIN_LOW) {
            rt_thread_delay(200);
            if(rt_pin_read(KEY_LEFT==PIN_LOW)){
                //rt_kprintf("KEY_LEFT press\r\n");
                rt_mb_send(&mb, (rt_uint32_t)&mb_str1);
                rt_thread_delay(50);
            }
        }
        rt_thread_mdelay(80);
    }
}
static void move_thread_entry(){
    char *str;

    while(1){
        //led_matrix_reflash();

        if (rt_mb_recv(&mb, (rt_uint32_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {
            //rt_kprintf("thread move: get a mail from mailbox, the content:%s\n", str);
            while (str == mb_str2){  //right  向右
                j+=1;
                if(j>11) j=0;
                rt_kprintf("%s\r\n",str);break;
            }while (str == mb_str1) {  //left  向左
                j-=1;
                if(j<0) j=11;
                rt_kprintf("%s\r\n",str);break;
            }while (str == mb_str3) { //0内圈  1 外圈 2中心  down切换
                i+=1;
                if(i>2) i=0;
                rt_kprintf("%s\r\n",str);break;
            }while(str == mb_str4) {  //up 确认按键
                if(m2>2) right=1;
                if(right){
                    if(save_array[0]==score_right[0]&&save_array[1]==score_right[1]&&save_array[2]==score_right[2]){
                        lcd_show_string(0, 200, 24, "SUCCESS one");
                        right=0;
                        m2=0;
                    }else{
                        m2=0;
                        right=0;
                        score_right[0]=score_right[1]=score_right[2]=0;
                        lcd_clear(WHITE);
                    }
                }else {
                   score_right[m2]=array[i][j];
                   rt_kprintf("score right %d,m2 is %d\r\n",score_right[m2],m2);
                   m2+=1;
                }
                rt_kprintf("%s\r\n",str);break;
            }


            /* 延时 100ms */
            rt_thread_mdelay(100);
        }
    }
}

int main(void)
{
    rt_err_t result,result2;
    static rt_uint8_t lcd_init = 0;
        rt_device_t lcd = RT_NULL;

    if (lcd_init == 0)
    {
       lcd_init = 1;
       lcd = rt_device_find("lcd");
       rt_device_init(lcd);
    }
    result = rt_mb_init(&mb,"mbt",&mb_pool[0],sizeof(mb_pool) / 4,RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
       rt_kprintf("init mailbox failed.\n");
       return -1;
    }

    result2 = rt_mb_init(&mb2,"mbt2",&mb_pool2[0],sizeof(mb_pool2) / 4,RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result2 != RT_EOK)
    {
       rt_kprintf("init mailbox2 failed.\n");
       return -1;
    }

    rt_pin_mode(KEY_DOWM, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY_LEFT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY_RIGHT, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(KEY_UP, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(BUZZER, PIN_MODE_OUTPUT);

    led_matrix_thread = rt_thread_create("led matrix ", led_matrix_example_entry, RT_NULL, 1024, 23, 20);
    if(led_matrix_thread == RT_NULL)
    {
        rt_kprintf("led matrix  thread creat failed!\n");
        return 0;
    }
    key_thread=rt_thread_create("key thread", key_thread_entry, RT_NULL, 512, 20, 20);
    if(key_thread ==RT_NULL){
        rt_kprintf("key thread failed\r\n");
        return 0;
    }
    moving_thread=rt_thread_create("move thread", move_thread_entry, RT_NULL, 512, 21, 20);
    if(moving_thread ==RT_NULL){
        rt_kprintf("move thread failed\r\n");
        return 0;
    }
    justic_right=rt_thread_create("right justic thread", right_thread, RT_NULL, 512, 24, 20);
    if(justic_right ==RT_NULL){
        rt_kprintf("justic_right thread failed\r\n");
        return 0;
    }

    rt_thread_mdelay(200); // avoid multi-thread on LED matrix transmit.
    rt_thread_startup(key_thread);
    rt_thread_startup(led_matrix_thread);
    rt_thread_startup(moving_thread);
    rt_thread_startup(justic_right);
    lcd_clear(WHITE);
    lcd_show_string(20, 120, 32, "GAME STARTING");

    return 0;
}

