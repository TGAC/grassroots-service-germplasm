/*
 * germplasm_markup.c
 *
 *  Created on: 30 May 2018
 *      Author: billy
 */


#include <string.h>

#include "germplasm_markup.h"

#include "germplasm_service_data.h"

#include "json_util.h"
#include "string_utils.h"
#include "address.h"
#include "geocoder_util.h"
#include "country_codes.h"


/*
 * STATIC DECLARATIONS
 */

static json_t *CreateOrGetContextNode (json_t *root_p);

static bool ConvertSeason (const json_t *src_p, json_t *dest_p, json_t *dest_context_p);

static bool ConvertCountry (const json_t *src_p, json_t *dest_p);


static bool ConvertDonorAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p);

static bool ConvertBreederAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p);

static bool ConvertAddress (const json_t *src_p, const char *dest_key_s, json_t *dest_p, json_t *dest_context_p);


static const char * const S_TOWN_S = "City";
static const char * const S_COUNTY_S = "NationalRegion";
static const char * const S_COUNTRY_S = "Country";
static const char * const S_COUNTRY_CODE_S = "CountryCode";
static const char * const S_POSTCODE_S = "PostZipCode";


/*
 * DEFINITIONS
 */


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
bool ConvertCorePassportData (const json_t *src_p, json_t *dest_p)
{
	bool success_flag = false;
	json_t *dest_context_p = CreateOrGetContextNode (dest_p);

	if (dest_context_p)
		{
			if (ConvertSeason (src_p, dest_p, dest_context_p))
				{
					if (ConvertCountry (src_p, dest_p))
						{
							if (ConvertDonorAddress (src_p, dest_p, dest_context_p))
								{
									if (ConvertBreederAddress (src_p, dest_p, dest_context_p))
										{

										}

								}
						}		/* if (ConvertSeason (src_p, dest_p)) */

				}		/* if (ConvertSeason (src_p, dest_p, dest_context_p)) */
		}

	return success_flag;
}


json_t *ConvertSeedstorResultToGrassrootsMarkUp (const json_t *src_p, GermplasmServiceData *data_p)
{
	json_t *dest_p = json_object ();

	if (dest_p)
		{
			if (!ConvertCorePassportData (src_p, dest_p))
				{
					json_decref (dest_p);
					dest_p = NULL;
				}
		}


	return dest_p;
}



/*
 * STATIC DEFINITIONS
 */

static bool ConvertSeason (const json_t *src_p, json_t *dest_p, json_t *dest_context_p)
{
	bool success_flag = false;
	const char *season_s = GetJSONString (src_p, "Sow Season");

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
	const char *country_s = GetJSONString (src_p, "Country of Origin");

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
static bool ConvertDonorAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p)
{
	return ConvertAddress (src_p, "DonorAddress", dest_p, dest_context_p);
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
static bool ConvertBreederAddress (const json_t *src_p, json_t *dest_p, json_t *dest_context_p)
{
	return ConvertAddress (src_p, "BreederAddress", dest_p, dest_context_p);
}


bool ConvertAddress (const json_t *src_p, const char *dest_key_s, json_t *dest_p, json_t *dest_context_p)
{
	bool got_location_flag = false;
	Address *address_p = NULL;
	const char *town_s = GetJSONString (src_p, S_TOWN_S);
	const char *county_s = GetJSONString (src_p, S_COUNTY_S);
	const char *country_s = GetJSONString (src_p, S_COUNTRY_S);
	const char *postcode_s = GetJSONString (src_p, S_POSTCODE_S);
	const char *country_code_s = GetJSONString (src_p, S_COUNTRY_CODE_S);

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

	address_p = AllocateAddress (town_s, county_s, country_s, postcode_s, country_code_s, NULL);

	if (address_p)
		{
			if (DetermineGPSLocationForAddress (address_p, NULL))
				{
					/*
					 * The address now has GPS coordinates so we need to add the
					 * appropriate location data to the JSON object.
					 */
					json_t *address_json_p = GetAddressAsJSON (address_p);

					if (address_json_p)
						{
							got_location_flag = true;
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

	return got_location_flag;
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


