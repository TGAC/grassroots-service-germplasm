/*
 * germplasm_markup.h
 *
 *  Created on: 30 May 2018
 *      Author: billy
 */

#ifndef SERVICES_GERMPLASM_SERVICE_INCLUDE_GERMPLASM_MARKUP_H_
#define SERVICES_GERMPLASM_SERVICE_INCLUDE_GERMPLASM_MARKUP_H_

#include "germplasm_service_data.h"
#include "germplasm_service_library.h"
#include "jansson.h"
#include "germplasm_service_data.h"

#ifdef __cplusplus
extern "C"
{
#endif


GERMPLASM_SERVICE_LOCAL json_t *ConvertSeedstorResultToGrassrootsMarkUp (const json_t *blast_job_output_p, GermplasmServiceData *data_p);



#ifdef __cplusplus
}
#endif

#endif /* SERVICES_GERMPLASM_SERVICE_INCLUDE_GERMPLASM_MARKUP_H_ */
