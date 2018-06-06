/*
 * country_codes.c
 *
 *  Created on: 6 Jun 2018
 *      Author: billy
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"
#include "country_codes.h"

static uint32 S_NUM_COUNTRIES = 247;

static Country s_countries_sorted_by_alpha_3_code [] =
{
	{ "Aruba", "AW", "ABW" },
	{ "Afghanistan", "AF", "AFG" },
	{ "Angola", "AO", "AGO" },
	{ "Anguilla", "AI", "AIA" },
	{ "Aland Islands", "AX", "ALA" },
	{ "Albania", "AL", "ALB" },
	{ "Andorra", "AD", "AND" },
	{ "Netherlands Antilles", "AN", "ANT" },
	{ "United Arab Emirates", "AE", "ARE" },
	{ "Argentina", "AR", "ARG" },
	{ "Armenia", "AM", "ARM" },
	{ "American Samoa", "AS", "ASM" },
	{ "Antarctica ", "AQ", "ATA" },
	{ "French Southern Territories ", "TF", "ATF" },
	{ "Antigua and Barbuda", "AG", "ATG" },
	{ "Australia", "AU", "AUS" },
	{ "Austria", "AT", "AUT" },
	{ "Azerbaijan", "AZ", "AZE" },
	{ "Burundi", "BI", "BDI" },
	{ "Belgium", "BE", "BEL" },
	{ "Benin", "BJ", "BEN" },
	{ "Burkina Faso", "BF", "BFA" },
	{ "Bangladesh", "BD", "BGD" },
	{ "Bulgaria", "BG", "BGR" },
	{ "Bahrain", "BH", "BHR" },
	{ "Bahamas", "BS", "BHS" },
	{ "Bosnia and Herzegovina", "BA", "BIH" },
	{ "Saint-Barthélemy", "BL", "BLM" },
	{ "Belarus", "BY", "BLR" },
	{ "Belize", "BZ", "BLZ" },
	{ "Bermuda", "BM", "BMU" },
	{ "Bolivia", "BO", "BOL" },
	{ "Brazil", "BR", "BRA" },
	{ "Barbados", "BB", "BRB" },
	{ "Brunei Darussalam", "BN", "BRN" },
	{ "Bhutan", "BT", "BTN" },
	{ "Bouvet Island", "BV", "BVT" },
	{ "Botswana", "BW", "BWA" },
	{ "Central African Republic", "CF", "CAF" },
	{ "Canada ", "CA", "CAN" },
	{ "Cocos (Keeling) Islands", "CC", "CCK" },
	{ "Switzerland", "CH", "CHE" },
	{ "Chile", "CL", "CHL" },
	{ "China", "CN", "CHN" },
	{ "Côte d'Ivoire", "CI", "CIV" },
	{ "Cameroon", "CM", "CMR" },
	{ "Congo,  (Kinshasa)", "CD", "COD" },
	{ "Congo (Brazzaville) ", "CG", "COG" },
	{ "Cook Islands ", "CK", "COK" },
	{ "Colombia", "CO", "COL" },
	{ "Comoros", "KM", "COM" },
	{ "Cape Verde", "CV", "CPV" },
	{ "Costa Rica", "CR", "CRI" },
	{ "Cuba", "CU", "CUB" },
	{ "Christmas Island", "CX", "CXR" },
	{ "Cayman Islands ", "KY", "CYM" },
	{ "Cyprus", "CY", "CYP" },
	{ "Czech Republic", "CZ", "CZE" },
	{ "Germany ", "DE", "DEU" },
	{ "Djibouti", "DJ", "DJI" },
	{ "Dominica", "DM", "DMA" },
	{ "Denmark", "DK", "DNK" },
	{ "Dominican Republic", "DO", "DOM" },
	{ "Algeria", "DZ", "DZA" },
	{ "Ecuador", "EC", "ECU" },
	{ "Egypt", "EG", "EGY" },
	{ "Eritrea", "ER", "ERI" },
	{ "Western Sahara ", "EH", "ESH" },
	{ "Spain", "ES", "ESP" },
	{ "Estonia", "EE", "EST" },
	{ "Ethiopia", "ET", "ETH" },
	{ "Finland", "FI", "FIN" },
	{ "Fiji", "FJ", "FJI" },
	{ "Falkland Islands (Malvinas) ", "FK", "FLK" },
	{ "France", "FR", "FRA" },
	{ "Faroe Islands", "FO", "FRO" },
	{ "Micronesia,  Federated States of", "FM", "FSM" },
	{ "Gabon", "GA", "GAB" },
	{ "United Kingdom", "GB", "GBR" },
	{ "Georgia", "GE", "GEO" },
	{ "Guernsey", "GG", "GGY" },
	{ "Ghana", "GH", "GHA" },
	{ "Gibraltar ", "GI", "GIB" },
	{ "Guinea", "GN", "GIN" },
	{ "Guadeloupe", "GP", "GLP" },
	{ "Gambia", "GM", "GMB" },
	{ "Guinea-Bissau", "GW", "GNB" },
	{ "Equatorial Guinea", "GQ", "GNQ" },
	{ "Greece", "GR", "GRC" },
	{ "Grenada", "GD", "GRD" },
	{ "Greenland", "GL", "GRL" },
	{ "Guatemala", "GT", "GTM" },
	{ "French Guiana", "GF", "GUF" },
	{ "Guam", "GU", "GUM" },
	{ "Guyana", "GY", "GUY" },
	{ "Hong Kong,  SAR China", "HK", "HKG" },
	{ "Heard and Mcdonald Islands", "HM", "HMD" },
	{ "Honduras", "HN", "HND" },
	{ "Croatia", "HR", "HRV" },
	{ "Haiti", "HT", "HTI" },
	{ "Hungary", "HU", "HUN" },
	{ "Indonesia", "ID", "IDN" },
	{ "Isle of Man ", "IM", "IMN" },
	{ "India", "IN", "IND" },
	{ "British Indian Ocean Territory", "IO", "IOT" },
	{ "Ireland", "IE", "IRL" },
	{ "Iran,  Islamic Republic of", "IR", "IRN" },
	{ "Iraq", "IQ", "IRQ" },
	{ "Iceland", "IS", "ISL" },
	{ "Israel ", "IL", "ISR" },
	{ "Italy ", "IT", "ITA" },
	{ "Jamaica", "JM", "JAM" },
	{ "Jersey", "JE", "JEY" },
	{ "Jordan", "JO", "JOR" },
	{ "Japan", "JP", "JPN" },
	{ "Kazakhstan", "KZ", "KAZ" },
	{ "Kenya", "KE", "KEN" },
	{ "Kyrgyzstan", "KG", "KGZ" },
	{ "Cambodia", "KH", "KHM" },
	{ "Kiribati", "KI", "KIR" },
	{ "Saint Kitts and Nevis", "KN", "KNA" },
	{ "Korea (South)", "KR", "KOR" },
	{ "Kuwait", "KW", "KWT" },
	{ "Lao PDR", "LA", "LAO" },
	{ "Lebanon", "LB", "LBN" },
	{ "Liberia", "LR", "LBR" },
	{ "Libya", "LY", "LBY" },
	{ "Saint Lucia", "LC", "LCA" },
	{ "Liechtenstein", "LI", "LIE" },
	{ "Sri Lanka", "LK", "LKA" },
	{ "Lesotho", "LS", "LSO" },
	{ "Lithuania", "LT", "LTU" },
	{ "Luxembourg", "LU", "LUX" },
	{ "Latvia", "LV", "LVA" },
	{ "Macao,  SAR China", "MO", "MAC" },
	{ "Saint-Martin (French part)", "MF", "MAF" },
	{ "Morocco", "MA", "MAR" },
	{ "Monaco", "MC", "MCO" },
	{ "Moldova", "MD", "MDA" },
	{ "Madagascar", "MG", "MDG" },
	{ "Maldives", "MV", "MDV" },
	{ "Mexico", "MX", "MEX" },
	{ "Marshall Islands", "MH", "MHL" },
	{ "Macedonia,  Republic of", "MK", "MKD" },
	{ "Mali", "ML", "MLI" },
	{ "Malta", "MT", "MLT" },
	{ "Myanmar", "MM", "MMR" },
	{ "Montenegro", "ME", "MNE" },
	{ "Mongolia", "MN", "MNG" },
	{ "Northern Mariana Islands", "MP", "MNP" },
	{ "Mozambique", "MZ", "MOZ" },
	{ "Mauritania", "MR", "MRT" },
	{ "Montserrat", "MS", "MSR" },
	{ "Martinique", "MQ", "MTQ" },
	{ "Mauritius", "MU", "MUS" },
	{ "Malawi", "MW", "MWI" },
	{ "Malaysia", "MY", "MYS" },
	{ "Mayotte", "YT", "MYT" },
	{ "Namibia", "NA", "NAM" },
	{ "New Caledonia", "NC", "NCL" },
	{ "Niger", "NE", "NER" },
	{ "Norfolk Island", "NF", "NFK" },
	{ "Nigeria", "NG", "NGA" },
	{ "Nicaragua", "NI", "NIC" },
	{ "Niue ", "NU", "NIU" },
	{ "Netherlands", "NL", "NLD" },
	{ "Norway", "NO", "NOR" },
	{ "Nepal", "NP", "NPL" },
	{ "Nauru", "NR", "NRU" },
	{ "New Zealand", "NZ", "NZL" },
	{ "Oman", "OM", "OMN" },
	{ "Pakistan", "PK", "PAK" },
	{ "Panama", "PA", "PAN" },
	{ "Pitcairn ", "PN", "PCN" },
	{ "Peru", "PE", "PER" },
	{ "Philippines", "PH", "PHL" },
	{ "Palau", "PW", "PLW" },
	{ "Papua New Guinea", "PG", "PNG" },
	{ "Poland", "PL", "POL" },
	{ "Puerto Rico", "PR", "PRI" },
	{ "Korea (North)", "KP", "PRK" },
	{ "Portugal", "PT", "PRT" },
	{ "Paraguay", "PY", "PRY" },
	{ "Palestinian Territory", "PS", "PSE" },
	{ "French Polynesia", "PF", "PYF" },
	{ "Qatar", "QA", "QAT" },
	{ "Réunion", "RE", "REU" },
	{ "Romania", "RO", "ROU" },
	{ "Russian Federation ", "RU", "RUS" },
	{ "Rwanda", "RW", "RWA" },
	{ "Saudi Arabia", "SA", "SAU" },
	{ "Sudan", "SD", "SDN" },
	{ "Senegal", "SN", "SEN" },
	{ "Singapore", "SG", "SGP" },
	{ "South Georgia and the South Sandwich Islands", "GS", "SGS" },
	{ "Saint Helena ", "SH", "SHN" },
	{ "Svalbard and Jan Mayen Islands ", "SJ", "SJM" },
	{ "Solomon Islands", "SB", "SLB" },
	{ "Sierra Leone", "SL", "SLE" },
	{ "El Salvador", "SV", "SLV" },
	{ "San Marino", "SM", "SMR" },
	{ "Somalia", "SO", "SOM" },
	{ "Saint Pierre and Miquelon ", "PM", "SPM" },
	{ "Serbia", "RS", "SRB" },
	{ "South Sudan", "SS", "SSD" },
	{ "Sao Tome and Principe", "ST", "STP" },
	{ "Suriname", "SR", "SUR" },
	{ "Slovakia", "SK", "SVK" },
	{ "Slovenia", "SI", "SVN" },
	{ "Sweden", "SE", "SWE" },
	{ "Swaziland", "SZ", "SWZ" },
	{ "Seychelles", "SC", "SYC" },
	{ "Syrian Arab Republic (Syria)", "SY", "SYR" },
	{ "Turks and Caicos Islands ", "TC", "TCA" },
	{ "Chad", "TD", "TCD" },
	{ "Togo", "TG", "TGO" },
	{ "Thailand", "TH", "THA" },
	{ "Tajikistan", "TJ", "TJK" },
	{ "Tokelau ", "TK", "TKL" },
	{ "Turkmenistan", "TM", "TKM" },
	{ "Timor-Leste", "TL", "TLS" },
	{ "Tonga", "TO", "TON" },
	{ "Trinidad and Tobago", "TT", "TTO" },
	{ "Tunisia", "TN", "TUN" },
	{ "Turkey", "TR", "TUR" },
	{ "Tuvalu", "TV", "TUV" },
	{ "Taiwan,  Republic of China ", "TW", "TWN" },
	{ "Tanzania,  United Republic of ", "TZ", "TZA" },
	{ "Uganda", "UG", "UGA" },
	{ "Ukraine", "UA", "UKR" },
	{ "US Minor Outlying Islands ", "UM", "UMI" },
	{ "Uruguay", "UY", "URY" },
	{ "United States of America", "US", "USA" },
	{ "Uzbekistan", "UZ", "UZB" },
	{ "Holy See (Vatican City State)", "VA", "VAT" },
	{ "Saint Vincent and Grenadines", "VC", "VCT" },
	{ "Venezuela (Bolivarian Republic)", "VE", "VEN" },
	{ "British Virgin Islands", "VG", "VGB" },
	{ "Virgin Islands,  US", "VI", "VIR" },
	{ "Viet Nam", "VN", "VNM" },
	{ "Vanuatu", "VU", "VUT" },
	{ "Wallis and Futuna Islands ", "WF", "WLF" },
	{ "Samoa", "WS", "WSM" },
	{ "Yemen", "YE", "YEM" },
	{ "South Africa", "ZA", "ZAF" },
	{ "Zambia", "ZM", "ZMB" },
	{ "Zimbabwe", "ZW", "ZWE" },
};


static int CompareCountriesByAlpha2Code (const void *v0_p, const void *v1_p);

static int CompareCountriesByAlpha3Code (const void *v0_p, const void *v1_p);

static int CompareCountriesByName (const void *v0_p, const void *v1_p);

static void PrintCountries (Country *country_p, const char *message_s);

static void PrintCountry (Country *country_p);




const Country *FindCountryByAlpha3Code (const char * const alpha_3_code_s)
{
	const Country *country_p = NULL;
	Country c;

	c.co_alpha_2_code_s = NULL;
	c.co_alpha_3_code_s = alpha_3_code_s;
	c.co_name_s = NULL;

	country_p = (const Country *) bsearch (&c, s_countries_sorted_by_alpha_3_code, S_NUM_COUNTRIES, sizeof (Country), CompareCountriesByAlpha3Code);

	return country_p;
}



int main (void)
{
	qsort (s_countries_sorted_by_alpha_3_code, S_NUM_COUNTRIES, sizeof (Country), CompareCountriesByAlpha2Code);
	PrintCountries (s_countries_sorted_by_alpha_3_code, "Alpha 2 code sort");

	qsort (s_countries_sorted_by_alpha_3_code, S_NUM_COUNTRIES, sizeof (Country), CompareCountriesByAlpha3Code);
	PrintCountries (s_countries_sorted_by_alpha_3_code, "Alpha 3 code sort");

	qsort (s_countries_sorted_by_alpha_3_code, S_NUM_COUNTRIES, sizeof (Country), CompareCountriesByName);
	PrintCountries (s_countries_sorted_by_alpha_3_code, "Name sort");

}


static int CompareCountriesByAlpha2Code (const void *v0_p, const void *v1_p)
{
	const Country * const c0_p = (const Country * const) v0_p;
	const Country * const c1_p = (const Country * const) v1_p;

	return strcmp (c0_p -> co_alpha_2_code_s, c1_p -> co_alpha_2_code_s);
}


static int CompareCountriesByAlpha3Code (const void *v0_p, const void *v1_p)
{
	const Country * const c0_p = (const Country * const) v0_p;
	const Country * const c1_p = (const Country * const) v1_p;

	return strcmp (c0_p -> co_alpha_3_code_s, c1_p -> co_alpha_3_code_s);

}


static int CompareCountriesByName (const void *v0_p, const void *v1_p)
{
	const Country * const c0_p = (const Country * const) v0_p;
	const Country * const c1_p = (const Country * const) v1_p;

	return strcmp (c0_p -> co_name_s, c1_p -> co_name_s);

}


static void PrintCountries (Country *country_p, const char *message_s)
{
	int i;

	printf ("%s\n{\n", message_s);

	for (i = S_NUM_COUNTRIES; i > 0; -- i, ++ country_p)
		{
			PrintCountry (country_p);
		}

	puts ("};\n");
}


static void PrintCountry (Country *country_p)
{
	printf ("\t{ \"%s\", \"%s\", \"%s\" },\n", country_p -> co_name_s, country_p -> co_alpha_2_code_s, country_p -> co_alpha_3_code_s);
}
