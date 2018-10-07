/*
 * germplasm_cache.h
 *
 *  Created on: 6 Jul 2018
 *      Author: billy
 */

#ifndef SERVICES_GERMPLASM_SERVICE_SRC_GERMPLASM_CACHE_H_
#define SERVICES_GERMPLASM_SERVICE_SRC_GERMPLASM_CACHE_H_

#include "germplasm_service_library.h"
#include "address.h"
#include "mongodb_tool.h"



GERMPLASM_SERVICE_LOCAL bool GetCachedGeolocationData (Address *address_p, const char *plant_id_s, const char *address_key_s, MongoTool *mongo_p);

GERMPLASM_SERVICE_LOCAL bool CacheGeolocationData (Address *address_p, const char *plant_id_s, const char *address_key_s, MongoTool *mongo_p);



#endif /* SERVICES_GERMPLASM_SERVICE_SRC_GERMPLASM_CACHE_H_ */
