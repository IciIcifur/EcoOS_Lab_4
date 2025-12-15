/*
 * <кодировка символов>
 *   Cyrillic ( UTF-8 with signature ) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoTaskScheduler1Lab_C761620F - FCFS без вытеснения (run-to-completion)
 * </сводка>
 *
 * <описание>
 *   Задачи исполняются по порядку регистрации (First-Come, First-Served)
 *   без вытеснения. Каждая задача вызывается ровно один раз (run-to-completion),
 *   после чего помечается завершённой (pfunc=0) и больше не вызывается.
 *   По завершении всех задач планировщик уходит в idle-петлю.
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
#include "IdEcoTimer1.h"
#include <stdint.h>

/* Максимум задач */
#define MAX_STATIC_TASK_COUNT 3

/* Экземпляр планировщика (в инициализированных данных) */
CEcoTaskScheduler1Lab_C761620F g_xCEcoTaskScheduler1Lab_C761620F = {0};

/* Пул задач (базовый TCB из CEcoTask1Lab.h) */
static CEcoTask1Lab_C761620F g_xCEcoTask1List_C761620F[MAX_STATIC_TASK_COUNT] = {0};

/* Пул стеков: по 4096 uint64_t (32KB) на задачу */
#define MAX_STATIC_STACK_TASK_COUNT   (4096 * MAX_STATIC_TASK_COUNT)
static uint64_t g_xCEcoStackTask1List_C761620F[MAX_STATIC_STACK_TASK_COUNT] = {0};

/* Локальное состояние планировщика (без изменения заголовка): число зарегистрированных задач */
static uint32_t g_TaskCount = 0;
static volatile uint32_t g_NewTaskPending = 0;

/* ================= базовые методы ================= */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_QueryInterface(IEcoTaskScheduler1Ptr_t me, const UGUID* riid, void** ppv) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0 || ppv == 0) {
        return ERR_ECO_POINTER;
    }
    if ( IsEqualUGUID(riid, &IID_IEcoTaskScheduler1) ) {
        *ppv = &pCMe->m_pVTblIScheduler;
        pCMe->m_pVTblIScheduler->AddRef((IEcoTaskScheduler1*)pCMe);
        return ERR_ECO_SUCCESES;
    }
    *ppv = 0;
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_AddRef(IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0 ) {
        return (uint32_t)-1;
    }
    return atomicincrement_int32_t(&pCMe->m_cRef);
}

static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Release(IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0 ) {
        return (uint32_t)-1;
    }
    atomicdecrement_int32_t(&pCMe->m_cRef);
    if (pCMe->m_cRef == 0) {
        deleteCEcoTaskScheduler1Lab_C761620F((IEcoTaskScheduler1*)pCMe);
        return 0;
    }
    return pCMe->m_cRef;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Init(IEcoTaskScheduler1Ptr_t me, IEcoInterfaceBus1Ptr_t pIBus) {
    if (me == 0 || pIBus == 0) {
        return -1;
    }
    return 0;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_InitWith(IEcoTaskScheduler1Ptr_t me, IEcoInterfaceBus1Ptr_t pIBus, voidptr_t heapStartAddress, uint32_t size) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    int16_t result;

    if (me == 0 || pIBus == 0) {
        return -1;
    }

    /* Инициализация данных планировщика */
    pCMe->m_pTaskList = g_xCEcoTask1List_C761620F;
    pCMe->m_pIBus = pIBus;
    pCMe->m_pIArmTimer = 0;
    g_TaskCount = 0;

    /* Таймер для невытесняющего режима не требуется (получим интерфейс, но не запускаем) */
    result = pCMe->m_pIBus->pVTbl->QueryComponent(pCMe->m_pIBus, &CID_EcoTimer1, 0, &IID_IEcoTimer1, (void**)&pCMe->m_pIArmTimer);
    (void)result; /* не используем */

    return 0;
}

/*
 *
 * <сводка>
 *   Функция NewTask
 * </сводка>
 *
 * <описание>
 *   Регистрирует функцию задачи и выделяет ей статический стек.
 *   Порядок индексов определяет порядок обслуживания (FCFS).
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_NewTask(IEcoTaskScheduler1Ptr_t me, voidptr_t address, voidptr_t data, uint32_t stackSize, IEcoTask1** ppITask) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;

    if (me == 0 || ppITask == 0 || address == 0) {
        return -1;
    }

    for (int32_t i = 0; i < MAX_STATIC_TASK_COUNT; i++) {
        if (g_xCEcoTask1List_C761620F[i].pfunc == 0) {
            g_xCEcoTask1List_C761620F[i].pfunc = address;
            g_xCEcoTask1List_C761620F[i].m_cRef = 1;
            g_xCEcoTask1List_C761620F[i].m_sp = (byte_t*)&g_xCEcoStackTask1List_C761620F[i * 4096];

            *ppITask = (IEcoTask1*)&g_xCEcoTask1List_C761620F[i];

            if ((uint32_t)(i + 1) > g_TaskCount) {
                g_TaskCount = (uint32_t)(i + 1);
            }
            g_NewTaskPending = 1;
            return 0;
        }
    }
    return -1;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_DeleteTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) {
    /* Для совместимости: пометить задачу завершённой */
    if (me == 0) {
        return -1;
    }
    if (taskId < g_TaskCount) {
        g_xCEcoTask1List_C761620F[taskId].pfunc = 0;
        return 0;
    }
    return -1;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_SuspendTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) { return 0; }
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_ResumeTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) { return 0; }
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt(IEcoTaskScheduler1Ptr_t me, uint16_t number, voidptr_t handlerAddress, int32_t flag) { return 0; }
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_UnRegisterInterrupt(IEcoTaskScheduler1Ptr_t me, uint16_t number) { return 0; }

/*
 *
 * <сводка>
 *   Функция Start
 * </сводка>
 *
 * <описание>
 *   FCFS run-to-completion: задача вызывается один раз и помечается завершённой.
 *   После завершения всех задач — idle-петля.
 * </описание>
 *
 */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Start(IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0) {
        return -1;
    }

    while (1) {
        uint32_t dispatched = 0;

        /* Важно: сканируем весь пул слева направо — это FCFS по индексу
           (индекс назначается по первому свободному месту при регистрации) */
        for (uint32_t idx = 0; idx < MAX_STATIC_TASK_COUNT; idx++) {
            if (pCMe->m_pTaskList[idx].pfunc != 0) {
                /* Снимем указатель, чтобы защититься от повторного вызова,
                   затем вызовем задачу run-to-completion */
                void (*task_entry)(void) = pCMe->m_pTaskList[idx].pfunc;
                pCMe->m_pTaskList[idx].pfunc = 0;
                dispatched++;
                task_entry();
            }
        }

        if (dispatched == 0) {
            /* Нет работы — «дремлем» и снова сканируем
               (можно заменить на wfi, если точно есть событие для пробуждения) */
            if (g_NewTaskPending == 0) {
                asm volatile ("NOP\n\t" ::: "memory");
            }
            g_NewTaskPending = 0;
        }
    }
}

/* Таймер не используется в невытесняющем варианте */
void CEcoTaskScheduler1Lab_C761620F_TimerHandler(void) {}

/*
 *
 * <сводка>
 *   Функция Stop
 * </сводка>
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
 *   Функция Init экземпляра
 * </сводка>
 */
int16_t ECOCALLMETHOD initCEcoTaskScheduler1Lab_C761620F(IEcoTaskScheduler1Ptr_t me, IEcoUnknown *pIUnkSystem) {
    if (me == 0) {
        return -1;
    }
    return 0;
}

/* Таблица функций IEcoTaskScheduler1 */
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
 */
int16_t ECOCALLMETHOD createCEcoTaskScheduler1Lab_C761620F(IEcoUnknown* pIUnkSystem, IEcoUnknown* pIUnkOuter, IEcoTaskScheduler1Ptr_t* ppITaskScheduler) {
    int16_t result = -1;
    CEcoTaskScheduler1Lab_C761620F* pCMe = 0;

    if (ppITaskScheduler == 0) {
        return result;
    }

    pCMe = &g_xCEcoTaskScheduler1Lab_C761620F;

    if (pCMe->m_cRef == 0) {
        pCMe->m_cRef = 1;
        pCMe->m_pVTblIScheduler = &g_x155C975395654F85B9AA27D5F377A79EVTbl_C761620F;
        result = 0;
    }

    *ppITaskScheduler = (IEcoTaskScheduler1*)pCMe;
    return result;
}

/*
 *
 * <сводка>
 *   Функция Delete
 * </сводка>
 */
void ECOCALLMETHOD deleteCEcoTaskScheduler1Lab_C761620F(IEcoTaskScheduler1Ptr_t pITaskScheduler) {
    /* Статический экземпляр — освобождать нечего */
}