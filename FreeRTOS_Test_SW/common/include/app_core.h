/*
 * app_core.h
 *
 * Logical application-core identity shared by both AMP images.
 */

#ifndef APP_CORE_H_
#define APP_CORE_H_

#include <stdint.h>

/*
 * Logical indices are intentionally contiguous and are safe for arrays.
 * They are not the raw MSCM CPXNUM values: CM7_1 is the lockstep checker
 * and does not own an application slot.
 */
typedef enum
{
    APP_CORE_PRIMARY = 0U,
    APP_CORE_SECONDARY,
    APP_CORE_COUNT,
    APP_CORE_INVALID = 0xFFU
} AppCoreIndexType;

#define APP_CORE_PRIMARY_PHYSICAL_ID       (0U)
#define APP_CORE_SECONDARY_PHYSICAL_ID     (2U)

/* Convert the executing hardware core ID to a contiguous application index. */
AppCoreIndexType AppCore_GetCurrentIndex(void);

/* Return the raw hardware core ID reported by the S32K3 startup support. */
uint32_t AppCore_GetCurrentPhysicalId(void);

#endif /* APP_CORE_H_ */
