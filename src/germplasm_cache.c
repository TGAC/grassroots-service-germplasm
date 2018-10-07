/*
 * germplasm_cache.c
 *
 *  Created on: 6 Jul 2018
 *      Author: billy
 */


#include "germplasm_cache.h"

#include "streams.h"
#include "json_util.h"
#include "string_utils.h"


static const char * const S_DB_KEY_S = "key";


static bool LoadCoordinateFromJSON (Coordinate **coord_pp, const json_t *values_p, const char *key_s);

static bool SaveCoordinateToJSON (Coordinate *coord_p, json_t *values_p, const char *key_s);

static char *MakeCachingKey (const char *plant_id_s, const char *address_key_s);




bool CacheGeolocationData (Address *address_p, const char *plant_id_s, const char *address_key_s, MongoTool *mongo_p)
{
	bool success_flag = false;
	char *key_value_s = MakeCachingKey (plant_id_s, address_key_s);

	if (key_value_s)
		{
			json_t *data_to_cache_p = json_object ();

			if (data_to_cache_p)
				{
					if (json_object_set_new (data_to_cache_p, S_DB_KEY_S, json_string (key_value_s)) == 0)
						{
							if (SaveCoordinateToJSON (address_p -> ad_gps_centre_p, data_to_cache_p, AD_LOCATION_S))
								{
									if (SaveCoordinateToJSON (address_p -> ad_gps_north_east_p, data_to_cache_p, AD_NORTH_EAST_LOCATION_S))
										{
											if (SaveCoordinateToJSON (address_p -> ad_gps_south_west_p, data_to_cache_p, AD_SOUTH_WEST_LOCATION_S))
												{
													if (EasyInsertOrUpdateMongoData (mongo_p, data_to_cache_p, S_DB_KEY_S))
														{
															success_flag = true;
														}
												}
										}
								}

						}		/* if (json_object_set_new (data_to_cache_p, S_DB_KEY_S, json_string (S_DB_KEY_S)) == 0) */

					json_decref (data_to_cache_p);
				}		/* if (data_to_cache_p) */

			FreeCopiedString (key_value_s);
		}		/* if (key_value_s) */

	return success_flag;
}


bool GetCachedGeolocationData (Address *address_p, const char *plant_id_s, const char *address_key_s, MongoTool *mongo_p)
{
	bool success_flag = false;
	json_t *query_p = json_object ();

	if (query_p)
		{
			char *key_value_s = MakeCachingKey (plant_id_s, address_key_s);

			if (key_value_s)
				{
					if (json_object_set_new (query_p, S_DB_KEY_S, json_string (key_value_s)) == 0)
						{
							/*
							 * check for cached gps coords for this entry
							 */
							if (FindMatchingMongoDocumentsByJSON (mongo_p, query_p, NULL, NULL))
								{
									json_t *docs_p = GetAllExistingMongoResultsAsJSON (mongo_p);

									if (docs_p)
										{
											const json_t *data_p = NULL;

											if (json_is_array (docs_p))
												{
													const size_t num_docs = json_array_size (docs_p);

													if (num_docs == 1)
														{
															data_p = json_array_get (docs_p, 0);
														}
													else
														{
														}

												}		/* if (json_is_array (docs_p)) */
											else if (json_is_object (docs_p))
												{
													data_p = docs_p;
												}		/* else if (json_is_object (docs_p)) */
											else
												{
													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, docs_p, "Not valid object for holding GPS data");
												}


											if (data_p)
												{
													if (LoadCoordinateFromJSON (& (address_p -> ad_gps_centre_p), data_p, AD_LOCATION_S))
														{
															if (LoadCoordinateFromJSON (& (address_p -> ad_gps_north_east_p), data_p, AD_NORTH_EAST_LOCATION_S))
																{
																	if (LoadCoordinateFromJSON (& (address_p -> ad_gps_south_west_p), data_p, AD_SOUTH_WEST_LOCATION_S))
																		{
																			success_flag = true;
																		}
																}
														}

												}		/* if (data_p) */

											json_decref (docs_p);
										}		/* if (docs_p) */

								}		/* if (FindMatchingMongoDocumentsByJSON (data_p -> gsd_mongo_tool_p, query_p, NULL)) */

						}		/* if (json_object_set_new (query_p, DB_KEY_S, json_string (plant_id_s)) == 0) */

					FreeCopiedString (key_value_s);
				}		/* if (key_value_s) */


			json_decref (query_p);
		}		/* if (query_p) */

	return success_flag;
}



static char *MakeCachingKey (const char *plant_id_s, const char *address_key_s)
{
	char *composite_key_s = ConcatenateVarargsStrings (plant_id_s, " - ", address_key_s, NULL);

	if (composite_key_s)
		{
			return composite_key_s;
		}
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to concatenate \"%s\", \" - \" & \"%s\"", plant_id_s, address_key_s);
		}

	return NULL;
}


static bool LoadCoordinateFromJSON (Coordinate **coord_pp, const json_t *values_p, const char *key_s)
{
	bool success_flag = false;
	const json_t *location_p = json_object_get (values_p, key_s);

	if (location_p)
		{
			Coordinate coord;

			if (SetCoordinateFromJSON (&coord, location_p))
				{
					if (*coord_pp)
						{
							(*coord_pp) -> co_x = coord.co_x;
							(*coord_pp) -> co_y = coord.co_y;

							success_flag = true;
						}
					else
						{
							*coord_pp = AllocateCoordinate (coord.co_x, coord.co_y);

							if (*coord_pp)
								{
									success_flag = true;
								}		/* if (*coord_pp) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate coordinate for cached db entry \"%s\"", key_s);
								}
						}

				}		/* if (SetCoordinateFromJSON (&coord, location_p)) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, location_p, "SetCoordinateFromJSON failed for \"%s\"", key_s);
				}

		}		/* if (location_p) */
	else
		{
			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, values_p, "Failed to find \"%s\"", key_s);
		}

	return success_flag;
}


static bool SaveCoordinateToJSON (Coordinate *coord_p, json_t *values_p, const char *key_s)
{
	bool success_flag = false;
	json_t *location_p = GetCoordinateAsJSON (coord_p);

	if (location_p)
		{
			if (json_object_set_new (values_p, key_s, location_p) == 0)
				{
					success_flag = true;
				}
			else
				{
					json_decref (location_p);
				}

		}		/* if (location_p) */


	return success_flag;
}

