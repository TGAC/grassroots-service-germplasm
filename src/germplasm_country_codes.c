/*
 * country_codes.c
 *
 *  Created on: 6 Jun 2018
 *      Author: billy
 */

#include <germplasm_country_codes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedefs.h"


typedef struct GRUCountryCodeMap
{
	const char *gccm_gru_code_s;
	const char *gccm_iso_code_s;
} GRUCountryCodeMap;


static uint32 S_GRU_COUNTRY_CODES_MAP_SIZE  = 37;


static GRUCountryCodeMap s_gru_country_codes_map [] =
{
	{"*", NULL },
	{ "ATB", "ATA" },
	{ "ATN", "ATA" },
	{ "BUR", "MMR" },
	{ "BYS", "BLR" },
	{ "CRT", "GRC" },
	{ "CRY", "ESP" },
	{ "CSK", "CZE" },
	{ "CTE", "KIR" },
	{ "DDR", "DEU" },
	{ "GBE", "GBR" },
	{ "GBI", "GBR" },
	{ "GBS", "GBR" },
	{ "GBW", "GBR" },
	{ "GEL", "KIR" },
	{ "GEM", "DEU" },
	{ "GER", "DEU" },
	{ "HVO", "BFO" },
	{ "JTN", "UMI" },
	{ "MAN", "CHN" },
	{ "MID", "UMI" },
	{ "NHB", "VUT" },
	{ "NTZ", "SAU" },
	{ "PAL", "PSE" },
	{ "PCI", "MHL" },
	{ "PCZ", "PAN" },
	{ "PUS", "UMI" },
	{ "SUN", "RUS" },
	{ "TIB", "CHN" },
	{ "TMP", "TLS" },
	{ "UAR", "ARE" },
	{ "USR", "RUS" },
	{ "WAK", "UMI" },
	{ "YMD", "YEM" },
	{ "YUG", "SCG" },
	{ "ZAR", "COG" },
	{ "ZIM", "ZWE" }
};



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


static Country s_countries_sorted_by_name [] =
{
	{ "Afghanistan", "AF", "AFG" },
	{ "Aland Islands", "AX", "ALA" },
	{ "Albania", "AL", "ALB" },
	{ "Algeria", "DZ", "DZA" },
	{ "American Samoa", "AS", "ASM" },
	{ "Andorra", "AD", "AND" },
	{ "Angola", "AO", "AGO" },
	{ "Anguilla", "AI", "AIA" },
	{ "Antarctica ", "AQ", "ATA" },
	{ "Antigua and Barbuda", "AG", "ATG" },
	{ "Argentina", "AR", "ARG" },
	{ "Armenia", "AM", "ARM" },
	{ "Aruba", "AW", "ABW" },
	{ "Australia", "AU", "AUS" },
	{ "Austria", "AT", "AUT" },
	{ "Azerbaijan", "AZ", "AZE" },
	{ "Bahamas", "BS", "BHS" },
	{ "Bahrain", "BH", "BHR" },
	{ "Bangladesh", "BD", "BGD" },
	{ "Barbados", "BB", "BRB" },
	{ "Belarus", "BY", "BLR" },
	{ "Belgium", "BE", "BEL" },
	{ "Belize", "BZ", "BLZ" },
	{ "Benin", "BJ", "BEN" },
	{ "Bermuda", "BM", "BMU" },
	{ "Bhutan", "BT", "BTN" },
	{ "Bolivia", "BO", "BOL" },
	{ "Bosnia and Herzegovina", "BA", "BIH" },
	{ "Botswana", "BW", "BWA" },
	{ "Bouvet Island", "BV", "BVT" },
	{ "Brazil", "BR", "BRA" },
	{ "British Indian Ocean Territory", "IO", "IOT" },
	{ "British Virgin Islands", "VG", "VGB" },
	{ "Brunei Darussalam", "BN", "BRN" },
	{ "Bulgaria", "BG", "BGR" },
	{ "Burkina Faso", "BF", "BFA" },
	{ "Burundi", "BI", "BDI" },
	{ "Cambodia", "KH", "KHM" },
	{ "Cameroon", "CM", "CMR" },
	{ "Canada ", "CA", "CAN" },
	{ "Cape Verde", "CV", "CPV" },
	{ "Cayman Islands ", "KY", "CYM" },
	{ "Central African Republic", "CF", "CAF" },
	{ "Chad", "TD", "TCD" },
	{ "Chile", "CL", "CHL" },
	{ "China", "CN", "CHN" },
	{ "Christmas Island", "CX", "CXR" },
	{ "Cocos (Keeling) Islands", "CC", "CCK" },
	{ "Colombia", "CO", "COL" },
	{ "Comoros", "KM", "COM" },
	{ "Congo (Brazzaville) ", "CG", "COG" },
	{ "Congo,  (Kinshasa)", "CD", "COD" },
	{ "Cook Islands ", "CK", "COK" },
	{ "Costa Rica", "CR", "CRI" },
	{ "Croatia", "HR", "HRV" },
	{ "Cuba", "CU", "CUB" },
	{ "Cyprus", "CY", "CYP" },
	{ "Czech Republic", "CZ", "CZE" },
	{ "Côte d'Ivoire", "CI", "CIV" },
	{ "Denmark", "DK", "DNK" },
	{ "Djibouti", "DJ", "DJI" },
	{ "Dominica", "DM", "DMA" },
	{ "Dominican Republic", "DO", "DOM" },
	{ "Ecuador", "EC", "ECU" },
	{ "Egypt", "EG", "EGY" },
	{ "El Salvador", "SV", "SLV" },
	{ "Equatorial Guinea", "GQ", "GNQ" },
	{ "Eritrea", "ER", "ERI" },
	{ "Estonia", "EE", "EST" },
	{ "Ethiopia", "ET", "ETH" },
	{ "Falkland Islands (Malvinas) ", "FK", "FLK" },
	{ "Faroe Islands", "FO", "FRO" },
	{ "Fiji", "FJ", "FJI" },
	{ "Finland", "FI", "FIN" },
	{ "France", "FR", "FRA" },
	{ "French Guiana", "GF", "GUF" },
	{ "French Polynesia", "PF", "PYF" },
	{ "French Southern Territories ", "TF", "ATF" },
	{ "Gabon", "GA", "GAB" },
	{ "Gambia", "GM", "GMB" },
	{ "Georgia", "GE", "GEO" },
	{ "Germany ", "DE", "DEU" },
	{ "Ghana", "GH", "GHA" },
	{ "Gibraltar ", "GI", "GIB" },
	{ "Greece", "GR", "GRC" },
	{ "Greenland", "GL", "GRL" },
	{ "Grenada", "GD", "GRD" },
	{ "Guadeloupe", "GP", "GLP" },
	{ "Guam", "GU", "GUM" },
	{ "Guatemala", "GT", "GTM" },
	{ "Guernsey", "GG", "GGY" },
	{ "Guinea", "GN", "GIN" },
	{ "Guinea-Bissau", "GW", "GNB" },
	{ "Guyana", "GY", "GUY" },
	{ "Haiti", "HT", "HTI" },
	{ "Heard and Mcdonald Islands", "HM", "HMD" },
	{ "Holy See (Vatican City State)", "VA", "VAT" },
	{ "Honduras", "HN", "HND" },
	{ "Hong Kong,  SAR China", "HK", "HKG" },
	{ "Hungary", "HU", "HUN" },
	{ "Iceland", "IS", "ISL" },
	{ "India", "IN", "IND" },
	{ "Indonesia", "ID", "IDN" },
	{ "Iran,  Islamic Republic of", "IR", "IRN" },
	{ "Iraq", "IQ", "IRQ" },
	{ "Ireland", "IE", "IRL" },
	{ "Isle of Man ", "IM", "IMN" },
	{ "Israel ", "IL", "ISR" },
	{ "Italy ", "IT", "ITA" },
	{ "Jamaica", "JM", "JAM" },
	{ "Japan", "JP", "JPN" },
	{ "Jersey", "JE", "JEY" },
	{ "Jordan", "JO", "JOR" },
	{ "Kazakhstan", "KZ", "KAZ" },
	{ "Kenya", "KE", "KEN" },
	{ "Kiribati", "KI", "KIR" },
	{ "Korea (North)", "KP", "PRK" },
	{ "Korea (South)", "KR", "KOR" },
	{ "Kuwait", "KW", "KWT" },
	{ "Kyrgyzstan", "KG", "KGZ" },
	{ "Lao PDR", "LA", "LAO" },
	{ "Latvia", "LV", "LVA" },
	{ "Lebanon", "LB", "LBN" },
	{ "Lesotho", "LS", "LSO" },
	{ "Liberia", "LR", "LBR" },
	{ "Libya", "LY", "LBY" },
	{ "Liechtenstein", "LI", "LIE" },
	{ "Lithuania", "LT", "LTU" },
	{ "Luxembourg", "LU", "LUX" },
	{ "Macao,  SAR China", "MO", "MAC" },
	{ "Macedonia,  Republic of", "MK", "MKD" },
	{ "Madagascar", "MG", "MDG" },
	{ "Malawi", "MW", "MWI" },
	{ "Malaysia", "MY", "MYS" },
	{ "Maldives", "MV", "MDV" },
	{ "Mali", "ML", "MLI" },
	{ "Malta", "MT", "MLT" },
	{ "Marshall Islands", "MH", "MHL" },
	{ "Martinique", "MQ", "MTQ" },
	{ "Mauritania", "MR", "MRT" },
	{ "Mauritius", "MU", "MUS" },
	{ "Mayotte", "YT", "MYT" },
	{ "Mexico", "MX", "MEX" },
	{ "Micronesia,  Federated States of", "FM", "FSM" },
	{ "Moldova", "MD", "MDA" },
	{ "Monaco", "MC", "MCO" },
	{ "Mongolia", "MN", "MNG" },
	{ "Montenegro", "ME", "MNE" },
	{ "Montserrat", "MS", "MSR" },
	{ "Morocco", "MA", "MAR" },
	{ "Mozambique", "MZ", "MOZ" },
	{ "Myanmar", "MM", "MMR" },
	{ "Namibia", "NA", "NAM" },
	{ "Nauru", "NR", "NRU" },
	{ "Nepal", "NP", "NPL" },
	{ "Netherlands", "NL", "NLD" },
	{ "Netherlands Antilles", "AN", "ANT" },
	{ "New Caledonia", "NC", "NCL" },
	{ "New Zealand", "NZ", "NZL" },
	{ "Nicaragua", "NI", "NIC" },
	{ "Niger", "NE", "NER" },
	{ "Nigeria", "NG", "NGA" },
	{ "Niue ", "NU", "NIU" },
	{ "Norfolk Island", "NF", "NFK" },
	{ "Northern Mariana Islands", "MP", "MNP" },
	{ "Norway", "NO", "NOR" },
	{ "Oman", "OM", "OMN" },
	{ "Pakistan", "PK", "PAK" },
	{ "Palau", "PW", "PLW" },
	{ "Palestinian Territory", "PS", "PSE" },
	{ "Panama", "PA", "PAN" },
	{ "Papua New Guinea", "PG", "PNG" },
	{ "Paraguay", "PY", "PRY" },
	{ "Peru", "PE", "PER" },
	{ "Philippines", "PH", "PHL" },
	{ "Pitcairn ", "PN", "PCN" },
	{ "Poland", "PL", "POL" },
	{ "Portugal", "PT", "PRT" },
	{ "Puerto Rico", "PR", "PRI" },
	{ "Qatar", "QA", "QAT" },
	{ "Romania", "RO", "ROU" },
	{ "Russian Federation ", "RU", "RUS" },
	{ "Rwanda", "RW", "RWA" },
	{ "Réunion", "RE", "REU" },
	{ "Saint Helena ", "SH", "SHN" },
	{ "Saint Kitts and Nevis", "KN", "KNA" },
	{ "Saint Lucia", "LC", "LCA" },
	{ "Saint Pierre and Miquelon ", "PM", "SPM" },
	{ "Saint Vincent and Grenadines", "VC", "VCT" },
	{ "Saint-Barthélemy", "BL", "BLM" },
	{ "Saint-Martin (French part)", "MF", "MAF" },
	{ "Samoa", "WS", "WSM" },
	{ "San Marino", "SM", "SMR" },
	{ "Sao Tome and Principe", "ST", "STP" },
	{ "Saudi Arabia", "SA", "SAU" },
	{ "Senegal", "SN", "SEN" },
	{ "Serbia", "RS", "SRB" },
	{ "Seychelles", "SC", "SYC" },
	{ "Sierra Leone", "SL", "SLE" },
	{ "Singapore", "SG", "SGP" },
	{ "Slovakia", "SK", "SVK" },
	{ "Slovenia", "SI", "SVN" },
	{ "Solomon Islands", "SB", "SLB" },
	{ "Somalia", "SO", "SOM" },
	{ "South Africa", "ZA", "ZAF" },
	{ "South Georgia and the South Sandwich Islands", "GS", "SGS" },
	{ "South Sudan", "SS", "SSD" },
	{ "Spain", "ES", "ESP" },
	{ "Sri Lanka", "LK", "LKA" },
	{ "Sudan", "SD", "SDN" },
	{ "Suriname", "SR", "SUR" },
	{ "Svalbard and Jan Mayen Islands ", "SJ", "SJM" },
	{ "Swaziland", "SZ", "SWZ" },
	{ "Sweden", "SE", "SWE" },
	{ "Switzerland", "CH", "CHE" },
	{ "Syrian Arab Republic (Syria)", "SY", "SYR" },
	{ "Taiwan,  Republic of China ", "TW", "TWN" },
	{ "Tajikistan", "TJ", "TJK" },
	{ "Tanzania,  United Republic of ", "TZ", "TZA" },
	{ "Thailand", "TH", "THA" },
	{ "Timor-Leste", "TL", "TLS" },
	{ "Togo", "TG", "TGO" },
	{ "Tokelau ", "TK", "TKL" },
	{ "Tonga", "TO", "TON" },
	{ "Trinidad and Tobago", "TT", "TTO" },
	{ "Tunisia", "TN", "TUN" },
	{ "Turkey", "TR", "TUR" },
	{ "Turkmenistan", "TM", "TKM" },
	{ "Turks and Caicos Islands ", "TC", "TCA" },
	{ "Tuvalu", "TV", "TUV" },
	{ "US Minor Outlying Islands ", "UM", "UMI" },
	{ "Uganda", "UG", "UGA" },
	{ "Ukraine", "UA", "UKR" },
	{ "United Arab Emirates", "AE", "ARE" },
	{ "United Kingdom", "GB", "GBR" },
	{ "United States of America", "US", "USA" },
	{ "Uruguay", "UY", "URY" },
	{ "Uzbekistan", "UZ", "UZB" },
	{ "Vanuatu", "VU", "VUT" },
	{ "Venezuela (Bolivarian Republic)", "VE", "VEN" },
	{ "Viet Nam", "VN", "VNM" },
	{ "Virgin Islands,  US", "VI", "VIR" },
	{ "Wallis and Futuna Islands ", "WF", "WLF" },
	{ "Western Sahara ", "EH", "ESH" },
	{ "Yemen", "YE", "YEM" },
	{ "Zambia", "ZM", "ZMB" },
	{ "Zimbabwe", "ZW", "ZWE" },
};



static int CompareCountriesByAlpha2Code (const void *v0_p, const void *v1_p);

static int CompareCountriesByAlpha3Code (const void *v0_p, const void *v1_p);

static int CompareCountriesByName (const void *v0_p, const void *v1_p);

static int CompareGRUCountriesByAlpha3Code (const void *v0_p, const void *v1_p);

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

	if (!country_p)
		{
			/*
			 * SeedStor stores the following non-standard 3 letter codes
			 *
			 * "*" = "Country Unknown"
			 * "ATB" = "British Antarctic Territory"
			 * "ATN" = "Dronning Maud Land"
			 * "BUR" = "Burma / Myanmar"
			 * "BYS" = "Byelorussian SSR"
			 * "CRT" = "Crete"
			 * "CRY" = "Canary Islands"
			 * "CSK" = "Czechoslovakia"
			 * "CTE" = "Canton and Enderbury Islands"
			 * "DDR" = "East Germany"
			 * "GBE" = "UK England"
			 * "GBI" = "UK Northern Ireland"
			 * "GBS" = "UK Scotland"
			 * "GBW" = "UK Wales"
			 * "GEL" = "Gilbert Islands"
			 * "GEM" = "Germany"
			 * "GER" = "Germany (Pre-War)"
			 * "HVO" = "Upper Volta / Burkina Faso"
			 * "JTN" = "Johnston Island"
			 * "MAN" = "Manchuria"
			 * "MID" = "Midway Islands"
			 * "NHB" = "New Hebrides"
			 * "NTZ" = "Neutral Zone"
			 * "PAL" = "Palestine"
			 * "PCI" = "Pacific Islands"
			 * "PCZ" = "Panama Canal Zone"
			 * "PUS" = "U.S. Miscellaneous Pacific Islands"
			 * "SUN" = "USSR"
		   * "TIB" = "Tibet"
			 * "TMP" = "East Timor"
			 * "UAR" = "United Arab Republic"
			 * "USR" = "Former Soviet Union"
			 * "WAK" = "Wake Island"
			 * "YMD" = "Yemen (Democratic)"
			 * "YUG" = "Yugoslavia"
			 * "ZAR" = "Zaire"
			 * "ZIM" = "Zimbabwe"
			 */
			GRUCountryCodeMap gc;
			const GRUCountryCodeMap *gru_country_p = NULL;

			gc.gccm_gru_code_s = alpha_3_code_s;
			gc.gccm_iso_code_s = NULL;

			gru_country_p = (const GRUCountryCodeMap *) bsearch (&gc, s_gru_country_codes_map, S_GRU_COUNTRY_CODES_MAP_SIZE, sizeof (GRUCountryCodeMap), CompareGRUCountriesByAlpha3Code);

			if (gru_country_p)
				{
					if (gru_country_p -> gccm_iso_code_s)
						{
							c.co_alpha_3_code_s = gru_country_p -> gccm_iso_code_s;

							country_p = (const Country *) bsearch (&c, s_countries_sorted_by_alpha_3_code, S_NUM_COUNTRIES, sizeof (Country), CompareCountriesByAlpha3Code);
						}
				}
		}

	return country_p;
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



static int CompareGRUCountriesByAlpha3Code (const void *v0_p, const void *v1_p)
{
	const GRUCountryCodeMap * const c0_p = (const GRUCountryCodeMap * const) v0_p;
	const GRUCountryCodeMap * const c1_p = (const GRUCountryCodeMap * const) v1_p;

	return strcmp (c0_p -> gccm_gru_code_s, c1_p -> gccm_gru_code_s);
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
