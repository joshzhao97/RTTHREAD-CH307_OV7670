/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-09-09     WCH        the first version
 */
 
#include "board.h"
#include <stdint.h>
#include "drv_usart.h"
#include <rthw.h>
#include <rtthread.h>


extern uint32_t SystemCoreClock;

void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t us_tick,tick_now,tick_last;
    uint32_t tick_cnt = 0;

    us_tick = (((SysTick->CMP)+1) / (1000000UL / RT_TICK_PER_SECOND));

    //us_tick = SystemCoreClock / 8000000UL;
    us_tick = us * us_tick;
    //rt_kprintf("us_tick:%d",us_tick);
    //tick_load = (SysTick->CMP) + 1;
    //rt_kprintf("tick_load%d",tick_load);
    //rt_kprintf("us_tick = %d\r\n",us_tick);
    tick_last = SysTick->CNT;
    //rt_kprintf("tick_last%d",tick_last);
    /*while(1)
    {
        tick_now = SysTick->CNT;
        //rt_kprintf("tick_now%d",tick_now);
        if (tick_now != tick_last)
                {
                    if (tick_now < tick_last)
                    {
                        tick_cnt += tick_last - tick_now;
                        //rt_kprintf("tick_cnt%d",tick_cnt);
                    }
                    else
                    {
                        tick_cnt += tick_load - tick_now + tick_last;
                        //rt_kprintf("tick_cnt%d",tick_cnt);
                    }
                    tick_last = tick_now;
                    if (tick_cnt >= us_tick)
                    {
                        rt_kprintf("tick_cnt%d",tick_cnt);
                        break;
                    }
                }
    }*/
    while(1)
    {
        tick_now = SysTick->CNT;
        tick_cnt = tick_now - tick_last;//很坑的是：CH32V307计数器是自增的不是自减，看数据手册上说是可以增减，但不知道哪个寄存器
        if(tick_cnt >= us_tick)
        {
            //rt_kprintf("now:%d",tick_cnt);

        break;
        }

    }
}


static uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    NVIC_SetPriority(SysTicK_IRQn,0xf0);
    NVIC_SetPriority(Software_IRQn,0xf0);
    NVIC_EnableIRQ(SysTicK_IRQn);
    NVIC_EnableIRQ(Software_IRQn);
    SysTick->CTLR=0;
    SysTick->SR=0;
    SysTick->CNT=0;
    SysTick->CMP = 0;
    SysTick->CMP=ticks-1;
    SysTick->CTLR=0xF;//使用内部时钟，即144MHZ
    return 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE (4096)
static uint32_t rt_heap[RT_HEAP_SIZE];
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    /* System Tick Configuration */
    _SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
    /* USART driver initialization is open by default */
#ifdef RT_USING_SERIAL
    rt_hw_usart_init();
#endif
#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

}

void LED1_BLINK_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler(void)
{
    GET_INT_SP();
    /* enter interrupt */
    rt_interrupt_enter();
    SysTick->SR=0;
    rt_tick_increase();
    /* leave interrupt */
    rt_interrupt_leave();
    FREE_INT_SP();

}


