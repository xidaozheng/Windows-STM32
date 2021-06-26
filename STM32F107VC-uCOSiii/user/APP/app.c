/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  LED1ToggleTastStart  (void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;

	
    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",				/* ��������												*/
                 (OS_TASK_PTR ) LED1ToggleTastStart,			/* ָ����������ָ��									*/
                 (void       *) 0,								/* ����Ĳ��� �����������ָ�� �������ݽṹ				*/
                 (OS_PRIO     ) APP_TASK_START_PRIO,			/* �������ȼ� �˴����õ�2								*/
                 (CPU_STK    *)&AppTaskStartStk[0],				/* �����ջ�Ļ���ַ										*/
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,	/* ��ַ��ˮӡ�� ����ջ������90%λ��ʱ�Ͳ�������������		*/
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,		/* ����Ķ�ջ��С(��λ��unsigned  int) 4���ֽ�			*/
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);														/*�������*/

    if(err != OS_ERR_NONE)
	{
		/* The task didn't get created. Lookup the value of the error code ... 	*/
		/* ... in OS.H for the meaning of the error								*/
	}
				 
	OSStart(&err);                                             /* Start multitasking (i.e. give control to uC/OS-III). */
	if(err != OS_ERR_NONE)
	{
		/* Your code is NEVER supposed to come back to this point.				*/
	}
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  LED1ToggleTastStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();													/* uC/CPU���ڲ����ж���Ӧʱ�䣬��ȡʱ������ṩ����ļ�������ָ���*/

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* ����uC/OS-III��ʱ���ж�*/
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    CPU_IntDisMeasMaxCurReset();


    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
			bspLedToggle(GPIOD, GPIO_Pin_13 | GPIO_Pin_14);
			OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );
    }
}





