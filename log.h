/**
 *@file: log.h
 *@desc: log head file
 */

#ifndef __LOG_H__
#define __LOG_H__

#define LOG_NEWLINE_SIGN                    "\n"
/* buffer size for every line's log */
#define LOG_LINE_BUF_SIZE                   256
#define LOG_BUF_OUTPUT_BUF_SIZE             (LOG_LINE_BUF_SIZE*4*4*32)

#ifdef DBG
unsigned long bl_filter_severity = 0xffffffff;
unsigned long bl_filter_module   = 0xffffffff;
#else
unsigned long bl_filter_severity;
unsigned long bl_filter_module;
#endif

#define CO_BIT(pos) (1U<<(pos))

// Prefix used for module filtering
// If you modify any value, modify also DBG_MOD_ macros below
#define D_KE        "\x80"   ///< Prefix for Kernel
#define D_DBG       "\x81"   ///< Prefix for DBG
#define D_IPC       "\x82"   ///< Prefix for IPC
#define D_DMA       "\x83"   ///< Prefix for DMA
#define D_MM        "\x84"   ///< Prefix for LMAC management
#define D_TX        "\x85"   ///< Prefix for Tx path
#define D_RX        "\x86"   ///< Prefix for Rx path
#define D_PHY       "\x87"   ///< Prefix for Modem / RF
#define D_XX0       "\x88"   ///< Prefix unused
#define D_XX1       "\x89"   ///< Prefix unused
#define D_XX2       "\x8A"   ///< Prefix unused
#define D_XX3       "\x8B"   ///< Prefix unused
#define D_XX4       "\x8C"   ///< Prefix unused

// Prefix used for severity filtering
// If you modify any value, modify also DBG_SEV_ macros below
#define D_CRT       "\x9A"   ///< Prefix for critical
#define D_ERR       "\x9B"   ///< Prefix for error
#define D_WRN       "\x9C"   ///< Prefix for warning
#define D_INF       "\x9D"   ///< Prefix for info
#define D_VRB       "\x9E"   ///< Prefix for verbose debug

/// Module filtering macros, used only by debug module
enum dbg_mod_tag
{
    DBG_MOD_IDX_KE = 0,   ///< Bit index for Kernel
    DBG_MOD_IDX_DBG,      ///< Bit index for debug
    DBG_MOD_IDX_IPC,      ///< Bit index for IPC
    DBG_MOD_IDX_DMA,      ///< Bit index for DMA
    DBG_MOD_IDX_MM,       ///< Bit index for LMAC management
    DBG_MOD_IDX_TX,       ///< Bit index for Tx path
    DBG_MOD_IDX_RX,       ///< Bit index for Rx path
    DBG_MOD_IDX_PHY,      ///< Bit index for Modem / RF
    DBG_MOD_IDX_MAX,      ///< Number of modules
};

#define DBG_MOD_MIN     0x80
#define DBG_MOD_MAX     (DBG_MOD_MIN + DBG_MOD_IDX_MAX)

#define DBG_MOD_ALL         0xFFFFFFFF

/// Severity filtering macros, used only by debug module
enum dbg_sev_tag
{
    DBG_SEV_IDX_NONE = 0,   ///< No print allowed
    DBG_SEV_IDX_CRT,        ///< Critical and unspecified allowed only
    DBG_SEV_IDX_ERR,        ///< Error allowed and above
    DBG_SEV_IDX_WRN,        ///< Warning allowed and above
    DBG_SEV_IDX_INF,        ///< Info allowed and above
    DBG_SEV_IDX_VRB,        ///< All allowed
    DBG_SEV_IDX_MAX,        ///< Number of severity levels
    DBG_SEV_ALL             ///< Convenient macro
};

#define DBG_SEV_MIN     0x9A
#define DBG_SEV_MAX     0xA0

#ifdef DBG
void log_output(const char *fmt, ...);
#define log(fmt, ...)  log_output(fmt, ## __VA_ARGS__)
#else
#define log(fmt, ...)  do {} while (0) 
#endif

struct bl_log {
	void *buff;
	ssize_t buf_write_size;
};

#endif /*__LOG_H__*/
