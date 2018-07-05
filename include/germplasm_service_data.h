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
#include "mongodb_tool.h"


typedef struct
{
	ServiceData gsd_base_data;

	/** The base URL for the Seedstor api/ */
	const char *gsd_seedstor_api_s;

	/** The url to order seeds */
	const char *gsd_seed_order_by_plant_id_api_s;

	/**
	 * We can cache GPS coords in a MongoDB to stop
	 * duplicated geocoding API calls for repeated calls
	 * for a given hit. It also allows us to store corrected
	 * GPS coords when the geocoding API gives the wrong
	 * result since we have only have read-only access to
	 * the seedstor DB.
	 */
	MongoTool *gsd_mongo_tool_p;

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
