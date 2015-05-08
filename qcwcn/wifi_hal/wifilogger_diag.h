/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __WIFI_HAL_WIFILOGGER_DIAG_H__
#define __WIFI_HAL_WIFILOGGER_DIAG_H__

#include "common.h"
#include "wifi_hal.h"
#include "wifilogger_event_defs.h"

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/rtnetlink.h>

#define ANI_NL_MSG_LOG_REG_TYPE  0x0001
#define ANI_NL_MSG_BASE     0x10    /* Some arbitrary base */
#define WIFI_HAL_USER_SOCK_PORT    646
#define WLAN_NL_MSG_CNSS_HOST_EVENT_LOG    17
#define ANI_NL_MSG_LOG_HOST_EVENT_LOG_TYPE 0x5050

#define WLAN_PKT_LOG_STATS            0x18E0

/*
 *  - verbose_level 0 corresponds to no collection
 *  - verbose_level 1 correspond to normal log level, with minimal user impact.
 *    this is the default value
 *  - verbose_level 2 are enabled when user is lazily trying to reproduce a
 problem, wifi performances and power
 *     can be impacted but device should not otherwise be significantly impacted
 *  - verbose_level 3+ are used when trying to actively debug a problem
 */

enum wifilogger_verbose_level {
   VERBOSE_NO_COLLECTION,
   VERBOSE_NORMAL_LOG,
   VERBOSE_REPRO_PROBLEM,
   VERBOSE_DEBUG_PROBLEM
};

enum wifilogger_fw_diag_type {
   DIAG_TYPE_FW_EVENT,     /* send fw event- to diag*/
   DIAG_TYPE_FW_LOG,       /* send log event- to diag*/
   DIAG_TYPE_FW_DEBUG_MSG, /* send dbg message- to diag*/
   DIAG_TYPE_FW_MSG = 4,   /* send fw message- to diag*/
};

enum wifilogger_host_diag_type {
   DIAG_TYPE_HOST_LOG_MSGS=1,
   DIAG_TYPE_HOST_EVENTS=2,
};

enum wlan_diag_frame_type {
     WLAN_DIAG_TYPE_CONFIG,
     WLAN_DIAG_TYPE_EVENT,
     WLAN_DIAG_TYPE_LOG,
     WLAN_DIAG_TYPE_MSG,
     WLAN_DIAG_TYPE_LEGACY_MSG,
};

static uint32_t get_le32(const uint8_t *pos)
{
    return pos[0] | (pos[1] << 8) | (pos[2] << 16) | (pos[3] << 24);
}

typedef struct event_remap {
    int q_event;
    int g_event;
} event_remap_t;

typedef struct {
    u32 diag_type;
    u32 timestamp;
    u32 length;
    u32 dropped;
    /* max ATH6KL_FWLOG_PAYLOAD_SIZE bytes */
    u_int8_t payload[0];
}__attribute__((packed)) dbglog_slot;

typedef enum eAniNlModuleTypes {
    ANI_NL_MSG_NETSIM = ANI_NL_MSG_BASE,// NetSim Messages (to the server)
    ANI_NL_MSG_PUMAC,       // Messages for/from the Upper MAC driver
    ANI_NL_MSG_WNS,         // Messages for the Wireless Networking
                            //  Services module(s)
    ANI_NL_MSG_MACSW,       // Messages from MAC
    ANI_NL_MSG_ES,          // Messages from ES
    ANI_NL_MSG_WSM,         // Message from the WSM in user space
    ANI_NL_MSG_DVT,         // Message from the DVT application
    ANI_NL_MSG_PTT,         // Message from the PTT application
    ANI_NL_MSG_MAC_CLONE,     //Message from the Mac clone App
    ANI_NL_MSG_LOG = ANI_NL_MSG_BASE + 0x0C, // Message for WLAN logging
    ANI_NL_MSG_MAX
} tAniNlModTypes;

//All Netlink messages must contain this header
typedef struct sAniHdr {
   unsigned short type;
   unsigned short length;
} tAniHdr, tAniMsgHdr;

/*
 * This msg hdr will always follow tAniHdr in all the messages exchanged
 * between the Applications in userspace the Pseudo Driver, in either
 * direction.
 */
typedef struct sAniNlMsg {
    struct  nlmsghdr nlh;   // Netlink Header
    int radio;          // unit number of the radio
    tAniHdr wmsg;       // Airgo Message Header
} tAniNlHdr;

typedef struct sAniAppRegReq {
    tAniNlModTypes type;    /* The module id that the application is
                    registering for */
    int pid;            /* Pid returned in the nl_sockaddr structure
                    in the call getsockbyname after the
                    application opens and binds a netlink
                    socket */
} tAniNlAppRegReq;

typedef struct host_event_hdr_s
{
    u16 event_id;
    u16 length;
} host_event_hdr_t;

typedef struct fw_event_hdr_s
{
    u16 diag_type;
    u16 length;
} fw_event_hdr_t;

typedef struct
{
    u32 timestamp:24;
    u32 diag_event_type:8;
    u16 payload_len;
    u16 diag_id;
    u8  payload[0];
}__attribute__((packed)) fw_diag_msg_hdr_t;

typedef struct wlan_wake_lock_event {
    u32 status;
    u32 reason;
    u32 timeout;
    u32 name_len;
    char name[];
} wlan_wake_lock_event_t;

wifi_error diag_message_handler(hal_info *info, nl_msg *msg);

#endif /* __WIFI_HAL_WIFILOGGER_DIAG_H__ */