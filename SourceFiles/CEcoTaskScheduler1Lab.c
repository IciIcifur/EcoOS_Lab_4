/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoTaskScheduler1Lab_C761620F - FCFS без вытеснения
 * </сводка>
 *
 * <описание>
 *   Реализация планировщика задач: задачи исполняются в порядке поступления,
 *   без контекстных переключений по таймеру.
 * </описание>
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoTaskScheduler1Lab.h"
#include "CEcoTask1Lab.h"

/* Максимум задач */
#define MAX_STATIC_TASK_COUNT 3

/* Экземпляр планировщика */
CEcoTaskScheduler1Lab_C761620F g_xCEcoTaskScheduler1Lab_C761620F = {0};

/* Пул задач */
CEcoTask1Lab_C761620F_ext g_xCEcoTask1List_C761620F[MAX_STATIC_TASK_COUNT] = {0};

/* Пул стеков */
#define MAX_STATIC_STACK_TASK_COUNT   (4096 * MAX_STATIC_TASK_COUNT)
uint64_t g_xCEcoStackTask1List_C761620F[MAX_STATIC_STACK_TASK_COUNT] = {0};

/* ================= базовые методы ================= */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_QueryInterface(IEcoTaskScheduler1Ptr_t me, const UGUID* riid, void** ppv) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    /* Проверка указателей */
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }

    /* Проверка и получение запрошенного интерфейса */
    if ( IsEqualUGUID(riid, &IID_IEcoTaskScheduler1) ) {
        *ppv = &pCMe->m_pVTblIScheduler;
        pCMe->m_pVTblIScheduler->AddRef((IEcoTaskScheduler1*)pCMe);
        return ERR_ECO_SUCCESES;
    }
    *ppv = 0;
    return ERR_ECO_NOINTERFACE;
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
    if (pCMe->m_cRef == 0) {
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
    if (me == 0 || pIBus == 0) {
        return -1;
    }

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
    int16_t result;

    /* Проверка указателей */
    if (me == 0 || pIBus == 0) {
        return -1;
    }

    /* Инициализация данных планировщика */
    pCMe->m_pTaskList = g_xCEcoTask1List_C761620F;
    pCMe->m_pIBus = pIBus;
    pCMe->m_TaskCount = 0;
    pCMe->m_CurrentIndex = 0;
    pCMe->m_TickUs = 1000; /* 1 ms — базовый тик по умолчанию */

    result = pCMe->m_pIBus->pVTbl->QueryComponent(pCMe->m_pIBus, &CID_EcoTimer1, 0, &IID_IEcoTimer1, (void**)&pCMe->m_pIArmTimer);
    if (result == 0 && pCMe->m_pIArmTimer != 0) {
        pCMe->m_pIArmTimer->pVTbl->set_Interval(pCMe->m_pIArmTimer, 1000000);

        pCMe->m_pIArmTimer->pVTbl->set_IrqHandler(pCMe->m_pIArmTimer, CEcoTaskScheduler1Lab_C761620F_TimerHandler);
        pCMe->m_pIArmTimer->pVTbl->Start(pCMe->m_pIArmTimer);

    }

    return ERR_ECO_SUCCESES;
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
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_NewTask(IEcoTaskScheduler1Ptr_t me, voidptr_t address, voidptr_t data, uint32_t stackSize, IEcoTask1** ppITask) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    int32_t i;

    if (me == 0 || ppITask == 0 || address == 0) {
        return -1;
    }

    // Найдём первую свободную ячейку
    for (i = 0; i < MAX_STATIC_TASK_COUNT; i++) {
        if (g_xCEcoTask1List_C761620F[i].base.pfunc == 0) {
            g_xCEcoTask1List_C761620F[i].base.pfunc = address;
            g_xCEcoTask1List_C761620F[i].base.m_cRef = 1;

            g_xCEcoTask1List_C761620F[i].base.m_sp = (byte_t*)&g_xCEcoStackTask1List_C761620F[i * 4096];
            g_xCEcoTask1List_C761620F[i].state = ECO_TASK_STATE_RUNNABLE;
            g_xCEcoTask1List_C761620F[i].delayTicks = 0;
            g_xCEcoTask1List_C761620F[i].saved_sp = (uint64_t*)g_xCEcoTask1List_C761620F[i].base.m_sp;

            *ppITask = (IEcoTask1*)&g_xCEcoTask1List_C761620F[i].base;

            if ((uint32_t)(i + 1) > pCMe->m_TaskCount) {
                pCMe->m_TaskCount = (uint32_t)(i + 1);
            }
            return 0;
        }
    }
    return -1;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_DeleteTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) {
    return 0;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_SuspendTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) {
    return 0;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_ResumeTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) {
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
    return 0;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt(IEcoTaskScheduler1Ptr_t me, uint16_t number, voidptr_t handlerAddress, int32_t flag) {
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
    uint32_t idx = 0;

    if (me == 0) {
        return -1;
    }

    // Запускаем каждую задачу по порядку
    while (1) {
        for (idx = 0; idx < pCMe->m_TaskCount; idx++) {
            if (pCMe->m_pTaskList[idx].base.pfunc != 0 &&
                pCMe->m_pTaskList[idx].state == ECO_TASK_STATE_RUNNABLE) {
                pCMe->m_pTaskList[idx].base.pfunc();
            }

            else if (pCMe->m_pTaskList[idx].state == ECO_TASK_STATE_DELAYED) {
                if (pCMe->m_pTaskList[idx].delayTicks > 0) {
                    pCMe->m_pTaskList[idx].delayTicks--;
                    if (pCMe->m_pTaskList[idx].delayTicks == 0) {
                        pCMe->m_pTaskList[idx].state = ECO_TASK_STATE_RUNNABLE;
                    }
                }
            }
        }

        asm volatile ("NOP\n\t" ::: "memory");
    }
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
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Stop(IEcoTaskScheduler1Ptr_t me) {
    if (me == 0) {
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
int16_t ECOCALLMETHOD initCEcoTaskScheduler1Lab_C761620F(IEcoTaskScheduler1Ptr_t me, IEcoUnknown *pIUnkSystem) {
    if (me == 0) {
        return -1;
    }
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

        result = 0;
    }

    /* Возврат указателя на интерфейс */
    *ppITaskScheduler = (IEcoTaskScheduler1*)pCMe;

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

void CEcoTaskScheduler1Lab_C761620F_TimerHandler(void) {

}