#include <mvs_logger.h>

_MVS_ATTR_INTERNAL_ MVSLogger *_logger; // Lets keep a local copy instead of
                                        // passing it as argument each time
_MVS_ATTR_INTERNAL_ mstr_t msg_type[] = {"Note", "Warning", "Error", "Debug"};

_MVS_ATTR_INTERNAL_ void mvs_logger_log_all() {
  MVSLogEntry entry;
  mResult_t res;
  while (!_MVS_HYBRID_CONCURRENCY_MODEL_QUEUE_CHECK_ISEMPTY_(_logger->queue)) {
    if ((res = mvs_hybrid_concurrency_model_queue_dequeue(
             _logger->queue, &entry)) != MRES_SUCCESS) {
      fprintf(stderr, "<Logger Error>: Error code=%u\n", res);
      // we don't terminate here yet
    } else {
      fprintf(stdout, "<%s>: %s\n", msg_type[entry.lvl], entry.msg);
    }
  }
}

mResult_t mvs_logger_init(mLogLvl_t allowed) {
  if (_logger)
    return MRES_SUCCESS;
  _logger = (MVSLogger *)malloc(sizeof(MVSLogger));
  if (!_logger)
    return MRES_SYS_FAILURE;
  mResult_t res = mvs_hybrid_concurrency_model_queue_create(
      &_logger->queue, 100 /*100 logs for now*/, sizeof(MVSLogEntry));
  if (res != MRES_SUCCESS) {
    free(_logger);
    return res;
  }
  if ((res = mvs_mutex_init(&_logger->lock)) != MRES_SUCCESS) {
    mvs_hybrid_concurrency_model_queue_destroy(_logger->queue);
    free(_logger);
    return res;
  }
  if ((res = mvs_cond_init(&_logger->cond)) != MRES_SUCCESS) {
    mvs_hybrid_concurrency_model_queue_destroy(_logger->queue);
    mvs_mutex_destroy(&_logger->lock);
    free(_logger);
    return res;
  }
  atomic_init(&_logger->stop, mfalse);
  atomic_init(&_logger->dead, mtrue);
  _logger->allowed_lvl = allowed;
  return MRES_SUCCESS;
}

mResult_t mvs_logger_destroy() {
  /*
   * The logger is guaranteed to log all messages before being destroyed
   */
  if (!_logger)
    return MRES_SUCCESS;
  mvs_hybrid_concurrency_model_queue_destroy(_logger->queue);
  mvs_mutex_destroy(&_logger->lock);
  mvs_cond_destroy(&_logger->cond);
  free(_logger);
  _logger = NULL;
  return MRES_SUCCESS;
}

mthreadRet_t mvs_logger_run(mptr_t _l) {
  _l = NULL; // in the future, _l could represent some result
  if (!_logger)
    return NULL;
  atomic_store_explicit(&_logger->dead, mfalse, memory_order_release);
  while (!atomic_load_explicit(&_logger->stop, memory_order_relaxed)) {
    mvs_mutex_lock(&_logger->lock);
    if (_MVS_HYBRID_CONCURRENCY_MODEL_QUEUE_CHECK_ISEMPTY_(_logger->queue)) {
      // go to sleep
      mvs_cond_wait(&_logger->cond, &_logger->lock);
    } else {
      mvs_logger_log_all();
    }
    mvs_mutex_unlock(&_logger->lock);
  }

  mvs_logger_log_all();
  atomic_store_explicit(&_logger->dead, mtrue, memory_order_release);
  return NULL;
}

void mvs_log_note(mstr_t fmt, ...) {
  if (!_logger)
    return;
  if (_logger->allowed_lvl < MLOG_NOTE)
    return;
  MVSLogEntry entry;
  entry.lvl = MLOG_NOTE;
  va_list args;
  va_start(args, fmt);
  vsnprintf(entry.msg, 128, fmt, args);
  va_end(args);
  // this may fail
  mvs_hybrid_concurrency_model_queue_enqueue(_logger->queue, &entry);
  mvs_cond_signal(&_logger->cond);
}

void mvs_log_warn(mstr_t fmt, ...) {
  if (!_logger)
    return;
  if (_logger->allowed_lvl < MLOG_WARN)
    return;
  MVSLogEntry entry;
  entry.lvl = MLOG_WARN;
  va_list args;
  va_start(args, fmt);
  vsnprintf(entry.msg, 128, fmt, args);
  va_end(args);
  mvs_hybrid_concurrency_model_queue_enqueue(_logger->queue, &entry);
  mvs_cond_signal(&_logger->cond);
}

void mvs_log_err(mstr_t fmt, ...) {
  if (!_logger)
    return;
  if (_logger->allowed_lvl < MLOG_ERR)
    return;
  MVSLogEntry entry;
  entry.lvl = MLOG_ERR;
  va_list args;
  va_start(args, fmt);
  vsnprintf(entry.msg, 128, fmt, args);
  va_end(args);
  mvs_hybrid_concurrency_model_queue_enqueue(_logger->queue, &entry);
  mvs_cond_signal(&_logger->cond);
}

void mvs_log_dbg(mstr_t fmt, ...) {
  if (!_logger)
    return;
  if (_logger->allowed_lvl < MLOG_DBG)
    return;
  MVSLogEntry entry;
  entry.lvl = MLOG_DBG;
  va_list args;
  va_start(args, fmt);
  vsnprintf(entry.msg, 128, fmt, args);
  va_end(args);
  mvs_hybrid_concurrency_model_queue_enqueue(_logger->queue, &entry);
  mvs_cond_signal(&_logger->cond);
}

void mvs_logger_wakeup(mbool_t flag) {
  if (!_logger)
    return;
  atomic_store_explicit(&_logger->stop, flag, memory_order_release);
  mvs_cond_signal(&_logger->cond);
}

void mvs_logger_wait_to_launch() {
  while (atomic_load_explicit(&_logger->dead, memory_order_relaxed)) {
  }	
}

void mvs_logger_wait_for_termination() {
  while (!atomic_load_explicit(&_logger->dead, memory_order_relaxed)) {
  	mvs_cond_signal(&_logger->cond);
  }
}
