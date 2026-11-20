/* Native-only, private game trace. Never packaged with the application. */
#ifndef GRX_SURVEY_CAPTURE_H
#define GRX_SURVEY_CAPTURE_H
#include "survey.h"
void grx_survey_capture_open();
void grx_survey_capture_call(const grx_guest_call *,uint32_t,const grx_guest_io *,const grx_survey *);
void grx_survey_capture_close();
#endif
