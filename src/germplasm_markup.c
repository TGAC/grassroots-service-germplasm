/*
 * germplasm_markup.c
 *
 *  Created on: 30 May 2018
 *      Author: billy
 */


#include "germplasm_country_codes.h"
#include <string.h>

#include "germplasm_markup.h"

#include "germplasm_service_data.h"
#include "germplasm_cache.h"

#include "json_util.h"
#include "string_utils.h"
#include "address.h"
#include "geocoder_util.h"

/*
 * STATIC DECLARATIONS
 */

static bool ConvertCorePassportData (const json_t *src_p, json_t *dest_p, GermplasmServiceData *data_p);

static bool ConvertTaxonomyData (const json_t *src_p, json_t *dest_p);


static json_t *CreateOrGetContextNode (json_t *root_p);

static bool ConvertSeason (const json_t *src_p, json_t *dest_p, json_t *dest_context_p);

static bool ConvertCountry (const json_t *src_p, json_t *dest_p);


static int ConvertDonorAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p, const char *plant_id_s, GermplasmServiceData *data_p);

static int ConvertBreederAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p, const char *plant_id_s, GermplasmServiceData *data_p);

/*
 * @return 1 on address found, 0 on no address to convert, -1 on error
 */
static int ConvertAddress (const json_t *src_p, const char *dest_key_s, json_t *dest_p, json_t *dest_context_p, const char *plant_id_s, GermplasmServiceData *data_p);

static bool AddDarwinCoreDetails (const json_t *src_p, json_t *dest_p, json_t *dest_context_p);

static bool AddDarwinCoreElementByKey (const json_t *src_p, const char *src_key_s, json_t *dest_p, const char *dest_key_s);

static bool AddDarwinCoreGenusAndSpecies (const json_t *src_p, json_t *dest_p);

static bool ConvertTerm (const json_t *src_p, const char *src_key_s, json_t *dest_p, const char *dest_key_s, json_t *dest_context_p, const char *context_url_s);

static bool ConvertIds (const json_t *src_p, json_t *dest_p, GermplasmServiceData *data_p);




static const char * const S_NAME_S = "InstituteName";
static const char * const S_TOWN_S = "City";
static const char * const S_COUNTY_S = "NationalRegion";
static const char * const S_COUNTRY_S = "Country";
static const char * const S_COUNTRY_CODE_S = "CountryCode";
static const char * const S_POSTCODE_S = "PostZipCode";



/*
 * DEFINITIONS
 */


json_t *ConvertSeedstorResultToGrassrootsMarkUp (const json_t *src_p, GermplasmServiceData *data_p)
{
	json_t *dest_p = json_object ();

	if (dest_p)
		{
			if (ConvertCorePassportData (src_p, dest_p, data_p))
				{
					if (ConvertTaxonomyData (src_p, dest_p))
						{
							if (ConvertIds (src_p, dest_p, data_p))
								{
									return dest_p;
								}		/* if (ConvertIds (src_p, dest_p, data_p)) */
							else
								{
									PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, src_p, "ConvertIds failed");
								}

						}		/* if (ConvertTaxonomyData (src_p, dest_p)) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, src_p, "ConvertTaxonomyData failed");
						}

				}		/* if (ConvertCorePassportData (src_p, dest_p)) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, src_p, "ConvertCorePassportData failed");
				}

			json_decref (dest_p);
		}		/* if (dest_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate object for marked up result");
		}

	return NULL;
}


static bool ConvertPlantId (const json_t *src_p, json_t *dest_p, json_t *dest_context_p, GermplasmServiceData *data_p)
{
	bool success_flag = false;
	const char *plant_id_s = GetJSONString (src_p, "idPlant");

	if (plant_id_s)
		{
			if (json_object_set_new (dest_p,  "dwc:recordNumber", json_string (plant_id_s)) == 0)
				{
					if (data_p -> gsd_seed_order_by_plant_id_api_s)
						{
							char *url_s = ConcatenateStrings (data_p -> gsd_seed_order_by_plant_id_api_s, plant_id_s);

							if (url_s)
								{
									if (AddOntologyContextTerm (dest_context_p, "so", "http://schema.org", false))
										{
											json_t *gru_buy_seed_link_p = json_object ();

											if (gru_buy_seed_link_p)
												{
													if (json_object_set_new (gru_buy_seed_link_p, "@type", json_string ("so:checkoutPage")) == 0)
														{
															if (json_object_set_new (gru_buy_seed_link_p, "url", json_string (url_s)) == 0)
																{
																	if (json_object_set_new (dest_p, "order_link", gru_buy_seed_link_p) == 0)
																		{
																			success_flag = true;
																		}		/* if (json_object_set_new (dest_p, "order_link", gru_buy_seed_link_p) == 0) */
																	else
																		{
																			PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_p, "Failed to add \"order_link\": for gru_buy_seed_link_p");
																		}

																}		/* if (json_object_set_new (gru_buy_seed_link_p, "url", json_string (url_s)) == 0) */
															else
																{
																	PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, gru_buy_seed_link_p, "Failed to add \"@url\": \"%s\"", url_s);
																}

														}		/* if (json_object_set_new (gru_buy_seed_link_p, "@type", json_string ("so:checkoutPage")) == 0) */
													else
														{
															PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, gru_buy_seed_link_p, "Failed to add \"@type\": \"so:checkoutPage\"");
														}

													if (!success_flag)
														{
															json_decref (gru_buy_seed_link_p);
														}

												}		/* if (gru_buy_seed_link_p) */
											else
												{
													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_p, "Failed to allocate object for gru_buy_seed_link_p for \"%s\"", plant_id_s);
												}

										}		/* if (AddOntologyContextTerm (dest_context_p, "so", "http://schema.org", false)) */
									else
										{
											PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_context_p, "Failed to add \"so\": \"http://schema.org\"");
										}


									FreeCopiedString (url_s);
								}		/* if (url_static bool SetCoordinateFromJSON (Coordinate *coord_p, const json_t *values_p, const char *key_s)s) */
							else
								{
									PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_p, "Failed to concatenate \"%s\" and \"%s\"", data_p -> gsd_seed_order_by_plant_id_api_s, plant_id_s);
								}

						}		/* if (data_p -> gsd_seed_order_by_plant_id_api_s) */
					else
						{
							success_flag = true;
						}

				}		/* if (json_object_set_new (dest_p,  "dwc:recordNumber", json_string (plant_id_s)) == 0) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_p, "Failed to add \"dwc:recordNumber\": \"%s\"", plant_id_s);
				}

		}		/* if (plant_id_s) */
	else
		{
			success_flag = true;
		}

	return success_flag;
}


static bool ConvertIds (const json_t *src_p, json_t *dest_p, GermplasmServiceData *data_p)
{
	bool success_flag = false;
	json_t *dest_context_p = CreateOrGetContextNode (dest_p);

	if (dest_context_p)
		{
			const char *src_key_s = "AccessionName";

			if (ConvertTerm (src_p, src_key_s, dest_p, "accession", dest_context_p, "http://edamontology.org/data_1093"))
				{
					if (ConvertPlantId (src_p, dest_p, dest_context_p, data_p))
						{
							success_flag = true;
						}		/* if (ConvertTerm (src_p, "AccessionName", dest_p, "accession", dest_context_p, "http://edamontology.org/data_1093")) */
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\"", src_key_s);
						}
				}		/* if (ConvertTerm (src_p, "AccessionName", dest_p, "accession", dest_context_p, "http://edamontology.org/data_1093")) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to convert \"%s\"", src_key_s);
				}
		}

	return success_flag;
}


static bool ConvertTaxonomyData (const json_t *src_p, json_t *dest_p)
{
	bool success_flag = false;
	json_t *dest_context_p = CreateOrGetContextNode (dest_p);

	if (dest_context_p)
		{
			if (ConvertTerm (src_p, "Ploidy", dest_p, "ploidy", dest_context_p, "http://purl.obolibrary.org/obo/PATO_0001374"))
				{
					success_flag = true;
				}		/* if (ConvertPloidy (src_p, dest_p, dest_context_p)) */

		}

	return success_flag;
}


/*
[{
	"idPlant": "12222",
	"SubCollection": "BBSRC_Barley",
	"AccessionName": "Raisa",
	"StoreCode": "B20083",
	"Genus": "Hordeum ",
	"Species": "vulgare",
	"SubTaxa": "subsp. vulgare",
	"Pedigree": "Roland*Aramir M",
	"Synonyms": "Weibull 7287",
	"DonorName": "PBI",
	"BreederName": "Weibull PBI",
	"SampStatDesc": "Advanced\/improved cultivar",
	"Country": "Sweden",
	"SowSeason": "Spring",
	"AccYear": "1987",
	"CollSite": null,
	"TaxonCode": "Hor-vul_vu",
	"CommonName": "Barley",
	"TaxonSynonym": null,
	"Ploidy": "2",
	"Karyotype": "2n=14",
	"Genome": "HH",
	"CommonTerms": "",
	"SpeciesAuthor": "L.",
	"SubSpeciesAuthor": "",
	"TaxonComments": "",
	"phenotype": [{
		"PhenotypeParameter": "Threshed Type",
		"PhenotypeValue": "Hulled grains",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Ear Distribution",
		"PhenotypeValue": "2 Row",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Lateral Floret Type",
		"PhenotypeValue": "Normal",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Lemma Appendage",
		"PhenotypeValue": "Awned",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Straw Length",
		"PhenotypeValue": "4 - Below average",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Time to ear emergence",
		"PhenotypeValue": "6 - Slightly late",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Mildew race reaction",
		"PhenotypeValue": "No test reaction",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Brown rust race reaction",
		"PhenotypeValue": "No test reaction",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Scald race data",
		"PhenotypeValue": "No test reaction",
		"PhenotypeDescribedBy": "Historic Data"
	}, {
		"PhenotypeParameter": "Yellow rust race reaction",
		"PhenotypeValue": "No test reaction",
		"PhenotypeDescribedBy": "Historic Data"
	}]
}]
 */


/*
 * https://www.seedstor.ac.uk/search-infoaccession.php?idPlant=14120
 *
 * Core Passport
 * -------------
 *
 * Sub Collection: BBSRC_Wheat
 * Taxonomy: Triticum aestivum
 * Pedigree: Heine 7*129 Genus(=Zagreb 129)
 * Synonyms: No known synonyms
 * Donor: Inst for Research in Plant Breeding, Yugoslavia
 * Breeder: Inst of Field and Vegetable Crops
 * Sample Status: Breeders line
 * Country of Origin: Yugoslavia
 * Accession Year: 1965
 * Sow Season: Winter
 * Collection Site:
 *
 * Core Phenotypes
 * ---------------
 *
 * Juvenile erectness: 3 - Semi prostrate / Prostrate
 * Ears per plant: 5 - Some
 * Peduncle filling: 1 - Hollow, very thin walled.
 * Ear Density: 3 - Lax (3.9 -4.4 mm)
 */
static bool ConvertCorePassportData (const json_t *src_p, json_t *dest_p, GermplasmServiceData *data_p)
{
	bool success_flag = false;
	json_t *dest_context_p = CreateOrGetContextNode (dest_p);

	if (dest_context_p)
		{
			if (ConvertSeason (src_p, dest_p, dest_context_p))
				{
					if (ConvertCountry (src_p, dest_p))
						{
							const char *plant_id_s = GetJSONString (src_p, "idPlant");

							if (ConvertDonorAddress (src_p, dest_p, dest_context_p, plant_id_s, data_p) >= 0)
								{
									if (ConvertBreederAddress (src_p, dest_p, dest_context_p, plant_id_s, data_p) >= 0)
										{
											if (AddDarwinCoreDetails (src_p, dest_p, dest_context_p))
												{
													success_flag = true;
												}
										}

								}

						}		/* if (ConvertSeason (src_p, dest_p)) */

				}		/* if (ConvertSeason (src_p, dest_p, dest_context_p)) */

		}

	return success_flag;
}



/*
 * STATIC DEFINITIONS
 */

static bool ConvertSeason (const json_t *src_p, json_t *dest_p, json_t *dest_context_p)
{
	bool success_flag = false;
	const char *season_s = GetJSONString (src_p, "SowSeason");

	if (season_s)
		{
			const char *value_s = NULL;

			if (strcasecmp (season_s, "spring") == 0)
				{
					value_s = "http://purl.obolibrary.org/obo/PECO_0007037";
				}
			else if (strcasecmp (season_s, "summer") == 0)
				{
					value_s = "http://purl.obolibrary.org/obo/PECO_0007036";
				}
			else if (strcasecmp (season_s, "autumn") == 0)
				{
					value_s = "http://purl.obolibrary.org/obo/PECO_0007038";
				}
			else if (strcasecmp (season_s, "winter") == 0)
				{
					value_s = "http://purl.obolibrary.org/obo/PECO_0007035";
				}
			else
				{

				}

			if (value_s)
				{
					if (json_object_set_new (dest_p, "season", json_string (value_s)) == 0)
						{
							if (AddOntologyContextTerm (dest_context_p, "season", "http://purl.obolibrary.org/obo/PECO_0007027", true))
								{
									success_flag = true;
								}
						}
				}

		}		/* if (season_s) */
	else
		{
			success_flag = true;
		}

	return success_flag;
}


static bool ConvertCountry (const json_t *src_p, json_t *dest_p)
{
	bool success_flag = false;
	const char *country_s = GetJSONString (src_p, "Country");

	if (country_s)
		{
			json_t *country_p = json_object ();

			if (country_p)
				{
					char *type_s = ConcatenateStrings (CONTEXT_PREFIX_SCHEMA_ORG_S, "Country");

					if (type_s)
						{
							if (json_object_set_new (country_p, "@type", json_string (type_s)) == 0)
								{
									if (json_object_set_new (country_p, "name", json_string (country_s)) == 0)
										{
											if (json_object_set_new (dest_p, "country", country_p) == 0)
												{
													success_flag = true;

													/*
													 * We've added it to the destination JSON, so we can now set it to NULL
													 */
													country_p = NULL;
												}
											else
												{
												}

										}
								}

							FreeCopiedString (type_s);
						}

					/*
					 * If this is not NULL, it means it hasn't been added to the destination JSON object
					 * successfully, so we can delete it.
					 */
					if (country_p)
						{
							json_decref (country_p);
						}

				}		/* if (country_p) */
		}
	else
		{
			success_flag = true;
		}

	return success_flag;
}


/*
 	"donnorAddress": {
		"Department": "",
		"InstituteName": "Plant Breeding Institute",
		"InstituteAcronym": "IPSR-AFRC, PBI",
		"InstituteCode": "GBR005",
		"AddressLine1": "Maris Lane",
		"AddressLine2": "Trumpington",
		"AddressLine3": "",
		"City": "Cambridge",
		"NationalRegion": "",
		"PostZipCode": "CB2 2LQ",
		"Country": "UK England",
		"CountryCode": "GBE"
	}
 */
static int ConvertDonorAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p, const char *plant_id_s, GermplasmServiceData *data_p)
{
	int res = 0;
	const json_t *breeder_address_src_p = json_object_get (src_p, "donorAddress");

	if (breeder_address_src_p)
		{
			res = ConvertAddress (breeder_address_src_p, "DonorAddress", dest_p, dest_context_p, plant_id_s, data_p);
		}

	return res;
}


/*
 	"breederAddress": {
		"Department": null,
		"InstituteName": "Weibullsholm Plant Breeding Institute",
		"InstituteAcronym": null,
		"InstituteCode": "SWE006",
		"AddressLine1": null,
		"AddressLine2": null,
		"AddressLine3": null,
		"City": "Landskrona",
		"NationalRegion": "Fack",
		"PostZipCode": "S-261 20 Landskrona",
		"Country": "Sweden",
		"CountryCode": "SWE"
	}
 */
static int ConvertBreederAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p,const char *plant_id_s, GermplasmServiceData *data_p)
{
	int res = 0;
	const json_t *breeder_address_src_p = json_object_get (src_p, "breederAddress");

	if (breeder_address_src_p)
		{
			res = ConvertAddress (breeder_address_src_p, "BreederAddress", dest_p, dest_context_p, plant_id_s, data_p);
		}

	return res;
}


static int ConvertAddress (const json_t *src_p, const char *dest_key_s, json_t *dest_p, json_t *dest_context_p, const char *plant_id_s, GermplasmServiceData *data_p)
{
	int res = -1;

	Address *address_p = NULL;
	const char *name_s = GetJSONString (src_p, S_NAME_S);
	const char *town_s = GetJSONString (src_p, S_TOWN_S);
	const char *county_s = GetJSONString (src_p, S_COUNTY_S);
	const char *country_s = GetJSONString (src_p, S_COUNTRY_S);
	const char *postcode_s = GetJSONString (src_p, S_POSTCODE_S);
	const char *country_code_s = GetJSONString (src_p, S_COUNTRY_CODE_S);


	/* Do we have any valid entries? */
	if ((!IsStringEmpty (name_s)) || (!IsStringEmpty (town_s)) || (!IsStringEmpty (county_s)) || (!IsStringEmpty (country_s)) || (!IsStringEmpty (country_code_s)) || (!IsStringEmpty (postcode_s)))
		{
			/*
			 * Make sure the country code is the 2 digit ISO Alpha-2 code
			 */
			if (country_code_s)
				{
					const size_t l = strlen (country_code_s);

					switch (l)
						{
							case 3:
								{
									const Country *country_p = FindCountryByAlpha3Code (country_code_s);

									if (country_p)
										{
											country_code_s = country_p -> co_alpha_2_code_s;
										}
								}
								break;

							case 2:
								break;

							default:
								country_code_s = NULL;
								break;
						}		/* switch (l) */

				}		/* if (country_code_s) */

			address_p = AllocateAddress (name_s, NULL, town_s, county_s, country_s, postcode_s, country_code_s, NULL);

			if (address_p)
				{
					bool got_gps_flag = false;

					/*
					 * Check if we have the Geolocation cached.
					 */
					if (data_p -> gsd_mongo_tool_p)
						{
							got_gps_flag = GetCachedGeolocationData (address_p, plant_id_s, dest_key_s, data_p -> gsd_mongo_tool_p);
						}		/* if (data_p -> gsd_mongo_tool_p) */

					if (!got_gps_flag)
						{
							if (DetermineGPSLocationForAddress (address_p, NULL))
								{
									if (data_p -> gsd_mongo_tool_p)
										{
											if (!CacheGeolocationData (address_p, plant_id_s, dest_key_s, data_p -> gsd_mongo_tool_p))
												{
													PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "Failed to cache Geolocation data for \"%s\" \"%s\"", plant_id_s, dest_key_s);
												}
										}

									got_gps_flag = true;
								}
						}

					if (got_gps_flag)
						{
							/*
							 * The address now has GPS coordinates so we need to add the
							 * appropriate location data to the JSON object.
							 */
							json_t *address_json_p = GetAddressAsJSON (address_p);

							if (address_json_p)
								{
									if (json_object_set_new (dest_p, dest_key_s, address_json_p) == 0)
										{
											res = 1;
										}
									else
										{
											PrintJSONToErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, address_json_p, "Failed to add address with \"%s\"", dest_key_s);
										}
								}
							else
								{
									PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "ConvertAddressToJSON failed for \"%s\"", dest_key_s);
								}

						}		/* if (DetermineGPSLocationForAddress (address_p)) */
					else
						{
							PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "DetermineGPSLocationForAddress failed for \"%s\"", dest_key_s);
						}

					FreeAddress (address_p);
				}		/* if (address_p) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate Address for \"%s\"", dest_key_s);
				}

		}		/* if ((!IsStringEmpty (town_s)) || (!IsStringEmpty (county_s)) || (!IsStringEmpty (country_s)) || (!IsStringEmpty (country_code_s)) || (!IsStringEmpty (postcode_s))) */
	else
		{
			PrintErrors (STM_LEVEL_WARNING, __FILE__, __LINE__, "No address elements are set");
			res = 0;
		}

	return res;
}



static bool AddDarwinCoreDetails (const json_t *src_p, json_t *dest_p, json_t *dest_context_p)
{
	bool success_flag = false;

	if (AddOntologyContextTerm (dest_context_p, "dwc", "http://rs.tdwg.org/dwc/terms/", false))
		{
			if (AddDarwinCoreGenusAndSpecies (src_p, dest_p))
					{
						if (AddDarwinCoreElementByKey (src_p, "AccYear", dest_p, "dwc:year"))
							{
								if (AddDarwinCoreElementByKey (src_p, "CommonName", dest_p, "dwc:vernacularName"))
									{
										success_flag = true;
									}		/* if (AddDarwinCoreElement (src_p, "CommonName", dest_p, dest_context_p, "dwc:vernacularName", "http://rs.tdwg.org/dwc/terms/vernacularName")) */

							}		/* if (AddDarwinCoreElement (src_p, "AccYear", dest_p, dest_context_p, "dwc:year", "http://rs.tdwg.org/dwc/terms/year")) */

					}		/* if (AddDarwinCoreGenusAndSpecies (src_p, dest_p)) */

		}		/* if (AddOntologyContextTerm (dest_context_p, "dwc", "http://rs.tdwg.org/dwc/terms/", false)) */


	return success_flag;
}


static bool AddDarwinCoreGenusAndSpecies (const json_t *src_p, json_t *dest_p)
{
	bool success_flag = false;
	const char *genus_s = GetJSONString (src_p, "Genus");

	if (genus_s)
		{
			if (json_object_set_new (dest_p, "dwc:genus", json_string (genus_s)) == 0)
				{
					const char *species_s = GetJSONString (src_p, "Species");

					if (species_s)
						{
							char *genus_and_species_s = ConcatenateVarargsStrings (genus_s, " ", species_s, NULL);

							if (genus_and_species_s)
								{
									if (json_object_set_new (dest_p, "dwc:scientificName", json_string (genus_and_species_s)) == 0)
										{
											success_flag = true;
										}
									else
										{
											PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_p, "Failed to add dwc:scientificName = \"%s\"", genus_and_species_s);
										}

									FreeCopiedString (genus_and_species_s);
								}		/* if (genus_and_species_s) */
							else
								{
									PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to concatenate \"%s\" and \"%s\"", genus_s, species_s);
								}

						}		/* if (species_s) */
					else
						{
							success_flag = true;
						}

				}		/* if (json_object_set_new (src_p, "dwc:genus", json_string (genus_s)) == 0) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_p, "Failed to add dwc:genus = \"%s\"", genus_s);
				}

		}		/* if (genus_s) */
	else
		{
			success_flag = true;
		}


	return success_flag;
}


static bool AddDarwinCoreElementByKey (const json_t *src_p, const char *src_key_s, json_t *dest_p, const char *dest_key_s)
{
	bool success_flag = false;
	const char *value_s = GetJSONString (src_p, src_key_s);

	if (value_s)
		{
			if (json_object_set_new (dest_p, dest_key_s, json_string (value_s)) == 0)
				{
					success_flag = true;
				}		/* if (json_object_set_new (dest_p, key_s, json_string (value_s)) == 0) */
			else
				{
					PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, dest_p, "Failed to add \"%s\" = \"%s\"", dest_key_s, value_s);
				}
		}
	else
		{
			/* no term is set */
			success_flag = true;
		}

	return success_flag;
}


static bool ConvertTerm (const json_t *src_p, const char *src_key_s, json_t *dest_p, const char *dest_key_s, json_t *dest_context_p, const char *context_url_s)
{
	bool success_flag = false;
	const char *src_value_s = GetJSONString (src_p, src_key_s);

	if (src_value_s)
		{
			if (json_object_set_new (dest_p, dest_key_s, json_string (src_value_s)) == 0)
				{
					if (AddOntologyContextTerm (dest_context_p, dest_key_s, context_url_s, true))
						{
							success_flag = true;
						}
				}
		}		/* if (ploidy_s) */
	else
		{
			success_flag = true;
		}

	return success_flag;
}



static json_t *CreateOrGetContextNode (json_t *root_p)
{
	const char * const KEY_S = "@context";
	json_t *context_p = json_object_get (root_p, KEY_S);

	if (!context_p)
		{
			context_p = json_object ();

			if (context_p)
				{
					if (json_object_set_new (root_p, KEY_S, context_p) != 0)
						{
							json_decref (context_p);
							context_p = NULL;
						}
				}
		}

	return context_p;
}


