#include <mvs_request_response_conversion.h>

apiReqResponse_t mvs_convert_result_to_req_response(mResult_t res) {
  switch (res) {
  case MRES_SUCCESS:
    return API_REQ_RESPONSE_NONE;
  case MRES_INVALID_ARGS:
    return API_REQ_RESPONSE_INVALID_DATA;
  case MRES_RESOURCE_SIZE_LIMITED:
    return API_REQ_RESPONSE_RESOURCE_EXHAUSTED;
  case MRES_RESOURCE_STATE_INVALID:
  case MRES_RESOURCE_SHARED:
  case MRES_RESOURCE_NOT_CONFIGURED:
    return API_REQ_RESPONSE_RESOURCE_INVALID_STATE;
  case MRES_SYS_FAILURE:
    return API_REQ_RESPONSE_HOST_FAILURE;
  case MRES_COULDNT_COMPLETE:
    return API_REQ_RESPONSE_RETRY;
  }
  return API_REQ_RESPONSE_NONE;
}
