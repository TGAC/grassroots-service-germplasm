# The Grassroots Germplasm Research Unit seed service
A Grassroots service for converting between seed codes for use with the [Germplasm Research Unit](https://www.jic.ac.uk/germplasm/Cereal%20Collections%20Public%20GRU.html). 




## Installation

To build this service, you need the [grassroots core](https://github.com/TGAC/grassroots-core) and [grassroots build config](https://github.com/TGAC/grassroots-build-config) installed and configured. 

The files to build the iRODS service are in the ```build/<platform>``` directory. 

### Linux

If you enter this directory 

```
cd build/linux
```

you can then build the service by typing

```
make all
```

and then 

```
make install
```

to install the service into the Grassroots system where it will be available for use immediately.



## Server Configuration

The service can be configured by files with the same names in the ```config``` directory in the Grassroots application directory, *e.g.* ```config/Germplasm Research Unit seeds service```

 * **database**: This is the SQLite database URI.
 * **table**: The name of the table within the given database that contains the seed data.
 
An example configuration file for the germplasm service which would be saved as the ```<Grassroots directory>/config/Germplasm Research Unit seeds service``` is:

~~~{.json}
{
	"database": "/home/billy/Applications/grassroots-0/grassroots/data/storeref.sqlite",
	"table": "gru_seeds"
}
~~~


