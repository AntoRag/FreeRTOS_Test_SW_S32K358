/*
 * stm_test.h
 *
 * Debugger-triggered STM channel tests shared by both AMP images.
 */

#ifndef STM_TEST_H_
#define STM_TEST_H_

#include <stdbool.h>
#include <stdint.h>

#include "Stm_Ip_Types.h"

/*
 * Test IDs are part of the debugger-facing interface. Keep assigned values
 * stable; add new tests at the end instead of reusing an old ID.
 */
typedef enum
{
    STM_TEST_ID_NONE = 0U,
    STM_TEST_ID_ONESHOT_COMPARE_IRQ = 1U,
    STM_TEST_ID_CONTINUOUS_COMPARE_IRQ = 2U,
    STM_TEST_ID_COUNTER_MONOTONIC = 3U,
    STM_TEST_ID_COMPARE_READBACK = 4U,
    STM_TEST_ID_COUNT
} StmTestIdType;

typedef enum
{
    STM_TEST_STATUS_IDLE = 0U,
    STM_TEST_STATUS_RUNNING,
    STM_TEST_STATUS_PASS,
    STM_TEST_STATUS_FAIL
} StmTestStatusType;

typedef enum
{
    STM_TEST_ERROR_NONE = 0U,
    STM_TEST_ERROR_INVALID_ID,
    STM_TEST_ERROR_CHANNEL_INIT,
    STM_TEST_ERROR_CALLBACK_TIMEOUT,
    STM_TEST_ERROR_CALLBACK_COUNT,
    STM_TEST_ERROR_COUNTER_STALLED,
    STM_TEST_ERROR_COMPARE_RANGE
} StmTestErrorType;

typedef struct
{
    uint8 instance;
    const Stm_Ip_ChannelConfigType *channelConfig;
    uint32 compareTicks;
} StmTestConfigType;

typedef struct
{
    uint32 activeTestId;
    uint32 status;
    uint32 callbackCount;
    /* For continuous tests this is the minimum callback count. */
    uint32 expectedCallbackCount;
    uint32 lastCounterDeltaTicks;
    uint32 compareRemainingTicks;
    uint32 error;
} StmTestResultType;

/* Set this to a StmTestIdType value from the debugger to request one test. */
extern volatile StmTestIdType g_StmTestCommand;

/* Each ELF has its own copy; inspect it in that core's debugger session. */
extern volatile StmTestResultType g_StmTestResult;

/* Implemented in each image's platform binding using its generated CH_3. */
const StmTestConfigType *StmTest_GetConfig(void);

bool StmTest_Init(void);
void StmTest_Task10ms(void);
void StmTest_NotifyFromISR(uint8 channel);

#endif /* STM_TEST_H_ */
