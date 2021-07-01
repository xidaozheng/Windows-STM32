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
static 	OS_Q	 Msg_Q;						//������Ϣ����
static 	OS_SEM	 SemOfLed;

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/
static  OS_TCB   AppTaskStartTCB;			//������ƿ�
static	OS_TCB	 AppTaskLed1TCB;
static	OS_TCB	 AppTaskLed2TCB;
static	OS_TCB	 AppTaskLed3TCB;
static	OS_TCB	 AppTaskUsart2TCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];			//�����ջ
static  CPU_STK  AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static  CPU_STK  AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static  CPU_STK  AppTaskLed3Stk[APP_TASK_LED3_STK_SIZE];
static  CPU_STK  AppTaskUsart2Stk[APP_TASK_USART2_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void AppTaskStart  (void *p_arg);				//����������
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


    OSInit(&err);    											//��ʼ��uC/OS-III                                          
                                            

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Start",				//��������
                 (OS_TASK_PTR ) AppTaskStart,					//������
                 (void       *) 0,								//���ݸ�������(�β�p_arg)��ʵ��
                 (OS_PRIO     ) APP_TASK_START_PRIO,			//��������ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],				//�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,	//��ַ��ˮӡ�� ����ջ������90%λ��ʱ�Ͳ�������������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,		//����Ķ�ջ��С(��λ��unsigned  int) 4���ֽ�
                 (OS_MSG_QTY  ) 5u, 							//����ɽ��ܵ������Ϣ��
                 (OS_TICK     ) 0u,								//�����ʱ��Ƭ������(0����Ĭ��ֵOSCfg_TickRate_Hz/10)
                 (void       *) 0,								//������չ(0������չ)
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);								

    if(err != OS_ERR_NONE)
	{
		/* The task didn't get created. Lookup the value of the error code ... 	*/
		/* ... in OS.H for the meaning of the error								*/
	}

	//ȫ��ֻ��Ҫһ������
	OSStart(&err);                                             	//�����������������uC/OS-III���ƣ�
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
    BSP_Init();                                                 //�弶��ʼ��
    CPU_Init();													//��ʼ�� CPU ��� ��ʱ��������ж�ʱ���������������
    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��
	
    Mem_Init();  												//��ʼ�� �ڴ������������ڴ�غ��ڴ�ر�
                                               
#if OS_CFG_STAT_TASK_EN > 0u									//���ʹ����ͳ������
    OSStatTaskCPUUsageInit(&err);                               //����û��Ӧ������(ֻ�п�������)����ʱ CPU �ģ����
#endif															//���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU ...
																//... ʹ����ʹ�ã���
																
    CPU_IntDisMeasMaxCurReset();								//��λ�����㣩��ǰ�����ж�ʱ��

	//����������������ȼ���ͬ��������Ҫ����ʱ��Ƭ������ת����
	OSSchedRoundRobinCfg ((CPU_BOOLEAN   )DEF_ENABLED,			//ʹ��ʱ��Ƭ��ת����
                          (OS_TICK       )0,					//��OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
                          (OS_ERR       *)&err
						 );

	//������Ϣ���� queue
	OSQCreate(	(OS_Q 			*)&Msg_Q, 				//ָ����Ϣ���е�ָ��
				(CPU_CHAR		*)"Queure For Test",	//���е����� 
				(OS_MSG_QTY		 )20, 					//���ɴ����Ϣ����Ŀ
				(OS_ERR			*)&err);
	if(err != OS_ERR_NONE)
	{
		printf("OSQCreate #err=%d\n", err);
	}
	
	//������ֵ�ź���
	OSSemCreate (	(OS_SEM      *)&SemOfLed,
					(CPU_CHAR    *)"SemOfLed",
					(OS_SEM_CTR   )0,			//�ź���������ָʾ�¼����������Ը�ֵ0����ʾʱ�仹û�з���
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


	OSTaskDel(&AppTaskStartTCB, &err);	//ɾ��������
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
			OSTaskRegSet(0, 0, ++value, &err);			//�����ۼ�����Ĵ���ֵ
		}
		else
		{
			OSTaskRegSet(0, 0, 0, &err);				//������Ĵ���ֵ��0
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
		
		OSQPost((OS_Q		*)&Msg_Q, 										//��Ϣ����ָ��
		(void		*)"AppTaskLed2 Msg Queue", 							//Ҫ���͵����ݵ�ָ�룬���ڴ���׵�ַͨ�����С����ͳ�ȥ��
				(OS_MSG_SIZE )sizeof("AppTaskLed2 Msg Queuue"), 			//�����ֽڴ�С
				(OS_OPT		 )OS_OPT_POST_ALL | OS_OPT_POST_LIFO, 			//����ȳ��ͷ���ȫ���������ʽ
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

	CPU_SR_ALLOC();			//ʹ���ٽ�Σ��ڹ�/���жϣ�ʱ�����������ú�
							//�ú������Ͷ���һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR(�ٽ�ι��ж�ֻ�豣��SR)�����ж�ʱ����ֵ��ԭ��

	
	(void)p_arg;
	
	cpu_clk_freq = BSP_CPU_ClkFreq();		//��ȡCPUʱ�ӣ�ʱ������Ը�ʱ�Ӽ���
	
	while(DEF_TRUE)
	{				
		OSSemPend ((OS_SEM   *)&SemOfLed,
				   (OS_TICK   )0,
				   (OS_OPT    )OS_OPT_PEND_BLOCKING,
				   (CPU_TS   *)&ts_sem_post,
				   (OS_ERR   *)&err);


		ts_sem_get = OS_TS_GET();
				
		bspLedToggle(GPIOD, GPIO_Pin_15);
		
		OS_CRITICAL_ENTER();                          //�����ٽ�Σ���ϣ�����洮�ڴ�ӡ�⵽�ж�
		
		printf ( "\r\n�����ź�����ʱ�����%d", ts_sem_post );
		printf ( "\r\n����ȴ�״̬��ʱ�����%d", ts_sem_get );
		printf ( "\r\n���յ��ź����뷢���ź�����ʱ�����%dus\r\n", 
				( ts_sem_get - ts_sem_post ) / ( cpu_clk_freq / 1000000 ) );
		
		OS_CRITICAL_EXIT(); 	
		
		OSTimeDly(500, OS_OPT_TIME_DLY, &err);
	}
}

static	void AppTaskUsart2(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE msg_size;
	CPU_SR_ALLOC();			//ʹ���ٽ�Σ��ڹ�/���жϣ�ʱ�����������ú�
							//�ú������Ͷ���һ���ֲ����������ڱ�����ж�ǰ�� CPU ״̬�Ĵ��� SR(�ٽ�ι��ж�ֻ�豣��SR)�����ж�ʱ����ֵ��ԭ��
	
	char *pMsg = NULL;		//������Ϣ
	
	(void)p_arg;

	while(DEF_TRUE)
	{
		pMsg = OSQPend (	(OS_Q         *)&Msg_Q,							//��Ϣ����ָ��
							(OS_TICK       )0,								//�ȴ�ʱ��Ϊ����
							(OS_OPT        )OS_OPT_PEND_BLOCKING,			//���û�л�ȡ���ź����͵ȴ�
							(OS_MSG_SIZE  *)&msg_size,						//��ȡ��Ϣ���ֽڴ�С
							(CPU_TS       *)0,								//��ȡ������ʱ��ʱ���
							(OS_ERR       *)&err);
		if(err == OS_ERR_NONE)
		{
			OS_CRITICAL_ENTER();			
			printf("\r\n������Ϣ�ĳ��ȣ�%d�ֽڣ����ݣ�%s\r\n", msg_size, pMsg);
			OS_CRITICAL_EXIT();
		}
	}
}

