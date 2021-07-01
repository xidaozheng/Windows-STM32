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
static 	OS_Q	 Msg_Q;						//声明消息队列
static 	OS_SEM	 SemOfLed;

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/
static  OS_TCB   AppTaskStartTCB;			//任务控制块
static	OS_TCB	 AppTaskLed1TCB;
static	OS_TCB	 AppTaskLed2TCB;
static	OS_TCB	 AppTaskLed3TCB;
static	OS_TCB	 AppTaskUsart2TCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];			//任务堆栈
static  CPU_STK  AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static  CPU_STK  AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static  CPU_STK  AppTaskLed3Stk[APP_TASK_LED3_STK_SIZE];
static  CPU_STK  AppTaskUsart2Stk[APP_TASK_USART2_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void AppTaskStart  (void *p_arg);				//任务函数声明
static	void AppTaskLed1(void *p_arg);
static	void AppTaskLed2(void *p_arg);
static	void AppTaskLed3(void *p_arg);
static	void AppTaskUsart2(void *p_arg);


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


    OSInit(&err);    											//初始化uC/OS-III                                          
                                            

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                //任务控制块地址
                 (CPU_CHAR   *)"App Task Start",				//任务名称
                 (OS_TASK_PTR ) AppTaskStart,					//任务函数
                 (void       *) 0,								//传递给任务函数(形参p_arg)的实参
                 (OS_PRIO     ) APP_TASK_START_PRIO,			//任务的优先级
                 (CPU_STK    *)&AppTaskStartStk[0],				//任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,	//地址“水印” 当堆栈生长到90%位置时就不再允许其生长
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,		//任务的堆栈大小(单位：unsigned  int) 4个字节
                 (OS_MSG_QTY  ) 5u, 							//任务可接受的最大消息数
                 (OS_TICK     ) 0u,								//任务的时间片节拍数(0代表默认值OSCfg_TickRate_Hz/10)
                 (void       *) 0,								//任务扩展(0代表不扩展)
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);								

    if(err != OS_ERR_NONE)
	{
		/* The task didn't get created. Lookup the value of the error code ... 	*/
		/* ... in OS.H for the meaning of the error								*/
	}

	//全局只需要一个启动
	OSStart(&err);                                             	//启动多任务管理（交由uC/OS-III控制）
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
static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
   (void)p_arg;
    BSP_Init();                                                 //板级初始化
    CPU_Init();													//初始化 CPU 组件 （时间戳、关中断时间测量和主机名）
    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器
	
    Mem_Init();  												//初始化 内存管理组件（堆内存池和内存池表）
                                               
#if OS_CFG_STAT_TASK_EN > 0u									//如果使能了统计任务
    OSStatTaskCPUUsageInit(&err);                               //计算没有应用任务(只有空闲任务)运行时 CPU 的（最大）
#endif															//容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU ...
																//... 使用率使用）。
																
    CPU_IntDisMeasMaxCurReset();								//复位（清零）当前最大关中断时间

	//由于三个任务的优先级相同，所以需要配置时间片进行轮转调度
	OSSchedRoundRobinCfg ((CPU_BOOLEAN   )DEF_ENABLED,			//使能时间片轮转调度
                          (OS_TICK       )0,					//把OSCfg_TickRate_Hz / 10 设为默认时间片值
                          (OS_ERR       *)&err
						 );

	//创建消息队列 queue
	OSQCreate(	(OS_Q 			*)&Msg_Q, 				//指向消息队列的指针
				(CPU_CHAR		*)"Queure For Test",	//队列的名字 
				(OS_MSG_QTY		 )20, 					//最多可存放消息的数目
				(OS_ERR			*)&err);
	if(err != OS_ERR_NONE)
	{
		printf("OSQCreate #err=%d\n", err);
	}
	
	//创建二值信号量
	OSSemCreate (	(OS_SEM      *)&SemOfLed,
					(CPU_CHAR    *)"SemOfLed",
					(OS_SEM_CTR   )0,			//信号量这里是指示事件发生，所以赋值0，表示时间还没有发生
					(OS_ERR      *)&err);

	OSTaskCreate ( (OS_TCB		*)      &AppTaskLed1TCB,
                   (CPU_CHAR	*)      "App Task Led1",
                   (OS_TASK_PTR  )  	AppTaskLed1,
                   (void        *)		0u,
                   (OS_PRIO      )  	APP_TASK_LED1_PRIO,
                   (CPU_STK     *)		&AppTaskLed1Stk[0],
                   (CPU_STK_SIZE )  	APP_TASK_LED1_STK_SIZE/10,
                   (CPU_STK_SIZE )  	APP_TASK_LED1_STK_SIZE,
                   (OS_MSG_QTY   )  	5u,
                   (OS_TICK      )  	0u,
                   (void        *)		0,
                   (OS_OPT       )  	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                   (OS_ERR      *)		&err
				 );
				   
	OSTaskCreate ( (OS_TCB		*)      &AppTaskLed2TCB,
                   (CPU_CHAR	*)      "App Task Led2",
                   (OS_TASK_PTR  )  	AppTaskLed2,
                   (void        *)		0u,
                   (OS_PRIO      )  	APP_TASK_LED2_PRIO,
                   (CPU_STK     *)		&AppTaskLed2Stk[0],
                   (CPU_STK_SIZE )  	APP_TASK_LED2_STK_SIZE/10,
                   (CPU_STK_SIZE )  	APP_TASK_LED2_STK_SIZE,
                   (OS_MSG_QTY   )  	5u,
                   (OS_TICK      )  	0u,
                   (void        *)		0,
                   (OS_OPT       )  	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                   (OS_ERR      *)		&err
				 );
				   
	OSTaskCreate ( (OS_TCB		*)      &AppTaskLed3TCB,
                   (CPU_CHAR	*)      "App Task Led3",
                   (OS_TASK_PTR  )  	AppTaskLed3,
                   (void        *)		0u,
                   (OS_PRIO      )  	APP_TASK_LED3_PRIO,
                   (CPU_STK     *)		&AppTaskLed3Stk[0],
                   (CPU_STK_SIZE )  	APP_TASK_LED3_STK_SIZE/10,
                   (CPU_STK_SIZE )  	APP_TASK_LED3_STK_SIZE,
                   (OS_MSG_QTY   )  	5u,
                   (OS_TICK      )  	0u,
                   (void        *)		0,
                   (OS_OPT       )  	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                   (OS_ERR      *)		&err
				 );
				   
	OSTaskCreate ( (OS_TCB		*)      &AppTaskUsart2TCB,
				   (CPU_CHAR	*)      "App Task Usart2",
				   (OS_TASK_PTR  )  	AppTaskUsart2,
				   (void        *)		0u,
				   (OS_PRIO      )  	APP_TASK_USART2_PRIO,
				   (CPU_STK     *)		&AppTaskUsart2Stk[0],
				   (CPU_STK_SIZE )  	APP_TASK_USART2_STK_SIZE/10,
				   (CPU_STK_SIZE )  	APP_TASK_USART2_STK_SIZE,
				   (OS_MSG_QTY   )  	5u,
				   (OS_TICK      )  	0u,
				   (void        *)		0,
				   (OS_OPT       )  	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				   (OS_ERR      *)		&err
			 );


	OSTaskDel(&AppTaskStartTCB, &err);	//删除主任务
	if(err != OS_ERR_NONE)
	{
		
	}
}
static	void AppTaskLed1(void *p_arg)
{
	OS_ERR      err;
	OS_REG		value;
	
	(void)p_arg;
	
	while(DEF_TRUE)
	{
		bspLedToggle(GPIOD, GPIO_Pin_13);
		
		OSQPost((OS_Q		*)&Msg_Q, 
				(void		*)"AppTaskLed1 Msg Queue", 
				(OS_MSG_SIZE )sizeof("AppTaskLed1 Msg Queuue"), 
				(OS_OPT		 )OS_OPT_POST_ALL | OS_OPT_POST_LIFO, 
				(OS_ERR		*)&err);
				
		value = OSTaskRegGet(0, 0, &err);
				
		if(value < 10)
		{
			OSTaskRegSet(0, 0, ++value, &err);			//继续累加任务寄存器值
		}
		else
		{
			OSTaskRegSet(0, 0, 0, &err);				//将任务寄存器值归0
			OSSemPost(&SemOfLed, OS_OPT_POST_ALL, &err);
		}

		OSTimeDly(500, OS_OPT_TIME_DLY, &err);
	}
}

static	void AppTaskLed2(void *p_arg)
{
	OS_ERR      err;
	
	(void)p_arg;
	
	while(DEF_TRUE)
	{
		bspLedToggle(GPIOD, GPIO_Pin_14);
		
		OSQPost((OS_Q		*)&Msg_Q, 										//消息变量指针
		(void		*)"AppTaskLed2 Msg Queue", 							//要发送的数据的指针，将内存块首地址通过队列“发送出去”
				(OS_MSG_SIZE )sizeof("AppTaskLed2 Msg Queuue"), 			//数据字节大小
				(OS_OPT		 )OS_OPT_POST_ALL | OS_OPT_POST_LIFO, 			//后进先出和发布全部任务的形式
				(OS_ERR		*)&err);		
		
		OSTimeDly(500, OS_OPT_TIME_DLY, &err);
	}
}

static	void AppTaskLed3(void *p_arg)
{
	OS_ERR      err;
	CPU_TS		ts_sem_post;
	CPU_TS		ts_sem_get;
	CPU_INT32U	cpu_clk_freq;

	CPU_SR_ALLOC();			//使用临界段（在关/开中断）时，必须声明该宏
							//该宏声明和定义一个局部变量，用于保存关中断前的 CPU 状态寄存器 SR(临界段关中断只需保存SR)，开中断时将该值还原。

	
	(void)p_arg;
	
	cpu_clk_freq = BSP_CPU_ClkFreq();		//获取CPU时钟，时间戳是以该时钟计数
	
	while(DEF_TRUE)
	{				
		OSSemPend ((OS_SEM   *)&SemOfLed,
				   (OS_TICK   )0,
				   (OS_OPT    )OS_OPT_PEND_BLOCKING,
				   (CPU_TS   *)&ts_sem_post,
				   (OS_ERR   *)&err);


		ts_sem_get = OS_TS_GET();
				
		bspLedToggle(GPIOD, GPIO_Pin_15);
		
		OS_CRITICAL_ENTER();                          //进入临界段，不希望下面串口打印遭到中断
		
		printf ( "\r\n发布信号量的时间戳是%d", ts_sem_post );
		printf ( "\r\n解除等待状态的时间戳是%d", ts_sem_get );
		printf ( "\r\n接收到信号量与发布信号量的时间相差%dus\r\n", 
				( ts_sem_get - ts_sem_post ) / ( cpu_clk_freq / 1000000 ) );
		
		OS_CRITICAL_EXIT(); 	
		
		OSTimeDly(500, OS_OPT_TIME_DLY, &err);
	}
}

static	void AppTaskUsart2(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	CPU_SR_ALLOC();			//使用临界段（在关/开中断）时，必须声明该宏
							//该宏声明和定义一个局部变量，用于保存关中断前的 CPU 状态寄存器 SR(临界段关中断只需保存SR)，开中断时将该值还原。
	
	char *pMsg = NULL;		//接受消息
	
	(void)p_arg;

	while(DEF_TRUE)
	{
		pMsg = OSQPend (	(OS_Q         *)&Msg_Q,							//消息变量指针
							(OS_TICK       )0,								//等待时长为无限
							(OS_OPT        )OS_OPT_PEND_BLOCKING,			//如果没有获取到信号量就等待
							(OS_MSG_SIZE  *)&msg_size,						//获取消息的字节大小
							(CPU_TS       *)0,								//获取任务发送时的时间戳
							(OS_ERR       *)&err);
		if(err == OS_ERR_NONE)
		{
			OS_CRITICAL_ENTER();			
			printf("\r\n接收消息的长度：%d字节，内容：%s\r\n", msg_size, pMsg);
			OS_CRITICAL_EXIT();
		}
	}
}

