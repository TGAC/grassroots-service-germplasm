/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <string.h>

#include "germplasm_service.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service.h"
#include "service_job.h"
#include "json_tools.h"



/*
 * STATIC DATATYPES
 */
typedef struct
{
	ServiceData gsd_base_data;
	const char *gsd_db_url_s;
} GermplasmServiceData;

static NamedParameterType GS_SEED_DETAILS = { "seed", PT_STRING };
static NamedParameterType GS_SEED_ID = { "id", PT_STRING };

static const char * const GS_STORE_REF_ID_S = "idStoreRef";
static const char * const GS_STORE_CODE_S = "StoreCode";
static const char * const GS_PLANT_ID_S = "idStoreRef";


/*
 * STATIC PROTOTYPES
 */

static GermplasmServiceData *AllocateGermplasmServiceData (void);

static void FreeGermplasmServiceData (GermplasmServiceData *data_p);

static const char *GetGermplasmServiceName (Service *service_p);

static const char *GetGermplasmServiceDesciption (Service *service_p);

static const char *GetGermplasmServiceURI (Service *service_p);

static ParameterSet *GetGermplasmServiceParameters (Service *service_p, Resource *resource_p, UserDetails *user_p);

static void ReleaseGermplasmServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunGermplasmService (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);

static ParameterSet *IsFileForGermplasmService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseGermplasmService (Service *service_p);

static ServiceMetadata *GetGermplasmServiceMetadata (Service *service_p);

static bool ConfigureGermplasmService (GermplasmServiceData *data_p);


/*
 * API FUNCTIONS
 */

ServicesArray *GetServices (UserDetails *user_p)
{
	Service *service_p = (Service *) AllocMemory (sizeof (Service));

	if (service_p)
		{
			ServicesArray *services_p = AllocateServicesArray (1);
			
			if (services_p)
				{		
					GermplasmServiceData *data_p = AllocateGermplasmServiceData ();
					
					if (data_p)
						{
							if (InitialiseService (service_p,
								GetGermplasmServiceName,
								GetGermplasmServiceDesciption,
								GetGermplasmServiceURI,
								RunGermplasmService,
								IsFileForGermplasmService,
								GetGermplasmServiceParameters,
								ReleaseGermplasmServiceParameters,
								CloseGermplasmService,
								NULL,
								true,
								SY_SYNCHRONOUS,
								(ServiceData *) data_p,
								GetGermplasmServiceMetadata))
								{
									if (ConfigureGermplasmService (data_p))
										{
											* (services_p -> sa_services_pp) = service_p;

											return services_p;
										}
								}
						}

					FreeServicesArray (services_p);
				}
				
			FreeService (service_p);
		}

	return NULL;
}


void ReleaseServices (ServicesArray *services_p)
{
	FreeServicesArray (services_p);
}





static bool CloseGermplasmService (Service *service_p)
{
	bool success_flag = true;
	GermplasmServiceData *data_p = (GermplasmServiceData *) service_p -> se_data_p;

	FreeGermplasmServiceData (data_p);
	
	return success_flag;
}

/*
 * STATIC FUNCTIONS
 */


static GermplasmServiceData *AllocateGermplasmServiceData (void)
{
	GermplasmServiceData *data_p = (GermplasmServiceData *) AllocMemory (sizeof (GermplasmServiceData));

	memset (data_p, 0, sizeof (GermplasmServiceData));

	return data_p;
}


static bool ConfigureGermplasmService (GermplasmServiceData *data_p)
{
	bool success_flag = false;
	const json_t *service_config_p = data_p -> gsd_base_data.sd_config_p;

	data_p -> gsd_db_url_s = GetJSONString (service_config_p, "database");

	if (data_p -> gsd_db_url_s)
		{
			success_flag = true;
		}

	return success_flag;
}


static void FreeGermplasmServiceData (GermplasmServiceData *data_p)
{
	FreeMemory (data_p);
}


static const char *GetGermplasmServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Germplasm Research Unit seeds service";
}


static const char *GetGermplasmServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "A service to access the Germplasm Research seeds data";
}


static const char *GetGermplasmServiceURI (Service * UNUSED_PARAM (service_p))
{
	return "https://www.jic.ac.uk/germplasm/Cereal%20Collections%20Public%20GRU.html";
}


static ParameterSet *GetGermplasmServiceParameters (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Germplasm service parameters", "The parameters used for the Germplasm service");

	if (param_set_p)
		{
			LinkedList *options_p = CreateParameterOptionsList ();

			if (options_p)
				{
					SharedType def;

					def.st_string_value_s = (char *) GS_STORE_REF_ID_S;

					if (CreateAndAddParameterOption (options_p, def, NULL, PT_STRING))
						{
							def.st_string_value_s = (char *) GS_STORE_CODE_S;

							if (CreateAndAddParameterOption (options_p, def, NULL, PT_STRING))
								{
									def.st_string_value_s = (char *) GS_PLANT_ID_S;

									if (CreateAndAddParameterOption (options_p, def, NULL, PT_STRING))
										{
											Parameter *param_p;
											ServiceData *data_p = service_p -> se_data_p;

											if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, NULL, GS_SEED_DETAILS.npt_type, GS_SEED_DETAILS.npt_name_s, "Seed data",
												"The different seed details",
												def, PL_ALL)) != NULL)
												{
													def.st_string_value_s = "";

													if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, NULL, GS_SEED_ID.npt_type, GS_SEED_ID.npt_name_s, "ID",
													  "The value to search for",
													 def, PL_ALL)) != NULL)
														{
															return param_set_p;
														}
												}
										}
								}
						}

					FreeLinkedList (options_p);
				}		/* if (options_p) */

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static void ReleaseGermplasmServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}


static ServiceJobSet *RunGermplasmService (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	GermplasmServiceData *data_p = (GermplasmServiceData *) service_p -> se_data_p;
	SharedType field;

	InitSharedType (&field);

	if (GetParameterValueFromParameterSet (param_set_p, GS_SEED_DETAILS.npt_name_s, &field, true))
		{
			SharedType id_value;

			InitSharedType (&id_value);

			if (GetParameterValueFromParameterSet (param_set_p, GS_SEED_ID.npt_name_s, &id_value, true))
				{
					/* We only have one task */
					service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Germplasm results");

					if (service_p -> se_jobs_p)
						{

						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate ServiceJobSet for %s", GetServiceName (service_p));
						}

				}
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s parameter value", GS_SEED_ID.npt_name_s);
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s parameter value", GS_SEED_DETAILS.npt_name_s);
		}

	return service_p -> se_jobs_p;
}



static ParameterSet *IsFileForGermplasmService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}



static ServiceMetadata *GetGermplasmServiceMetadata (Service *service_p)
{
	const char *term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "operation_0304";
	SchemaTerm *category_p = AllocateSchemaTerm (term_url_s, "Query and retrieval", "Search or query a data resource and retrieve entries and / or annotation.");

	if (category_p)
		{
			ServiceMetadata *metadata_p = AllocateServiceMetadata (category_p, NULL);

			if (metadata_p)
				{
					SchemaTerm *input_p;

					/* Gene ID */
					term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "data_1063";
					input_p = AllocateSchemaTerm (term_url_s, "Sequence identifier",
							"An identifier of molecular sequence(s) or entries from a molecular sequence database.");

					if (input_p)
						{
							if (AddSchemaTermToServiceMetadataInput (metadata_p, input_p))
								{
									SchemaTerm *output_p;

									term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "data_2044";
									output_p = AllocateSchemaTerm (term_url_s, "Sequence", "This concept is a placeholder of concepts for primary sequence data "
										"including raw sequences and sequence records. It should not normally be used for derivatives such as sequence alignments, "
										"motifs or profiles. One or more molecular sequences, possibly with associated annotation.");

									if (output_p)
										{
											if (AddSchemaTermToServiceMetadataOutput (metadata_p, output_p))
												{
													return metadata_p;
												}		/* if (AddSchemaTermToServiceMetadataOutput (metadata_p, output_p)) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add output term %s to service metadata", term_url_s);
													FreeSchemaTerm (output_p);
												}

										}		/* if (output_p) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate output term %s for service metadata", term_url_s);
										}

								}		/* if (AddSchemaTermToServiceMetadataInput (metadata_p, input_p)) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add input term %s to service metadata", term_url_s);
									FreeSchemaTerm (input_p);
								}

						}		/* if (input_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate input term %s for service metadata", term_url_s);
						}

				}		/* if (metadata_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate service metadata");
				}


		}		/* if (category_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate category term %s for service metadata", term_url_s);
		}

	return NULL;
}
