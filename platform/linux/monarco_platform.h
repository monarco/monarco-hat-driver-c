/***************************************************************************//**
 * @file monarco_platform.h
 * @brief LibMonarco - Platform specific header - Generic Linux platform
 *******************************************************************************
 * @section License
 * Copyright REX Controls s.r.o. http://www.rexcontrols.com
 * Author: Vlastimil Setka
 * This file is covered by the BSD 3-Clause License
 *   see LICENSE.txt in the root directory of this project
 *   or <https://opensource.org/licenses/BSD-3-Clause>
 *******************************************************************************/

#ifndef LIBMONARCO_PLATFORM_H_
#define LIBMONARCO_PLATFORM_H_

#define MONARCO_DPF_ERROR     0x01
#define MONARCO_DPF_WARNING   0x02
#define MONARCO_DPF_INFO      0x04
#define MONARCO_DPF_VERB      0x08
#define MONARCO_DPF_READ      0x10
#define MONARCO_DPF_WRITE     0x20

#define MONARCO_DPF_TO_STR(flags) \
    ( (flags) == MONARCO_DPF_ERROR ? "ERROR" : \
    ( (flags) == MONARCO_DPF_WARNING ? "WARNING" : \
    ( (flags) == MONARCO_DPF_INFO ? "INFO" : \
    ( (flags) == MONARCO_DPF_VERB ? "VERB" : \
    ( (flags) == MONARCO_DPF_READ ? "READ" : \
    ( (flags) == MONARCO_DPF_WRITE ? "WRITE" : "?" ) ) ) ) ) )

extern int monarco_platform_dprint_flags;

#define MONARCO_DPRINT(flags, fmtstr, ...) do { \
    if ((flags) & monarco_platform_dprint_flags) \
        printf("%s[%s] " fmtstr, cxt->platform == NULL ? "" : (char *)(cxt->platform), MONARCO_DPF_TO_STR(flags), ##__VA_ARGS__); \
    } while (0)

#endif
