#include <stdio.h>
#include "IEcoBase1.h"
#include "IEcoTaskScheduler1.h"
#include "CEcoTaskScheduler1Lab.h"
#include "IdEcoTaskScheduler1Lab.h"

/* Тестовые задачи */
static void TaskA(void){ printf("TaskA\n"); }
static void TaskB(void){ printf("TaskB\n"); }
static void TaskC(void){ printf("TaskC\n"); }

int main(void) {
    IEcoTaskScheduler1* pSched = 0;
    int16_t r = createCEcoTaskScheduler1Lab_C761620F(0, 0, &pSched);
    if (r != 0 || pSched == 0) { printf("Failed to create scheduler, r=%d\n", r); return 1; }

    /* Должно упасть на NULL pIBus */
    r = pSched->pVTbl->Init(pSched, 0);
    if (r == 0) { printf("Init with NULL pIBus unexpectedly succeeded\n"); pSched->pVTbl->Release(pSched); return 2; }
    printf("[OK] Init(NULL) failed as expected, r=%d\n", r);

    /* Инициализация с непустым pIBus (фиктивный указатель достаточно для имитации) */
    r = pSched->pVTbl->Init(pSched, (IEcoInterfaceBus1*)1);
    if (r != 0) { printf("Init failed, r=%d\n", r); pSched->pVTbl->Release(pSched); return 3; }
    printf("Init OK\n");

    /* FCFS */
    if (pSched->pVTbl->NewTask(pSched, (voidptr_t)TaskA, 0, 0, 0) != 0 ||
        pSched->pVTbl->NewTask(pSched, (voidptr_t)TaskB, 0, 0, 0) != 0 ||
        pSched->pVTbl->NewTask(pSched, (voidptr_t)TaskC, 0, 0, 0) != 0) {
        printf("NewTask failed\n"); pSched->pVTbl->Release(pSched); return 4;
        }
    r = pSched->pVTbl->Start(pSched);
    if (r != 0) { printf("Start failed, r=%d\n", r); pSched->pVTbl->Release(pSched); return 5; }

    pSched->pVTbl->Release(pSched);
    printf("Release OK\n");
    printf("[OK] UnitTest finished successfully\n");
    return 0;
}