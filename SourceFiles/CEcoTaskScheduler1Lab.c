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
 *   FCFS без вытеснения. Правки:
 *   - Строгая проверка pIBus в Init/InitWith (NULL -> ошибка)
 *   - m_cRef: volatile long + atomic*_int32_t((volatile long*))
 *   - Forward declarations для методов, используемых в vtbl/IRQ
 *   - C89: объявления переменных в начале блока
 * </описание>
 */

#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoInterfaceBus1MemExt.h"
#include "CEcoTaskScheduler1Lab.h"
#include "CEcoTask1Lab.h"
#include "IEcoBase1.h"
#include "depend.h"
#include "atomic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== Forward declarations ========== */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_QueryInterface(IEcoTaskScheduler1Ptr_t me, const UGUID* riid, void** ppv);
static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_AddRef(IEcoTaskScheduler1Ptr_t me);
static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Release(IEcoTaskScheduler1Ptr_t me);

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Init(IEcoTaskScheduler1Ptr_t me, IEcoInterfaceBus1Ptr_t pIBus);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_InitWith(IEcoTaskScheduler1Ptr_t me, IEcoInterfaceBus1Ptr_t pIBus, voidptr_t heapStartAddress, uint32_t size);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_NewTask(IEcoTaskScheduler1Ptr_t me, voidptr_t address, voidptr_t data, uint32_t stackSize, IEcoTask1** ppITask);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_DeleteTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_SuspendTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_ResumeTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt(IEcoTaskScheduler1Ptr_t me, uint16_t number, voidptr_t handlerAddress, int32_t flag);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_UnRegisterInterrupt(IEcoTaskScheduler1Ptr_t me, uint16_t number);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Start(IEcoTaskScheduler1Ptr_t me);
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Stop(struct IEcoTaskScheduler1* me);

void CEcoTaskScheduler1Lab_C761620F_TimerHandler(void);
void CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext(void);

/* ===== FCFS очередь ===== */
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

static void FCFS_Init(FCFS_Queue* q) { q->head = q->tail = q->count = 0; }
static int FCFS_Push(FCFS_Queue* q, EcoTaskFunc fn, void* data, uint32_t stackSize, IEcoTask1* pITask) {
    if (q->count >= FCFS_QUEUE_MAX) return -1;
    q->items[q->tail].fn = fn;
    q->items[q->tail].data = data;
    q->items[q->tail].stackSize = stackSize;
    q->items[q->tail].pITask = pITask;
    q->tail = (q->tail + 1) % FCFS_QUEUE_MAX;
    q->count++;
    return 0;
}
static int FCFS_Pop(FCFS_Queue* q, FCFS_TaskItem* out) {
    if (q->count == 0) return -1;
    *out = q->items[q->head];
    q->head = (q->head + 1) % FCFS_QUEUE_MAX;
    q->count--;
    return 0;
}

/* Выделяем память под один экземпляр */
CEcoTaskScheduler1Lab_C761620F g_xCEcoTaskScheduler1Lab_C761620F = {0};
/* Резерв statics как у тебя */
#define MAX_STATIC_TASK_COUNT   3
CEcoTask1Lab_C761620F g_xCEcoTask1List_C761620F[MAX_STATIC_TASK_COUNT] = {0};
#define MAX_STATIC_STACK_TASK_COUNT   4096 * MAX_STATIC_TASK_COUNT
uint64_t g_xCEcoStackTask1List_C761620F[MAX_STATIC_STACK_TASK_COUNT] = {0};
uint64_t * volatile g_pxCurrentTCB_C761620F = 0;
uint64_t g_indx = 0;

typedef struct CEcoTaskScheduler1Lab_State {
    FCFS_Queue queue;
    int initialized;
} CEcoTaskScheduler1Lab_State;

static CEcoTaskScheduler1Lab_State g_state = {0};

/* VTable */
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

/* ===== IEcoTaskScheduler1 ===== */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_QueryInterface(/* in */ IEcoTaskScheduler1Ptr_t me, /* in */ const UGUID* riid, /* out */ void** ppv) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0 || ppv == 0) return ERR_ECO_POINTER;
    if ( IsEqualUGUID(riid, &IID_IEcoTaskScheduler1) || IsEqualUGUID(riid, &IID_IEcoUnknown) ) {
        *ppv = &pCMe->m_pVTblIScheduler;
        pCMe->m_pVTblIScheduler->AddRef((IEcoTaskScheduler1*)pCMe);
        return ERR_ECO_SUCCESES;
    }
    *ppv = 0;
    return ERR_ECO_NOINTERFACE;
}

static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_AddRef(IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0) return (uint32_t)-1;
    return (uint32_t)atomicincrement_int32_t((volatile long*)&pCMe->m_cRef);
}

static uint32_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Release(IEcoTaskScheduler1Ptr_t me) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0) return (uint32_t)-1;
    atomicdecrement_int32_t((volatile long*)&pCMe->m_cRef);
    if ( pCMe->m_cRef == 0 ) {
        deleteCEcoTaskScheduler1Lab_C761620F((IEcoTaskScheduler1*)pCMe);
        return 0;
    }
    return (uint32_t)pCMe->m_cRef;
}

/* ВАЖНО: pIBus обязателен */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Init(IEcoTaskScheduler1Ptr_t me, IEcoInterfaceBus1Ptr_t pIBus) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    if (me == 0 || pIBus == 0) {
        return -1;
    }
    pCMe->m_pIBus = pIBus;
    FCFS_Init(&g_state.queue);
    g_state.initialized = 1;
    return 0;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_InitWith(IEcoTaskScheduler1Ptr_t me, IEcoInterfaceBus1Ptr_t pIBus, voidptr_t heapStartAddress, uint32_t size) {
    CEcoTaskScheduler1Lab_C761620F* pCMe = (CEcoTaskScheduler1Lab_C761620F*)me;
    int16_t result;
    if (me == 0 || pIBus == 0) {
        return -1;
    }
    pCMe->m_pTaskList = g_xCEcoTask1List_C761620F;
    pCMe->m_pIBus = pIBus;

    FCFS_Init(&g_state.queue);
    g_state.initialized = 1;

    result = pCMe->m_pIBus->pVTbl->QueryComponent(pCMe->m_pIBus, &CID_EcoTimer1, 0, &IID_IEcoTimer1, (void**)&pCMe->m_pIArmTimer);
    if (result != 0 || pCMe->m_pIArmTimer == 0) {
        return result;
    }
    pCMe->m_pIArmTimer->pVTbl->set_Interval(pCMe->m_pIArmTimer, 1000000);
    pCMe->m_pIArmTimer->pVTbl->set_IrqHandler(pCMe->m_pIArmTimer, CEcoTaskScheduler1Lab_C761620F_TimerHandler);

    (void)heapStartAddress; (void)size;
    return 0;
}

/* NewTask: C89 объявления в начале блока */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_NewTask(IEcoTaskScheduler1Ptr_t me, voidptr_t address, voidptr_t data, uint32_t stackSize, IEcoTask1** ppITask) {
    EcoTaskFunc fn;
    IEcoTask1* pITask;
    if (me == 0) return -1;
    if (g_state.initialized == 0) return -1;

    fn = (EcoTaskFunc)address;
    pITask = 0;

    if (fn == 0) return -1;
    if (ppITask != 0) {
        *ppITask = 0;
    }
    if (FCFS_Push(&g_state.queue, fn, (void*)data, stackSize, pITask) != 0) {
        return -1;
    }
    return 0;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_DeleteTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) { (void)me; (void)taskId; return 0; }
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_SuspendTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) { (void)me; (void)taskId; return 0; }
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_ResumeTask(IEcoTaskScheduler1Ptr_t me, uint16_t taskId) { (void)me; (void)taskId; return 0; }
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_RegisterInterrupt(IEcoTaskScheduler1Ptr_t me, uint16_t number, voidptr_t handlerAddress, int32_t flag) { (void)me; (void)number; (void)handlerAddress; (void)flag; return 0; }
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_UnRegisterInterrupt(IEcoTaskScheduler1Ptr_t me, uint16_t number) { (void)me; (void)number; return 0; }

/* IRQ handler */
void CEcoTaskScheduler1Lab_C761620F_TimerHandler(void) {
#if defined(ECO_AARCH64)
    __asm volatile ("BL  CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext \n");
#else
    CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext();
#endif
}

/* Context switch stub (для совместимости с шаблоном) */
void CEcoTaskScheduler1Lab_C761620F_TaskSwitchContext(void) {
    g_indx++;
    if (g_indx > 1) {
        g_indx = 0;
    }
    g_xCEcoTask1List_C761620F[g_indx].pfunc();
}

/* FCFS Start */
static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Start(IEcoTaskScheduler1Ptr_t me) {
    FCFS_TaskItem item;
    if (me == 0) return -1;
    if (g_state.initialized) {
        while (FCFS_Pop(&g_state.queue, &item) == 0) {
            if (item.fn) item.fn();
        }
        return 0;
    }
    return 0;
}

static int16_t ECOCALLMETHOD CEcoTaskScheduler1Lab_C761620F_Stop(struct IEcoTaskScheduler1* me) {
    if (me == 0) return -1;
    return 0;
}

/* init/create/delete (FCFS init добавлен) */
int16_t ECOCALLMETHOD initCEcoTaskScheduler1Lab_C761620F(IEcoTaskScheduler1Ptr_t me, struct IEcoUnknown *pIUnkSystem) {
    if (me == 0) return -1;
    FCFS_Init(&g_state.queue);
    g_state.initialized = 1;
    (void)pIUnkSystem;
    return 0;
}

int16_t ECOCALLMETHOD createCEcoTaskScheduler1Lab_C761620F(IEcoUnknown* pIUnkSystem, IEcoUnknown* pIUnkOuter, IEcoTaskScheduler1Ptr_t* ppITaskScheduler) {
    int16_t result = -1;
    CEcoTaskScheduler1Lab_C761620F* pCMe = 0;
    if (ppITaskScheduler == 0) return result;
    pCMe = &g_xCEcoTaskScheduler1Lab_C761620F;
    if (pCMe->m_cRef == 0) {
        pCMe->m_cRef = 1;
        pCMe->m_pVTblIScheduler = &g_x155C975395654F85B9AA27D5F377A79EVTbl_C761620F;
        FCFS_Init(&g_state.queue);
        g_state.initialized = 1;
        result = 0;
    }
    *ppITaskScheduler = (IEcoTaskScheduler1*)pCMe;
    (void)pIUnkSystem; (void)pIUnkOuter;
    return result;
}

void ECOCALLMETHOD deleteCEcoTaskScheduler1Lab_C761620F(IEcoTaskScheduler1Ptr_t pITaskScheduler) {
    (void)pITaskScheduler;
}