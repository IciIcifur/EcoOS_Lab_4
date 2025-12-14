/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoTaskScheduler1Lab_C761620F
 * </сводка>
 *
 * <описание>
 *   Данный исходный код описывает реализацию интерфейсов CEcoTaskScheduler1Lab_C761620F
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoTaskScheduler1Lab.h"
#include "CEcoTask1Lab.h"
#include "IEcoBase1.h"

/* ===================== FCFS минимальная реализация без вытеснения ===================== */
/* Реализуем простую внутреннюю очередь FIFO, в которую NewTask добавляет задачи
   (указатели на функции), а Start последовательно выполняет их в порядке добавления
   и завершает работу, когда очередь опустеет. Это соответствует FCFS без вытеснения. */


typedef void (*EcoTaskFunc)(void);
#define FCFS_QUEUE_MAX 64

typedef struct {
    EcoTaskFunc fn;
    void*       data;
    uint32_t    stackSize;
    IEcoTask1*  pITask;
} FCFS_TaskItem;

typedef struct {
    FCFS_TaskItem items[FCFS_QUEUE_MAX];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
} FCFS_Queue;

static void FCFS_Init(FCFS_Queue* q) {
    q->head = q->tail = q->count = 0;
}
static int FCFS_Push(FCFS_Queue* q, EcoTaskFunc fn, void* data, uint32_t stackSize, IEcoTask1* pITask) {
    if (q->count >= FCFS_QUEUE_MAX) {
        return -1; /* overflow */
    }
    q->items[q->tail].fn = fn;
    q->items[q->tail].data = data;
    q->items[q->tail].stackSize = stackSize;
    q->items[q->tail].pITask = pITask;
    q->tail = (q->tail + 1) % FCFS_QUEUE_MAX;
    q->count++;
    return 0;
}
static int FCFS_Pop(FCFS_Queue* q, FCFS_TaskItem* out) {
    if (q->count == 0) {
        return -1; /* empty */
    }
    *out = q->items[q->head];
    q->head = (q->head + 1) % FCFS_QUEUE_MAX;
    q->count--;
    return 0;
}


/* Выделяем память под один экземпляр */
CEcoTaskScheduler1Lab_C761620F g_xCEcoTaskScheduler1Lab_C761620F = {0};

/* Резервируем область под статические задачи */
#define MAX_STATIC_TASK_COUNT   3
CEcoTask1Lab_C761620F g_xCEcoTask1List_C761620F[MAX_STATIC_TASK_COUNT] = {0};

/* Резервируем область под стеки статических задач */
#define MAX_STATIC_STACK_TASK_COUNT   4096 * MAX_STATIC_TASK_COUNT
uint64_t g_xCEcoStackTask1List_C761620F[MAX_STATIC_STACK_TASK_COUNT] = {0};

/* Контекст */
uint64_t * volatile g_pxCurrentTCB_C761620F = 0;

uint64_t g_indx = 0;

typedef struct CEcoTaskScheduler1Lab_State {
    FCFS_Queue queue;
    int initialized;
} CEcoTaskScheduler1Lab_State;

static CEcoTaskScheduler1Lab_State g_state = {0};

/*
 *
 * <сводка>
 *   Функция TaskSwitchContext
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */

/*__attribute__((naked))*/ void CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext( void ) {

        //if (g_pxCurrentTCB_C761620F == (uint64_t*)&g_xCEcoTask1List_C761620F[g_indx] && g_xCEcoTask1List_C761620F[g_indx].pfunc != 0) {
            g_indx++;
            if (g_indx > 1) {
                g_indx = 0;
            }

          //  g_pxCurrentTCB_C761620F = (uint64_t*)&g_xCEcoTask1List_C761620F[g_indx];
        //}

   // g_xCEcoTaskScheduler1Lab_C761620F.m_pIArmTimer->pVTbl->Reset(g_xCEcoTaskScheduler1Lab_C761620F.m_pIArmTimer);
    g_xCEcoTask1List_C761620F[g_indx].pfunc();
}

/*
 *
 * <сводка>
 *   Функция TimerHandler
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
void CEcoTaskScheduler1Lab_C761620F_TimerHandler(void) {

    /* Сохранение контекста текущей задачи */
    //__asm volatile (
    //"STP 	X0, X1, [SP, #-0x10]! \n"
    //"STP 	X2, X3, [SP, #-0x10]! \n"
    //"STP 	X4, X5, [SP, #-0x10]! \n"
    //"STP 	X6, X7, [SP, #-0x10]! \n"
    //"STP 	X8, X9, [SP, #-0x10]! \n"
    //"STP 	X10, X11, [SP, #-0x10]!\n"
    //"STP 	X12, X13, [SP, #-0x10]!\n"
    //"STP 	X14, X15, [SP, #-0x10]!\n"
    //"STP 	X16, X17, [SP, #-0x10]!\n"
    //"STP 	X18, X19, [SP, #-0x10]!\n"
    //"STP 	X20, X21, [SP, #-0x10]!\n"
    //"STP 	X22, X23, [SP, #-0x10]!\n"
    //"STP 	X24, X25, [SP, #-0x10]!\n"
    //"STP 	X26, X27, [SP, #-0x10]!\n"
    //"STP 	X28, X29, [SP, #-0x10]!\n"
    //"STP 	X30, XZR, [SP, #-0x10]!\n"
    //"MRS		X3, SPSR_EL1\n"
    //"MRS		X2, ELR_EL1\n"
    //"STP 	X2, X3, [SP, #-0x10]!\n"
    //"LDR 	X0, =g_pxCurrentTCB_C761620F \n"
    //"LDR 	X1, [X0] \n"
    //"MOV 	X0, SP \n"
    //"STR 	X0, [X1] \n"
    //);
    /* Переключение контекста задач */
    #if defined(ECO_AARCH64)
        __asm volatile ("BL  CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext \n");
    #else
        CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext();
    #endif

    /* Востановление контекста следующей задачи */
    //__asm volatile ( "LDR		X0, =g_pxCurrentTCB_C761620F \n"
    //"LDR		X1, [X0] \n"
    //"LDR		X0, [X1] \n"
    //"MOV		SP, X0 \n"
    //"LDP 	X2, X3, [SP], #0x10 \n"
    //"MSR		SPSR_EL1, X3 \n"
    //"MSR		ELR_EL1, X2 \n"
    //"LDP 	X30, XZR, [SP], #0x10 \n"
    //"LDP 	X28, X29, [SP], #0x10 \n"
    //"LDP 	X26, X27, [SP], #0x10 \n"
    //"LDP 	X24, X25, [SP], #0x10 \n"
    //"LDP 	X22, X23, [SP], #0x10 \n"
    //"LDP 	X20, X21, [SP], #0x10 \n"
    //"LDP 	X18, X19, [SP], #0x10 \n"
    //"LDP 	X16, X17, [SP], #0x10 \n"
    //"LDP 	X14, X15, [SP], #0x10 \n"
    //"LDP 	X12, X13, [SP], #0x10 \n"
    //"LDP 	X10, X11, [SP], #0x10 \n"
    //"LDP 	X8, X9, [SP], #0x10 \n"
    //"LDP 	X6, X7, [SP], #0x10 \n"
    //"LDP 	X4, X5, [SP], #0x10 \n"
    //"LDP 	X2, X3, [SP], #0x10 \n"
    //"LDP 	X0, X1, [SP], #0x10 \n"
    //"ERET \n"
    //);

}


/*
 *
 * <сводка>
 *   Функция QueryInterface
 * </сводка>
 *
 * <описание>
 *   Функция QueryInterface для интерфейса IEcoTaskScheduler1Lab
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_QueryInterface(/* in */ IEcoTaskScheduler1Ptr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoTaskScheduler1) ) {
        *ppv = &pCMe->m_pVTblIScheduler;
        pCMe->m_pVTblIScheduler->AddRef((IEcoTaskScheduler1*)pCMe);
    }
    else if ( IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIScheduler;
        pCMe->m_pVTblIScheduler->AddRef((IEcoTaskScheduler1*)pCMe);
    }
    else {
        *ppv = 0;
        return ERR_ECO_NOINTERFACE;
    }
    return ERR_ECO_SUCCESES;
}

/*
 *
 * <сводка>
 *   Функция AddRef
 * </сводка>
 *
 * <описание>
 *   Функция AddRef для интерфейса IEcoTaskScheduler1Lab
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_AddRef(/* in */ IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return (uint32_t)-1;
    }

    return atomicincrement_int32_t(&pCMe->m_cRef);
}

/*
 *
 * <сводка>
 *   Функция Release
 * </сводка>
 *
 * <описание>
 *   Функция Release для интерфейса IEcoTaskScheduler1Lab
 * </описание>
 *
 */
static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Release(/* in */ IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателя */
    if (me == 0 ) {
        return (uint32_t)-1;
    }

    /* Уменьшение счетчика ссылок на компонент */
    atomicdecrement_int32_t(&pCMe->m_cRef);

    /* В случае обнуления счетчика, освобождение данных экземпляра */
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoTaskScheduler1Lab_C761620F((IEcoTaskScheduler1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Init(/*in*/IEcoTaskScheduler1Ptr_t me, /*in*/ IEcoInterfaceBus1Ptr_t pIBus) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    /* Для Windows юнит-теста просто инициализируем очередь */
    if (me == 0) {
        return -1;
    }

    FCFS_Init(&g_state.queue);
    g_state.initialized = 1;

    (void)pIBus;

    return 0;
}

/*
 *
 * <сводка>
 *   Функция InitWith
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_InitWith(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ IEcoInterfaceBus1Ptr_t pIBus, /*in*/ voidptr_t heapStartAddress, /*in*/ uint32_t size) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    int16_t result = -1;

    /* Проверка указателей */
    if (me == 0 || pIBus == 0) {
        return -1;
    }

    /* Инициализация данных */
    pCMe->m_pTaskList = g_xCEcoTask1List_C761620F;
    pCMe->m_pIBus = pIBus;

    /* Инициализация FCFS-очереди */
    FCFS_Init(&g_state.queue);
    g_state.initialized = 1;

    /* Получение интерфейса для работы с программируемым таймером */
    result = pCMe->m_pIBus->pVTbl->QueryComponent(pCMe->m_pIBus, &CID_EcoTimer1, 0, &IID_IEcoTimer1, (void**) &pCMe->m_pIArmTimer);
    /* Проверка */
    if (result != 0 || pCMe->m_pIArmTimer == 0) {
        return result;
    }

    /* Установка обработчика прерывания программируемого таймера */
    pCMe->m_pIArmTimer->pVTbl->set_Interval(pCMe->m_pIArmTimer, 1000000);
    pCMe->m_pIArmTimer->pVTbl->set_IrqHandler(pCMe->m_pIArmTimer, CEcoTaskScheduler1Lab_C761620F_TimerHandler);

    return 0;
}

/*
 *
 * <сводка>
 *   Функция NewTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_NewTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ voidptr_t address, /*in*/ voidptr_t data, /*in*/ uint32_t stackSize, /* out */ IEcoTask1** ppITask) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/
    int32_t indx = 0;
    int32_t reg = 30;
    uint64_t* pxTopOfStack = 0;

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }
    if (g_state.initialized == 0) {
        return -1;
    }

    /* FCFS: добавляем задачу в внутреннюю очередь */
    {
        EcoTaskFunc fn = (EcoTaskFunc)address;
        IEcoTask1* pITask = 0;
        if (fn == NULL) {
            return -1;
        }
        /* Возвращаем NULL как IEcoTask1 */
        if (ppITask != 0) {
            *ppITask = NULL;
        }
        if (FCFS_Push(&g_state.queue, fn, (void*)data, stackSize, pITask) != 0) {
            return -1;
        }
        return 0;
    }

    /* ===== (не используем в Windows FCFS) ===== */
    /* Проверяем указатель пула статических задач */
    for (indx = 0; indx < 3; indx++) {
        if (g_xCEcoTask1List_C761620F[indx].pfunc == 0) {
            g_xCEcoTask1List_C761620F[indx].pfunc = address;
            g_xCEcoTask1List_C761620F[indx].m_cRef = 1;
            g_xCEcoTask1List_C761620F[indx].m_sp = (byte_t*)&g_xCEcoStackTask1List_C761620F[indx*4096];
            pxTopOfStack = (uint64_t*)g_xCEcoTask1List_C761620F[indx].m_sp;
            while (reg > 0) {
                pxTopOfStack--;
                reg--;
            }
            *pxTopOfStack = (uint64_t)g_xCEcoTask1List_C761620F[indx].pfunc;
            if (ppITask) {
                *ppITask = (IEcoTask1*)&g_xCEcoTask1List_C761620F[indx];
            }
            return 0;
            break;
        }
    }
    return -1;
}

/*
 *
 * <сводка>
 *   Функция DeleteTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_DeleteTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t taskId) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция SuspendTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_SuspendTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t taskId) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция ResumeTask
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_ResumeTask(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t taskId) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция RegisterInterrupt
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t number, /*in*/ voidptr_t handlerAddress, /*in*/ int32_t flag) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция UnRegisterInterrupt
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_UnRegisterInterrupt(/*in*/ IEcoTaskScheduler1Ptr_t me, /*in*/ uint16_t number) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Start
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Start(/*in*/ IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    /* ===== FCFS без вытеснения =====
       Последовательно извлекаем задачи из очереди и вызываем их.
       По завершении очереди выходим из Start (возвращаем 0). */
    if (g_state.initialized) {
        FCFS_TaskItem item;
        while (FCFS_Pop(&g_state.queue, &item) == 0) {
            if (item.fn) {
                item.fn();
            }
        }
        return 0;
    }

    /* Запускаем таймер */
    //pCMe->m_pIArmTimer->pVTbl->Start(pCMe->m_pIArmTimer);
    g_pxCurrentTCB_C761620F = (uint64_t*)&pCMe->m_pTaskList[0];

    /* Передаем управление задаче */
    while (1) {
        if (g_pxCurrentTCB_C761620F == (uint64_t*)&pCMe->m_pTaskList[g_indx] && pCMe->m_pTaskList[g_indx].pfunc != 0) {
            pCMe->m_pTaskList[g_indx].pfunc();
            g_indx++;
            if (g_indx >= MAX_STATIC_TASK_COUNT) {
                g_indx = 0;
            }
            else if (pCMe->m_pTaskList[g_indx].pfunc == 0) {
                g_indx = 0;
            }
            g_pxCurrentTCB_C761620F = (uint64_t*)&pCMe->m_pTaskList[g_indx];
        }
        else {
            asm volatile ("NOP\n\t" ::: "memory");
        }
    }
    return 0;
}

/*
 *
 * <сводка>
 *   Функция Stop
 * </сводка>
 *
 * <описание>
 *   Функция
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Stop(/*in*/ struct IEcoTaskScheduler1* me) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    return 0;
}

/*
 *
 * <сводка>
 *   Функция Init
 * </сводка>
 *
 * <описание>
 *   Функция инициализации экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD initCEcoTaskScheduler1Lab_C761620F(/*in*/ IEcoTaskScheduler1Ptr_t me, /* in */ struct IEcoUnknown *pIUnkSystem) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;*/

    /* Проверка указателей */
    if (me == 0 ) {
        return -1;
    }

    /* Инициализируем FCFS очередь */
    FCFS_Init(&g_state.queue);
    g_state.initialized = 1;

    (void)pIUnkSystem;
    return 0;
}

/* Create Virtual Table IEcoTaskScheduler1Lab */
IEcoTaskScheduler1VTbl g_x155C975395654F85B9AA27D5F377A79EVTbl_C761620F = {
    CEcoTaskScheduler1Lab_C761620F_QueryInterface,
    CEcoTaskScheduler1Lab_C761620F_AddRef,
    CEcoTaskScheduler1Lab_C761620F_Release,
    CEcoTaskScheduler1Lab_C761620F_Init,
    CEcoTaskScheduler1Lab_C761620F_InitWith,
    CEcoTaskScheduler1Lab_C761620F_NewTask,
    CEcoTaskScheduler1Lab_C761620F_DeleteTask,
    CEcoTaskScheduler1Lab_C761620F_SuspendTask,
    CEcoTaskScheduler1Lab_C761620F_ResumeTask,
    CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt,
    CEcoTaskScheduler1Lab_C761620F_UnRegisterInterrupt,
    CEcoTaskScheduler1Lab_C761620F_Start,
    CEcoTaskScheduler1Lab_C761620F_Stop
};

/*
 *
 * <сводка>
 *   Функция Create
 * </сводка>
 *
 * <описание>
 *   Функция создания экземпляра
 * </описание>
 *
 */
int16_t ECOCALLMETHOD createCEcoTaskScheduler1Lab_C761620F(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoUnknown* pIUnkOuter, /* out */ IEcoTaskScheduler1Ptr_t* ppITaskScheduler) {
    int16_t result = -1;
    CEcoTaskScheduler1Lab_C761620F* pCMe = 0;

    /* Проверка указателей */
    if (ppITaskScheduler == 0) {
        return result;
    }

    /* Получение указателя на структуру компонента расположенной в области инициализированных данных */
    pCMe = &g_xCEcoTaskScheduler1Lab_C761620F;

    if (pCMe->m_cRef == 0) {
        /* Установка счетчика ссылок на компонент */
        pCMe->m_cRef = 1;

        /* Создание таблицы функций интерфейса IEcoTaskScheduler1 */
        pCMe->m_pVTblIScheduler = &g_x155C975395654F85B9AA27D5F377A79EVTbl_C761620F;

        /* Инициализация очереди FCFS */
        FCFS_Init(&g_state.queue);
        g_state.initialized = 1;

        result = 0;
    }

    /* Возврат указателя на интерфейс */
    *ppITaskScheduler = (IEcoTaskScheduler1*)pCMe;

    (void)pIUnkSystem;
    (void)pIUnkOuter;
    return result;
}

/*
 *
 * <сводка>
 *   Функция Delete
 * </сводка>
 *
 * <описание>
 *   Функция освобождения экземпляра
 * </описание>
 *
 */
void ECOCALLMETHOD deleteCEcoTaskScheduler1Lab_C761620F(/* in */ IEcoTaskScheduler1Ptr_t pITaskScheduler) {
    /*CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)pITaskScheduler;*/

}