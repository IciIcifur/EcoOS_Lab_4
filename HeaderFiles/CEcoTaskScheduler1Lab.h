/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   CEcoTaskScheduler1Lab
 * </сводка>
 *
 * <описание>
 *   Данный заголовок описывает реализацию компонента CEcoTaskScheduler1Lab
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

#ifndef __C_ECO_TASK_SCHEDULER_1_LAB_H__
#define __C_ECO_TASK_SCHEDULER_1_LAB_H__

#include "IEcoTaskScheduler1.h"
#include "CEcoTask1Lab.h"
#include "IEcoSystem1.h"
#include "IdEcoTimer1.h"
#include "IdEcoMemoryManager1.h"

/*
 * <сводка>
 *   Состояние задачи
 * </сводка>
 *
 * <описание>
 *   Перечисление состояний задач для планировщика
 * </описание>
 */
typedef enum {
    ECO_TASK_STATE_EMPTY = 0,
    ECO_TASK_STATE_RUNNABLE,
    ECO_TASK_STATE_BLOCKED,
    ECO_TASK_STATE_DELAYED
} ECO_TASK_STATE;

/*
 *
 * <сводка>
 *   Расширение структуры задачи для планировщика
 * </сводка>
 *
 * <описание>
 *   Дополнительные поля состояния задачи и задержки
 * </описание>
 *
 */
typedef struct CEcoTask1Lab_C761620F_ext {
    CEcoTask1Lab_C761620F base;
    /* Состояние задачи */
    ECO_TASK_STATE state;
    /* Счётчик тиков задержки (для Delay) */
    uint32_t delayTicks;
    /* Сохранённый указатель стека (для вытесняющего режима) */
    uint64_t* saved_sp;
} CEcoTask1Lab_C761620F_ext;

typedef struct CEcoTaskScheduler1Lab_C761620F {

    /* Таблица функций интерфейса IEcoTaskScheduler1Lab */
    IEcoTaskScheduler1VTbl* m_pVTblIScheduler;

    /* Счетчик ссылок */
    uint32_t m_cRef;

    /* Интерфейсная Шина */
    IEcoInterfaceBus1* m_pIBus;

    /* Интерфейс для работы с памятью */
    IEcoMemoryAllocator1* m_pIMem;

    /* Системный интерфейс */
    IEcoSystem1* m_pISys;

    /* Данные экземпляра */
    IEcoTimer1Ptr_t m_pIArmTimer;

    /* Список задач */
    CEcoTask1Lab_C761620F_ext* m_pTaskList;

    /* Количество задач в очереди */
    uint32_t m_TaskCount;

    /* Индекс текущей задачи */
    uint32_t m_CurrentIndex;

    /* Период тика таймера в мс */
    uint32_t m_TickUs;

} CEcoTaskScheduler1Lab_C761620F, *CEcoTaskScheduler1Lab_C761620FPtr;

/* Инициализация экземпляра */
int16_t ECOCALLMETHOD initCEcoTaskScheduler1Lab_C761620F(/*in*/ IEcoTaskScheduler1Ptr_t me, /* in */ IEcoUnknown* pIUnkSystem);
/* Создание экземпляра */
int16_t ECOCALLMETHOD createCEcoTaskScheduler1Lab_C761620F(/* in */ IEcoUnknown* pIUnkSystem, /* in */ IEcoUnknown* pIUnkOuter, /* out */ IEcoTaskScheduler1Ptr_t* ppITaskScheduler);
/* Удаление */
void ECOCALLMETHOD deleteCEcoTaskScheduler1Lab_C761620F(/* in */ IEcoTaskScheduler1Ptr_t pITaskScheduler);

/*
 * <сводка>
 *   Обработчик таймера планировщика
 * </сводка>
 *
 * <описание>
 *   Вызывается аппаратным таймером. Вытесняющий режим — переключение задач.
 * </описание>
 */
void CEcoTaskScheduler1Lab_C761620F_TimerHandler(void);

#endif /* __C_ECO_TASK_SCHEDULER_1_LAB_H__ */