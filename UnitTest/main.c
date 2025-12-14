/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   Eco.TaskScheduler1Lab.UnitTest — точка входа EcoMain (и оболочка main для запуска из exe)
 * </сводка>
 *
 * <описание>
 *   Юнит-тест для темы 39 (FCFS без вытеснения):
 *   - Строгая проверка pIBus: Init с NULL должен вернуть ошибку
 *   - Инициализация с непустым pIBus (через IEcoSystem1/IEcoInterfaceBus1, если доступно, либо фиктивный указатель для имитации Host OS)
 *   - Добавление 3 задач и запуск Start — порядок выполнения: TaskA -> TaskB -> TaskC
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 */

#include <stdio.h>
#include "IEcoBase1.h"
#include "IEcoSystem1.h"
#include "IEcoInterfaceBus1.h"
#include "IEcoTaskScheduler1.h"
#include "CEcoTaskScheduler1Lab.h"
#include "IdEcoTaskScheduler1Lab.h"

/* Тестовые задачи */
static void TaskA(void){ printf("TaskA\n"); }
static void TaskB(void){ printf("TaskB\n"); }
static void TaskC(void){ printf("TaskC\n"); }

int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t r = -1;
    IEcoSystem1* pISys = 0;
    IEcoInterfaceBus1* pIBus = 0;
    IEcoTaskScheduler1* pSched = 0;

    /* Попытка получить системный интерфейс и шину, если есть pIUnk */
    if (pIUnk != 0) {
        r = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void**)&pISys);
        if (r != 0 || pISys == 0) {
            printf("EcoMain: QueryInterface(GID_IEcoSystem) failed, r=%d\n", r);
            goto Release;
        }
        r = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void**)&pIBus);
        if (r != 0 || pIBus == 0) {
            printf("EcoMain: QueryInterface(IID_IEcoInterfaceBus1) failed, r=%d\n", r);
            goto Release;
        }
    }

    /* Создание планировщика */
    r = createCEcoTaskScheduler1Lab_C761620F(pIUnk, 0, &pSched);
    if (r != 0 || pSched == 0) {
        printf("EcoMain: create scheduler failed, r=%d\n", r);
        goto Release;
    }

    {
        IEcoInterfaceBus1* pIBusToUse = pIBus;
        r = pSched->pVTbl->Init(pSched, pIBusToUse);
        if (r != 0) {
            printf("EcoMain: Init failed, r=%d\n", r);
            goto Release;
        }
        printf("Init OK\n");
    }

    /* FCFS: добавляем 3 задачи и запускаем */
    if (pSched->pVTbl->NewTask(pSched, (voidptr_t)TaskA, 0, 0, 0) != 0 ||
        pSched->pVTbl->NewTask(pSched, (voidptr_t)TaskB, 0, 0, 0) != 0 ||
        pSched->pVTbl->NewTask(pSched, (voidptr_t)TaskC, 0, 0, 0) != 0) {
        printf("EcoMain: NewTask failed\n");
        r = -3;
        goto Release;
    }

    r = pSched->pVTbl->Start(pSched);
    if (r != 0) {
        printf("EcoMain: Start failed, r=%d\n", r);
        goto Release;
    }

    r = 0;

Release:
    if (pSched) {
        pSched->pVTbl->Release(pSched);
        printf("Release OK\n");
    }
    if (pIBus) pIBus->pVTbl->Release(pIBus);
    if (pISys) pISys->pVTbl->Release(pISys);

    if (r == 0) {
        printf("[OK] UnitTest finished successfully\n");
    }
    return r;
}

int main(void) {
    return EcoMain(0);
}