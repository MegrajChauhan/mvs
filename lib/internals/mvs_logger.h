#ifndef _MVS_LOGGER_
#define _MVS_LOGGER_

/*
 * For the logger subsystem, I have decided to use a buffer and a consumer.
 * The producers, the rest of MVS, will push messages for the consumer.
 * The consumer will keep a buffer of 1024 bytes in size. If the buffer is full,
 * the consumer will flush the buffer(or if no new message can be added). The
 * producer may request an immediate flush for important messages. If MVS
 * terminates, the logger will flush whatever remains in the buffer and
 * terminate.
 */

#include <mvs_config.h>
#include <mvs_threads.h>
#include <mvs_tools.h>
#include <mvs_types.h>
#include <mvs_queue.h>
#include <mvs_protectors.h>
#include <mvs_results.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdatomic.h>

typedef enum mLogLvl_t mLogLvl_t; 
typedef struct MVSLogger MVSLogger;
typedef struct MVSLogEntry MVSLogEntry;

enum mLogLvl_t {
	MLOG_NOTE,
	MLOG_WARN,
	MLOG_ERR,
	MLOG_DBG
};

struct MVSLogEntry {
	mLogLvl_t lvl; // level of the log message
	char msg[128]; // allowed level of just 128 characters including the terminating byte
};

struct MVSLogger {
	mLogLvl_t allowed_lvl;
	MVSHybridConcurrencyModelQueue *queue; // the lock-free queue that the loggers will use
	mmutex_t lock;
	mcond_t cond;
	atm_mbool_t stop;
};

/*
 * Maybe the logger will never fail...?
 * Even if it fails, it will print a message for the user but the system never knows
 * The logger remains silent...
 * If the logger failed to initialize or start, the system will know but any failures to
 * log won't be made known to the caller or the system
 */

mResult_t mvs_logger_init(mLogLvl_t allowed);
mResult_t mvs_logger_destroy();
mthreadRet_t mvs_logger_run(mptr_t _l);
void mvs_logger_wakeup(mbool_t flag);

// Logging functions
void mvs_log_note(mstr_t fmt, ...);
void mvs_log_warn(mstr_t fmt, ...);
void mvs_log_err(mstr_t fmt, ...);
void mvs_log_dbg(mstr_t fmt, ...);

#endif
