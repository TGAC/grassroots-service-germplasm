/*
 * germplasm_service_data.h
 *
 *  Created on: 30 May 2018
 *      Author: billy
 */

#ifndef SERVICES_GERMPLASM_SERVICE_INCLUDE_GERMPLASM_SERVICE_DATA_H_
#define SERVICES_GERMPLASM_SERVICE_INCLUDE_GERMPLASM_SERVICE_DATA_H_


#include "germplasm_service_library.h"
#include "service.h"

typedef struct
{
	ServiceData gsd_base_data;
	const char *gsd_db_url_s;
	const char *gsd_table_s;

	/** The base URL for the Seedstor api/ */
	const char *gsd_seedstor_api_s;
} GermplasmServiceData;


#ifdef __cplusplus
extern "C"
{
#endif



GERMPLASM_SERVICE_LOCAL GermplasmServiceData *AllocateGermplasmServiceData (void);

GERMPLASM_SERVICE_LOCAL void FreeGermplasmServiceData (GermplasmServiceData *data_p);

GERMPLASM_SERVICE_LOCAL bool ConfigureGermplasmService (GermplasmServiceData *data_p);


#ifdef __cplusplus
}
#endif

#endif /* SERVICES_GERMPLASM_SERVICE_INCLUDE_GERMPLASM_SERVICE_DATA_H_ */
