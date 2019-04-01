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
#include "germplasm_service.h"

#include <string.h>

#include "germplasm_markup.h"
#include "germplasm_service_data.h"
#include "jansson.h"

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


static NamedParameterType GS_SEARCH = { "query", PT_STRING };


/*
 * STATIC PROTOTYPES
 */



static const char *GetGermplasmServiceName (Service *service_p);

static const char *GetGermplasmServiceDesciption (Service *service_p);

static const char *GetGermplasmServiceURI (Service *service_p);


static ParameterSet *GetGermplasmServiceParametersForSeedstorAPI (Service *service_p, Resource *resource_p, UserDetails *user_p);


static void ReleaseGermplasmServiceParameters (Service *service_p, ParameterSet *params_p);

static bool GetGermplasmServiceParameterTypesForNamedParameters (struct Service *service_p, const char *param_name_s, ParameterType *pt_p);

static ServiceJobSet *RunGermplasmServiceForSeedstorAPI (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);


static ParameterSet *IsFileForGermplasmService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseGermplasmService (Service *service_p);

static ServiceMetadata *GetGermplasmServiceMetadata (Service *service_p);


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
									RunGermplasmServiceForSeedstorAPI,
									IsFileForGermplasmService,
									GetGermplasmServiceParametersForSeedstorAPI,
									GetGermplasmServiceParameterTypesForNamedParameters,
									ReleaseGermplasmServiceParameters,
									CloseGermplasmService,
									NULL,
									true,
									SY_SYNCHRONOUS,
									(ServiceData *) data_p,
									GetGermplasmServiceMetadata,
									NULL))
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



static const char *GetGermplasmServiceName (Service * UNUSED_PARAM (service_p))
{
	return "Germplasm Research Unit seeds service";
}


static const char *GetGermplasmServiceDesciption (Service * UNUSED_PARAM (service_p))
{
	return "Search the Germplasm Research seeds data at the John Innes Centre.";
}


static const char *GetGermplasmServiceURI (Service * UNUSED_PARAM (service_p))
{
	return "https://www.jic.ac.uk/research-impact/germplasm-resource-unit/";
}


static ParameterSet *GetGermplasmServiceParametersForSeedstorAPI (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Germplasm service parameters", "The parameters used for the Germplasm service");

	if (param_set_p)
		{
			Parameter *param_p;
			ServiceData *data_p = service_p -> se_data_p;
			SharedType def;

			InitSharedType (&def);

			def.st_string_value_s = (char *) "";

			if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, NULL, GS_SEARCH.npt_type, GS_SEARCH.npt_name_s, "Search", "The value to search SeedStor for. This can be the Watkins ID, accession name, etc.", def, PL_ALL)) != NULL)
				{
					return param_set_p;
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static bool GetGermplasmServiceParameterTypesForNamedParameters (struct Service *service_p, const char *param_name_s, ParameterType *pt_p)
{
	bool success_flag = true;

	if (strcmp (param_name_s, GS_SEARCH.npt_name_s) == 0)
		{
			*pt_p = GS_SEARCH.npt_type;
		}
	else
		{
			success_flag = false;
		}

	return success_flag;
}



static void ReleaseGermplasmServiceParameters (Service * UNUSED_PARAM (service_p), ParameterSet *params_p)
{
	FreeParameterSet (params_p);
}

/*
 * http://seedstor.gru.jic.ac.uk/apisearch-accessionname.php?accessionname=Germany
 *
 * http://seedstor.gru.jic.ac.uk/apisearch-storecode.php?storecode=WAT1070001
 *
 * https://grassroots.tools/seedstor/apisearch-unified.php?query=10222
 */

static ServiceJobSet *RunGermplasmServiceForSeedstorAPI (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
{
	GermplasmServiceData *data_p = (GermplasmServiceData *) service_p -> se_data_p;
	SharedType search_value;

	InitSharedType (&search_value);

	if (GetParameterValueFromParameterSet (param_set_p, GS_SEARCH.npt_name_s, &search_value, true))
		{
			if (search_value.st_string_value_s)
				{
					/* We only have one task */
					service_p -> se_jobs_p = AllocateSimpleServiceJobSet (service_p, NULL, "Germplasm results");

					if (service_p -> se_jobs_p)
						{
							const char *api_page_s = "apisearch-unified.php";
							const char *var_s = "query";

							/* Generate the REST API address */
							char *api_url_s = ConcatenateVarargsStrings (data_p -> gsd_seedstor_api_s, api_page_s, "?", var_s, "=", search_value.st_string_value_s, NULL);

							if (api_url_s)
								{
									/*
									 * Now we make the call to the REST API
									 */
									CurlTool *curl_p = AllocateCurlTool (CM_MEMORY);

									if (curl_p)
										{
											if (SetUriForCurlTool (curl_p, api_url_s))
												{
													CURLcode c = RunCurlTool (curl_p);

													if (c == CURLE_OK)
														{
															const char *results_s = GetCurlToolData (curl_p);

															if (results_s)
																{
																	json_error_t err;
																	json_t *results_p = json_loads (results_s, JSON_DECODE_ANY, &err);

																	if (results_p)
																		{
																			if (json_is_array (results_p))
																				{
																					size_t i;
																					ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);
																					const size_t num_results = json_array_size (results_p);
																					size_t num_converted = 0;
																					OperationStatus status;

																					for (i = 0; i < num_results; ++ i)
																						{
																							const json_t *raw_result_p = json_array_get (results_p, i);
																							/*
																							 * We should now have the Seedstor results,
																							 * so let's mark them up
																							 */
																							json_t *marked_up_result_p = ConvertSeedstorResultToGrassrootsMarkUp (raw_result_p, data_p);

																							if (marked_up_result_p)
																								{
																									json_t *job_result_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, search_value.st_string_value_s, marked_up_result_p);

																									if (job_result_p)
																										{
																											if (AddResultToServiceJob (job_p, job_result_p))
																												{
																													++ num_converted;
																												}
																											else
																												{
																													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_result_p, "AddResultToServiceJob failed");
																													json_decref (job_result_p);
																												}

																										}		/* if (job_result_p) */
																									else
																										{
																											PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, job_result_p, "GetResourceAsJSONByParts failed for query \"%s\"", search_value.st_string_value_s);
																										}

																									json_decref (marked_up_result_p);
																								}		/* if (marked_up_result_p) */
																							else
																								{
																									PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, raw_result_p, "Failed to convert raw result to Markup");
																								}


																						}		/* json_array_forech (results_p, i, result_p) */

																					if (num_converted == num_results)
																						{
																							status = OS_SUCCEEDED;
																						}
																					else if (num_converted == 0)
																						{
																							status = OS_FAILED;
																						}
																					else
																						{
																							status = OS_PARTIALLY_SUCCEEDED;
																						}

																					SetServiceJobStatus (job_p, status);

																				}		/* if (json_is_array (results_p)) */
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Results are not a JSON array \"%s\"", results_s);
																				}

																			json_decref (results_p);
																		}		/* if (results_p) */
																	else
																		{
																			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\" to JSON, error: \"%s\"", results_s, err.source);
																		}

																}		/* if (results_s) */
															else
																{
																	PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "No results from calling \"%s\"", api_url_s);
																}

														}		/* if (c == CURLE_OK) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "RunCurlTool failed for \"%s\", error, %d ", api_url_s, c);
														}

												}		/* if (SetUriForCurlTool (curl_p, api_url_s)) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "SetUriForCurlTool failed for \"%s\"", api_url_s);
												}

											FreeCurlTool (curl_p);
										}		/* if (curl_p) */
									else
										{
											PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "AllocateCurlTool failed for \"%s\"", api_url_s);
										}

									FreeCopiedString (api_url_s);
								}		/* if (api_url_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "ConcatenateVarargsStrings failed for \"%s\", \"%s\" and \"%s\"", data_p -> gsd_seedstor_api_s, var_s, search_value.st_string_value_s);
								}

						}		/* if (service_p -> se_jobs_p) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate ServiceJobSet for %s", GetServiceName (service_p));
						}

				}		/* if (search_value.st_string_value_s) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate ServiceJobSet for %s", GetServiceName (service_p));
				}
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s parameter value", GS_SEARCH.npt_name_s);
		}

	return service_p -> se_jobs_p;
}


static ParameterSet *IsFileForGermplasmService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}





static ServiceMetadata *GetGermplasmServiceMetadata (Service *service_p)
{
	const char *term_url_s = CONTEXT_PREFIX_EXPERIMENTAL_FACTOR_ONTOLOGY_S "EFO_0000651 ";
	SchemaTerm *category_p = AllocateSchemaTerm (term_url_s, "phenotype",
			"The observable form taken by some character (or group of characters) in an individual or an organism, excluding pathology and disease. "
			"The detectable outward manifestations of a specific genotype.");

	if (category_p)
		{
			SchemaTerm *subcategory_p;

			term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "operation_0304";
			subcategory_p = AllocateSchemaTerm (term_url_s, "Query and retrieval", "Search or query a data resource and retrieve entries and / or annotation.");

			if (subcategory_p)
				{
					ServiceMetadata *metadata_p = AllocateServiceMetadata (category_p, subcategory_p);

					if (metadata_p)
						{
							SchemaTerm *input_p;

							term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "data_0968";
							input_p = AllocateSchemaTerm (term_url_s, "Keyword",
									"Boolean operators (AND, OR and NOT) and wildcard characters may be allowed. Keyword(s) or phrase(s) used (typically) for text-searching purposes.");

							if (input_p)
								{
									if (AddSchemaTermToServiceMetadataInput (metadata_p, input_p))
										{
											SchemaTerm *output_p;

											/* Place */
											term_url_s = CONTEXT_PREFIX_SCHEMA_ORG_S "Place";
											output_p = AllocateSchemaTerm (term_url_s, "Place", "Entities that have a somewhat fixed, physical extension.");

											if (output_p)
												{
													if (AddSchemaTermToServiceMetadataOutput (metadata_p, output_p))
														{
															/* Date */
															term_url_s = CONTEXT_PREFIX_SCHEMA_ORG_S "Date";
															output_p = AllocateSchemaTerm (term_url_s, "Date", "A date value in ISO 8601 date format.");

															if (output_p)
																{
																	if (AddSchemaTermToServiceMetadataOutput (metadata_p, output_p))
																		{
																			/* Phenotype */
																			term_url_s = CONTEXT_PREFIX_EXPERIMENTAL_FACTOR_ONTOLOGY_S "EFO_0000651";
																			output_p = AllocateSchemaTerm (term_url_s, "phenotype", "The observable form taken by some character (or group of characters) "
																					"in an individual or an organism, excluding pathology and disease. The detectable outward manifestations of a specific genotype.");

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

														}		/* if (AddSchemaTermToServiceMetadataOutput (metadata_p, output_p)) */
													else
														{
															PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add output term %s to service metadata", term_url_s);
															FreeSchemaTerm (output_p);
														}

												}		/* if (output_p) */
											else
												{
													PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate output term %s to service metadata", term_url_s);
													FreeSchemaTerm (output_p);
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

				}		/* if (subcategory_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate sub-category term %s for service metadata", term_url_s);
				}

		}		/* if (category_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate category term %s for service metadata", term_url_s);
		}

	return NULL;
}

