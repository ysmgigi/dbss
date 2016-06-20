#include "ocilib.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	OCI_Connection *cn = NULL;
	OCI_DirPath *dp = NULL;
	OCI_TypeInfo *tbl = NULL;

	if(!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT)) {
		printf("OCI initialize failed, %s, %s, %d\n", __FUNCTION__, __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
}

