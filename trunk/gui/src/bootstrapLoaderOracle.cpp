#include "bootstrapLoaderOracle.h"
#include <stdlib.h>


void reset_device(){
	
	System("bsl.py --telosb --sdev /dev/USB0 -r >> prova.txt");
	
}
