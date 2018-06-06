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

#include "jansson.h"

#include "germplasm_service.h"
#include "germplasm_service_data.h"
#include "memory_allocations.h"
#include "parameter.h"
#include "handler.h"
#include "handler_utils.h"
#include "string_utils.h"
#include "filesystem_utils.h"
#include "service.h"
#include "service_job.h"
#include "json_tools.h"
#include "sqlite_tool.h"
#include "sql_clause.h"
#include "germplasm_markup.h"


/*
 * STATIC DATATYPES
 */

static NamedParameterType GS_SEED_DETAILS = { "seed", PT_STRING };
static NamedParameterType GS_SEED_ID = { "id", PT_STRING };

static NamedParameterType GS_SEARCH = { "query", PT_STRING };

static const char * const GS_STORE_REF_ID_S = "idStoreRef";
static const char * const GS_STORE_CODE_S = "StoreCode";
static const char * const GS_PLANT_ID_S = "idPlant";
static const char * const GS_ACCESSION_S = "AccessionName";
static const char * const GS_SEARCH_S = "Search";

static const char * const GS_STORE_REF_ID_DISPLAY_NAME_S = "Store reference ID";
static const char * const GS_STORE_CODE_DISPLAY_NAME_S = "Watkins store code";
static const char * const GS_PLANT_ID_DISPLAY_NAME_S = "Plant ID";
static const char * const GS_ACCESSION_DISPLAY_NAME_S = "Accession";



/*
 * STATIC PROTOTYPES
 */



static const char *GetGermplasmServiceName (Service *service_p);

static const char *GetGermplasmServiceDesciption (Service *service_p);

static const char *GetGermplasmServiceURI (Service *service_p);

static ParameterSet *GetGermplasmServiceParametersForSimpleDB (Service *service_p, Resource *resource_p, UserDetails *user_p);

static ParameterSet *GetGermplasmServiceParametersForSeedstorAPI (Service *service_p, Resource *resource_p, UserDetails *user_p);


static void ReleaseGermplasmServiceParameters (Service *service_p, ParameterSet *params_p);

static ServiceJobSet *RunGermplasmServiceForSimpleDB (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);


static ServiceJobSet *RunGermplasmServiceForSeedstorAPI (Service *service_p, ParameterSet *param_set_p, UserDetails *user_p, ProvidersStateTable *providers_p);


static ParameterSet *IsFileForGermplasmService (Service *service_p, Resource *resource_p, Handler *handler_p);

static bool CloseGermplasmService (Service *service_p);

static ServiceMetadata *GetGermplasmServiceMetadata (Service *service_p);


static LinkedList *GetWhereClauses (const char *key_s, const char *value_s);


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


static ParameterSet *GetGermplasmServiceParametersForSeedstorAPI (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Germplasm service parameters", "The parameters used for the Germplasm service");

	if (param_set_p)
		{
			Parameter *param_p;
			ServiceData *data_p = service_p -> se_data_p;
			SharedType def;

			InitSharedType (&def);

			def.st_string_value_s = (char *) GS_SEARCH_S;

			if ((param_p = EasyCreateAndAddParameterToParameterSet (data_p, param_set_p, NULL, GS_SEARCH.npt_type, GS_SEARCH.npt_name_s, "Search", "The value to search SeedStor for", def, PL_ALL)) != NULL)
				{
					return param_set_p;
				}

			FreeParameterSet (param_set_p);
		}		/* if (param_set_p) */

	return NULL;
}


static ParameterSet *GetGermplasmServiceParametersForSimpleDB (Service *service_p, Resource * UNUSED_PARAM (resource_p), UserDetails * UNUSED_PARAM (user_p))
{
	ParameterSet *param_set_p = AllocateParameterSet ("Germplasm service parameters", "The parameters used for the Germplasm service");

	if (param_set_p)
		{
			LinkedList *options_p = CreateParameterOptionsList ();

			if (options_p)
				{
					SharedType def;

					def.st_string_value_s = (char *) GS_STORE_REF_ID_S;

					if (CreateAndAddParameterOption (options_p, def, GS_STORE_REF_ID_DISPLAY_NAME_S, PT_STRING))
						{
							def.st_string_value_s = (char *) GS_STORE_CODE_S;

							if (CreateAndAddParameterOption (options_p, def, GS_STORE_CODE_DISPLAY_NAME_S, PT_STRING))
								{
									def.st_string_value_s = (char *) GS_PLANT_ID_S;

									if (CreateAndAddParameterOption (options_p, def, GS_PLANT_ID_DISPLAY_NAME_S, PT_STRING))
										{
											def.st_string_value_s = (char *) GS_ACCESSION_S;

											if (CreateAndAddParameterOption (options_p, def, GS_ACCESSION_DISPLAY_NAME_S, PT_STRING))
												{
													Parameter *param_p;
													ServiceData *data_p = service_p -> se_data_p;

													def.st_string_value_s = (char *) GS_STORE_CODE_S;

													if ((param_p = CreateAndAddParameterToParameterSet (data_p, param_set_p, NULL, GS_SEED_DETAILS.npt_type, false, GS_SEED_DETAILS.npt_name_s, "Seed data",
														"The different seed details", options_p,
														def, NULL, NULL,  PL_ALL, NULL)) != NULL)
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


static ServiceJobSet *RunGermplasmServiceForSimpleDB (Service *service_p, ParameterSet *param_set_p, UserDetails * UNUSED_PARAM (user_p), ProvidersStateTable * UNUSED_PARAM (providers_p))
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
							SQLiteTool *tool_p = AllocateSQLiteTool (data_p -> gsd_db_url_s, SQLITE_OPEN_READONLY, data_p -> gsd_table_s);

							if (tool_p)
								{
									LinkedList *where_clauses_p = GetWhereClauses (field.st_string_value_s, id_value.st_string_value_s);

									if (where_clauses_p)
										{
											const char *fields_ss [] = { GS_STORE_REF_ID_S, GS_STORE_CODE_S, GS_PLANT_ID_S , NULL };
											ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);
											char *error_s = NULL;
											OperationStatus status = OS_STARTED;
											json_t *results_p = FindMatchingSQLiteDocuments (tool_p, where_clauses_p, fields_ss, &error_s);

											if (results_p)
												{
													json_t *row_p;
													size_t i;
													const size_t num_rows = json_array_size (results_p);
													size_t count = 0;

													json_array_foreach (results_p, i, row_p)
														{
															char *title_s = ConvertLongToString ((int64) i);

															if (title_s)
																{
																	json_t *row_resource_p = GetResourceAsJSONByParts (PROTOCOL_INLINE_S, NULL, title_s, row_p);

																	if (row_resource_p)
																		{
																			if (AddResultToServiceJob (job_p, row_resource_p))
																				{
																					++ count;
																				}
																			else
																				{
																					json_decref (row_resource_p);
																				}
																		}

																	FreeCopiedString (title_s);
																}

														}

													if (count == 0)
														{
															status = OS_FAILED;
														}
													else if (count == num_rows)
														{
															status = OS_SUCCEEDED;
														}
													else
														{
															status = OS_PARTIALLY_SUCCEEDED;
														}

													json_decref (results_p);
												}
											else
												{
													status = OS_FAILED;
													AddErrorToServiceJob (job_p, ERROR_S, error_s);

													FreeSQLiteToolErrorString (tool_p, error_s);
												}


											SetServiceJobStatus (job_p, status);

											FreeLinkedList (where_clauses_p);
										}		/* if (where_clauses_p) */

									FreeSQLiteTool (tool_p);
								}		/* if (tool_p) */

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

	if (GetParameterValueFromParameterSet (param_set_p, GS_SEED_DETAILS.npt_name_s, &search_value, true))
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
							char *api_url_s = ConcatenateVarargsStrings (data_p -> gsd_seedstor_api_s, "?", var_s, "=", search_value.st_string_value_s, NULL);

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
																					json_t *raw_result_p;
																					ServiceJob *job_p = GetServiceJobFromServiceJobSet (service_p -> se_jobs_p, 0);

																					json_array_foreach (results_p, i, raw_result_p)
																						{
																							/*
																							 * We should now have the Seedstor results,
																							 * so let's mark them up
																							 */
																							json_t *marked_up_result_p = ConvertSeedstorResultToGrassrootsMarkUp (raw_result_p, data_p);

																							if (marked_up_result_p)
																								{
																									if (AddResultToServiceJob (job_p, marked_up_result_p))
																										{

																										}

																								}		/* if (marked_up_result_p) */
																							else
																								{
																									PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, raw_result_p, "Failed to convert raw result to Markup");
																								}


																						}		/* json_array_forech (results_p, i, result_p) */

																				}		/* if (json_is_array (results_p)) */
																			else
																				{
																					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Results are not a JSON array \"%s\"", results_s);
																				}

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
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get %s parameter value", GS_SEED_DETAILS.npt_name_s);
		}

	return service_p -> se_jobs_p;
}


static ParameterSet *IsFileForGermplasmService (Service * UNUSED_PARAM (service_p), Resource * UNUSED_PARAM (resource_p), Handler * UNUSED_PARAM (handler_p))
{
	return NULL;
}



static ServiceMetadata *GetGermplasmServiceMetadata (Service * UNUSED_PARAM (service_p))
{
	const char *term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "operation_0304";
	SchemaTerm *category_p = AllocateSchemaTerm (term_url_s, "Query and retrieval", "Search or query a data resource and retrieve entries and / or annotation.");

	if (category_p)
		{
			ServiceMetadata *metadata_p = AllocateServiceMetadata (category_p, NULL);

			if (metadata_p)
				{
					SchemaTerm *input_p;

					/* ID */
					term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "data_2596";
					input_p = AllocateSchemaTerm (term_url_s, "Catalogue ID", "An identifier of a catalogue of biological resources.");

					if (input_p)
						{
							if (AddSchemaTermToServiceMetadataInput (metadata_p, input_p))
								{
									SchemaTerm *output_p;

									term_url_s = CONTEXT_PREFIX_EDAM_ONTOLOGY_S "data_2596";
									output_p = AllocateSchemaTerm (term_url_s, "Catalogue ID", "An identifier of a catalogue of biological resources.");

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



static LinkedList *GetWhereClauses (const char *key_s, const char *value_s)
{
	LinkedList *where_clauses_p = AllocateLinkedList (FreeSQLClauseNode);

	if (where_clauses_p)
		{
			SQLClauseNode *node_p = AllocateSQLClauseNode (key_s, SQLITE_OP_EQUALS_S, value_s);

			if (node_p)
				{
					LinkedListAddTail (where_clauses_p, (ListItem *) node_p);

					return where_clauses_p;
				}

			FreeLinkedList (where_clauses_p);
		}

	return NULL;
}
