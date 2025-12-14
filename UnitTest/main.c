#include <stdio.h>
#include "IEcoBase1.h"
#include "IEcoTaskScheduler1.h"
#include "CEcoTaskScheduler1Lab.h"
#include "IdEcoTaskScheduler1Lab.h"

int main(void) {
    IEcoTaskScheduler1* pSched = 0;
    int16_t r;

    r = createCEcoTaskScheduler1Lab_C761620F(0, 0, &pSched);
    if (r != 0 || pSched == 0) {
        printf("Failed to create scheduler, r=%d\n", r);
        return 1;
    }

    r = pSched->pVTbl->Init(pSched, 0);
    if (r != 0) {
        printf("Init failed, r=%d\n", r);
    } else {
        printf("Init OK\n");
    }

    pSched->pVTbl->AddRef(pSched);
    pSched->pVTbl->Release(pSched);

    pSched->pVTbl->Release(pSched);
    printf("Release OK\n");
    return 0;
}