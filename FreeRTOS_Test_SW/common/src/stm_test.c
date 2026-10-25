/*
 * stm_test.c
 *
 * Non-blocking STM test runner. TaskA_10ms or TaskB_10ms advances the
 * state machine; the STM callback only records the compare event.
 */

#include <stddef.h>

#include "stm_test.h"

#include "Stm_Ip.h"

#define STM_TEST_CHANNEL_INDEX             (3U)
#define STM_TEST_ONESHOT_OBSERVE_CYCLES    (3U)
#define STM_TEST_CONTINUOUS_DURATION_CYCLES (10U)
#define STM_TEST_COUNTER_SAMPLE_COUNT      (4U)
#define STM_TEST_TIMEOUT_CYCLES            (100U)

volatile StmTestIdType g_StmTestCommand = STM_TEST_ID_NONE;
volatile StmTestResultType g_StmTestResult =
{
    STM_TEST_ID_NONE,
    STM_TEST_STATUS_IDLE,
    0U,
    0U,
    0U,
    0U,
    STM_TEST_ERROR_NONE
};

typedef enum
{
    STM_TEST_PHASE_IDLE = 0U,
    STM_TEST_PHASE_WAIT_CALLBACK,
    STM_TEST_PHASE_OBSERVE_ONESHOT,
    STM_TEST_PHASE_RUN_CONTINUOUS,
    STM_TEST_PHASE_SAMPLE_COUNTER
} StmTestPhaseType;

static const StmTestConfigType *s_config = NULL;
static StmTestPhaseType s_phase = STM_TEST_PHASE_IDLE;
static uint32 s_elapsedCycles = 0U;
static uint32 s_observeCyclesRemaining = 0U;
static uint32 s_counterPrevious = 0U;
static uint32 s_counterSamples = 0U;
/* Read from both task and ISR context; initialization precedes channel use. */
static volatile boolean s_isInitialized = FALSE;

static void StmTest_Finish(
    StmTestStatusType status,
    StmTestErrorType error,
    boolean disableChannel
)
{
    if (disableChannel && (s_config != NULL))
    {
        Stm_Ip_DisableChannel(
            s_config->instance,
            STM_TEST_CHANNEL_INDEX
        );
    }

    g_StmTestResult.error = (uint32)error;
    g_StmTestResult.status = (uint32)status;
    g_StmTestCommand = STM_TEST_ID_NONE;
    s_phase = STM_TEST_PHASE_IDLE;
}

static bool StmTest_StartChannelTest(
    StmTestIdType testId,
    Stm_Ip_ChannelModeType mode
)
{
    Stm_Ip_ChannelConfigType runtimeChannelConfig;

    if ((s_config == NULL) || (s_config->channelConfig == NULL))
    {
        return false;
    }

    /*
     * The generated configuration supplies the channel identity and
     * callback. Copy it so a test can select its mode without editing
     * generated files or changing the other STM channels.
     */
    runtimeChannelConfig = *(s_config->channelConfig);
    runtimeChannelConfig.channelMode = mode;
    Stm_Ip_InitChannel(s_config->instance, &runtimeChannelConfig);

    g_StmTestResult.callbackCount = 0U;
    g_StmTestResult.activeTestId = (uint32)testId;
    g_StmTestResult.status = (uint32)STM_TEST_STATUS_RUNNING;
    g_StmTestResult.error = (uint32)STM_TEST_ERROR_NONE;
    s_elapsedCycles = 0U;

    Stm_Ip_StartCounting(
        s_config->instance,
        STM_TEST_CHANNEL_INDEX,
        s_config->compareTicks
    );

    s_phase = STM_TEST_PHASE_WAIT_CALLBACK;
    return true;
}

static void StmTest_StartRequestedTest(StmTestIdType testId)
{
    g_StmTestResult.activeTestId = (uint32)testId;
    g_StmTestResult.status = (uint32)STM_TEST_STATUS_RUNNING;
    g_StmTestResult.callbackCount = 0U;
    g_StmTestResult.expectedCallbackCount = 0U;
    g_StmTestResult.lastCounterDeltaTicks = 0U;
    g_StmTestResult.compareRemainingTicks = 0U;
    g_StmTestResult.error = (uint32)STM_TEST_ERROR_NONE;
    s_elapsedCycles = 0U;
    s_observeCyclesRemaining = 0U;
    s_counterSamples = 0U;

    switch (testId)
    {
        case STM_TEST_ID_ONESHOT_COMPARE_IRQ:
            g_StmTestResult.expectedCallbackCount = 1U;
            if (!StmTest_StartChannelTest(
                    testId,
                    STM_IP_CH_MODE_ONESHOT))
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_CHANNEL_INIT,
                    FALSE
                );
            }
            break;

        case STM_TEST_ID_CONTINUOUS_COMPARE_IRQ:
            g_StmTestResult.expectedCallbackCount = 3U;
            if (!StmTest_StartChannelTest(
                    testId,
                    STM_IP_CH_MODE_CONTINUOUS))
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_CHANNEL_INIT,
                    FALSE
                );
            }
            else
            {
                s_phase = STM_TEST_PHASE_RUN_CONTINUOUS;
            }
            break;

        case STM_TEST_ID_COUNTER_MONOTONIC:
            if (s_config == NULL)
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_CHANNEL_INIT,
                    FALSE
                );
            }
            else
            {
                s_counterPrevious = Stm_Ip_GetCounterValue(s_config->instance);
                s_phase = STM_TEST_PHASE_SAMPLE_COUNTER;
            }
            break;

        case STM_TEST_ID_COMPARE_READBACK:
            if (!StmTest_StartChannelTest(
                    testId,
                    STM_IP_CH_MODE_ONESHOT))
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_CHANNEL_INIT,
                    FALSE
                );
            }
            else
            {
                uint32 counterValue;
                uint32 compareValue;
                uint32 remainingTicks;

                compareValue = Stm_Ip_GetCompareValue(
                    s_config->instance,
                    STM_TEST_CHANNEL_INDEX
                );
                counterValue = Stm_Ip_GetCounterValue(s_config->instance);
                remainingTicks = compareValue - counterValue;
                g_StmTestResult.compareRemainingTicks = remainingTicks;

                if ((remainingTicks > 0U) &&
                    (remainingTicks <= s_config->compareTicks))
                {
                    StmTest_Finish(
                        STM_TEST_STATUS_PASS,
                        STM_TEST_ERROR_NONE,
                        TRUE
                    );
                }
                else
                {
                    StmTest_Finish(
                        STM_TEST_STATUS_FAIL,
                        STM_TEST_ERROR_COMPARE_RANGE,
                        TRUE
                    );
                }
            }
            break;

        default:
            StmTest_Finish(
                STM_TEST_STATUS_FAIL,
                STM_TEST_ERROR_INVALID_ID,
                FALSE
            );
            break;
    }
}

bool StmTest_Init(void)
{
    s_config = StmTest_GetConfig();

    if ((s_config == NULL) ||
        (s_config->channelConfig == NULL) ||
        (s_config->channelConfig->hwChannel != STM_TEST_CHANNEL_INDEX) ||
        (s_config->compareTicks == 0U))
    {
        return false;
    }

    Stm_Ip_InitChannel(s_config->instance, s_config->channelConfig);
    s_isInitialized = TRUE;
    return true;
}

void StmTest_NotifyFromISR(uint8 channel)
{
    if (s_isInitialized &&
        (s_config != NULL) &&
        (channel == STM_TEST_CHANNEL_INDEX) &&
        (g_StmTestResult.status == (uint32)STM_TEST_STATUS_RUNNING))
    {
        g_StmTestResult.callbackCount++;
    }
}

void StmTest_Task10ms(void)
{
    StmTestIdType requestedTest;

    if (!s_isInitialized)
    {
        return;
    }

    if (s_phase == STM_TEST_PHASE_IDLE)
    {
        requestedTest = g_StmTestCommand;
        if (requestedTest != STM_TEST_ID_NONE)
        {
            StmTest_StartRequestedTest(requestedTest);
        }
        return;
    }

    s_elapsedCycles++;

    switch (s_phase)
    {
        case STM_TEST_PHASE_WAIT_CALLBACK:
            if (g_StmTestResult.callbackCount > 1U)
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_CALLBACK_COUNT,
                    TRUE
                );
            }
            else if (g_StmTestResult.callbackCount == 1U)
            {
                if (g_StmTestResult.activeTestId ==
                    (uint32)STM_TEST_ID_ONESHOT_COMPARE_IRQ)
                {
                    s_observeCyclesRemaining =
                        STM_TEST_ONESHOT_OBSERVE_CYCLES;
                    s_phase = STM_TEST_PHASE_OBSERVE_ONESHOT;
                }
                else
                {
                    /* Unexpected callback from the compare-readback test. */
                    StmTest_Finish(
                        STM_TEST_STATUS_FAIL,
                        STM_TEST_ERROR_CALLBACK_COUNT,
                        TRUE
                    );
                }
            }
            else if (s_elapsedCycles >= STM_TEST_TIMEOUT_CYCLES)
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_CALLBACK_TIMEOUT,
                    TRUE
                );
            }
            break;

        case STM_TEST_PHASE_OBSERVE_ONESHOT:
            if (g_StmTestResult.callbackCount != 1U)
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_CALLBACK_COUNT,
                    TRUE
                );
            }
            else if (s_observeCyclesRemaining > 0U)
            {
                s_observeCyclesRemaining--;
                if (s_observeCyclesRemaining == 0U)
                {
                    StmTest_Finish(
                        STM_TEST_STATUS_PASS,
                        STM_TEST_ERROR_NONE,
                        FALSE
                    );
                }
            }
            break;

        case STM_TEST_PHASE_RUN_CONTINUOUS:
            if (s_elapsedCycles >= STM_TEST_CONTINUOUS_DURATION_CYCLES)
            {
                Stm_Ip_DisableChannel(
                    s_config->instance,
                    STM_TEST_CHANNEL_INDEX
                );

                if (g_StmTestResult.callbackCount >=
                    g_StmTestResult.expectedCallbackCount)
                {
                    StmTest_Finish(
                        STM_TEST_STATUS_PASS,
                        STM_TEST_ERROR_NONE,
                        FALSE
                    );
                }
                else
                {
                    StmTest_Finish(
                        STM_TEST_STATUS_FAIL,
                        STM_TEST_ERROR_CALLBACK_COUNT,
                        FALSE
                    );
                }
            }
            break;

        case STM_TEST_PHASE_SAMPLE_COUNTER:
        {
            uint32 currentCounter;
            uint32 deltaTicks;

            currentCounter = Stm_Ip_GetCounterValue(s_config->instance);
            deltaTicks = currentCounter - s_counterPrevious;
            s_counterPrevious = currentCounter;
            g_StmTestResult.lastCounterDeltaTicks = deltaTicks;

            if (deltaTicks == 0U)
            {
                StmTest_Finish(
                    STM_TEST_STATUS_FAIL,
                    STM_TEST_ERROR_COUNTER_STALLED,
                    FALSE
                );
            }
            else
            {
                s_counterSamples++;
                if (s_counterSamples >= STM_TEST_COUNTER_SAMPLE_COUNT)
                {
                    StmTest_Finish(
                        STM_TEST_STATUS_PASS,
                        STM_TEST_ERROR_NONE,
                        FALSE
                    );
                }
                else if (s_elapsedCycles >= STM_TEST_TIMEOUT_CYCLES)
                {
                    StmTest_Finish(
                        STM_TEST_STATUS_FAIL,
                        STM_TEST_ERROR_COUNTER_STALLED,
                        FALSE
                    );
                }
            }
            break;
        }

        case STM_TEST_PHASE_IDLE:
        default:
            /* An impossible phase is reported as an invalid command. */
            StmTest_Finish(
                STM_TEST_STATUS_FAIL,
                STM_TEST_ERROR_INVALID_ID,
                TRUE
            );
            break;
    }
}
