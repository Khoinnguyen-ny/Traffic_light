/*
 * scheduler.c
 *
 *  Created on: Nov 9, 2026
 *      Author: Admin
 */

#include "scheduler.h"

uint8_t Error_code_G = 0;

static sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t  SCH_count = 0;
static uint32_t newTaskID = 1;
static volatile uint32_t tick = 0;
static volatile uint32_t missed = 0;

/* ========== Helper function ==========*/
static uint32_t SCH_Lock(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void SCH_Unlock(uint32_t primask) {
    __set_PRIMASK(primask);
}

static void push(uint32_t index, sTask newTask) {
    for (uint32_t i = SCH_count; i > index; i--) {
        SCH_tasks_G[i] = SCH_tasks_G[i - 1];
    }
    SCH_tasks_G[index] = newTask;
}

static void pull(uint32_t index) {
    for (uint32_t i = index; i + 1 < SCH_count; i++) {
        SCH_tasks_G[i] = SCH_tasks_G[i + 1];
    }
}

static uint8_t SCH_Insert(sTask t) {
    uint32_t primask = SCH_Lock();

    if (SCH_count >= SCH_MAX_TASKS) {
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        SCH_Unlock(primask);
        return RETURN_ERROR;
    }

    uint32_t index = 0;
    while (index < SCH_count && t.Delay >= SCH_tasks_G[index].Delay) {
        t.Delay -= SCH_tasks_G[index].Delay;
        index++;
    }
    push(index, t);

    if (index < SCH_count) {
        SCH_tasks_G[index + 1].Delay -= t.Delay;
    }

    SCH_count++;
    SCH_Unlock(primask);
    return RETURN_NORMAL;
}
/* ==============================*/


void SCH_Init(void) {
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_tasks_G[i].pTask  = 0;
        SCH_tasks_G[i].Delay  = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].TaskID = NO_TASK_ID;
        SCH_tasks_G[i].RunMe  = 0;
    }
    SCH_count    = 0;
    newTaskID    = 1;
    tick         = 0;
    missed       = 0;
    Error_code_G = 0;
}


void SCH_Update(void) {
    tick++;

    if (SCH_count == 0) return;

    if (SCH_tasks_G[0].Delay > 0) SCH_tasks_G[0].Delay--;
    else missed++;

    if (SCH_tasks_G[0].Delay == 0) SCH_tasks_G[0].RunMe = 1;
}


void SCH_Dispatch_Tasks(void) {
    while (1) {
        uint32_t primask = SCH_Lock();

        while (missed && SCH_count && SCH_tasks_G[0].Delay) {
            SCH_tasks_G[0].Delay--;
            missed--;
        }

        if (SCH_count && (SCH_tasks_G[0].RunMe || SCH_tasks_G[0].Delay == 0)) {
            sTask running = SCH_tasks_G[0];
            void (*f)(void) = running.pTask;
            running.Delay = running.Period;
            running.RunMe = 0;

            SCH_Delete_Task(running.TaskID);
            if (running.Period) {
                SCH_Insert(running);
            }
            SCH_Unlock(primask);

            f();
        } else {
            if (SCH_count == 0) missed = 0;
            SCH_Unlock(primask);

            SCH_Report_Status();
            SCH_Go_To_Sleep();
            return;
        }
    }
}


uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t Delay, uint32_t Period) {
    if (pFunction == 0) return NO_TASK_ID;

    uint32_t primask = SCH_Lock();

    if (SCH_count == 0) missed = 0;

    sTask newTask = {
        .pTask  = pFunction,
        .Delay  = Delay,
        .Period = Period,
        .TaskID = newTaskID,
        .RunMe  = 0
    };

    uint32_t id = NO_TASK_ID;
    if (SCH_Insert(newTask) == RETURN_NORMAL) {
        id = newTaskID++;
        if (newTaskID == NO_TASK_ID) newTaskID = 1;
    }

    SCH_Unlock(primask);
    return id;
}


uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID == NO_TASK_ID) {
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
        return RETURN_ERROR;
    }

    uint32_t primask = SCH_Lock();
    for (uint32_t i = 0; i < SCH_count; i++) {
        if (SCH_tasks_G[i].TaskID == taskID) {
            if (i + 1 < SCH_count) {
                SCH_tasks_G[i + 1].Delay += SCH_tasks_G[i].Delay;
            }
            pull(i);
            SCH_count--;
            SCH_Unlock(primask);
            return RETURN_NORMAL;
        }
    }

    Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
    SCH_Unlock(primask);
    return RETURN_ERROR;
}


uint32_t get_time(void) {
    return tick * TICK_MS;
}

void SCH_Report_Status(void) {
#ifdef SCH_REPORT_ERRORS
    static uint8_t Last_error_code_G = 0;
    if (Error_code_G != Last_error_code_G) {
        Last_error_code_G = Error_code_G;
    }
#endif
}

void SCH_Go_To_Sleep(void) {

}

