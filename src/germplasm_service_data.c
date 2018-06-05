/*
 * germplasm_service_data.c
 *
 *  Created on: 30 May 2018
 *      Author: billy
 */

#include <string.h>

#include "memory_allocations.h"
#include "germplasm_service_data.h"
#include "json_util.h"


GermplasmServiceData *AllocateGermplasmServiceData (void)
{
	GermplasmServiceData *data_p = (GermplasmServiceData *) AllocMemory (sizeof (GermplasmServiceData));

	memset (data_p, 0, sizeof (GermplasmServiceData));

	return data_p;
}


void FreeGermplasmServiceData (GermplasmServiceData *data_p)
{
	FreeMemory (data_p);
}


bool ConfigureGermplasmService (GermplasmServiceData *data_p)
{
	bool success_flag = false;
	const json_t *service_config_p = data_p -> gsd_base_data.sd_config_p;

	if ((data_p -> gsd_db_url_s = GetJSONString (service_config_p, "database")) != NULL)
		{
			if ((data_p -> gsd_table_s = GetJSONString (service_config_p, "table")) != NULL)
				{
					success_flag = true;
				}
		}

	if ((data_p -> gsd_seedstor_api_s = GetJSONString (service_config_p, "seedstor_api")) != NULL)
		{
			success_flag  = true;
		}

	return success_flag;
}

