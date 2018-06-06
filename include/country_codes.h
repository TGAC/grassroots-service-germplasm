/*
 * country_codes.h
 *
 *  Created on: 6 Jun 2018
 *      Author: billy
 */

#ifndef SERVICES_GERMPLASM_SERVICE_INCLUDE_COUNTRY_CODES_H_
#define SERVICES_GERMPLASM_SERVICE_INCLUDE_COUNTRY_CODES_H_

#include "germplasm_service_library.h"



typedef struct Country
{
	const char *co_name_s;
	const char *co_alpha_2_code_s;
	const char *co_alpha_3_code_s;
} Country;


#ifdef __cplusplus
extern "C"
{
#endif


GERMPLASM_SERVICE_API const Country *FindCountryByAlpha3Code (const char * const alpha_3_code_s);


#ifdef __cplusplus
}
#endif

#endif /* SERVICES_GERMPLASM_SERVICE_INCLUDE_COUNTRY_CODES_H_ */
