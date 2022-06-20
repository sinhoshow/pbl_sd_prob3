#include <stdio.h>
#include <string.h>
#include "cJSON.h"

int main()
{
	
    char *string = NULL;
    size_t index = 0;
	char *temperaturas[10] = {
		"27C-June 16 22:32:55 2022", 
		"27C-June 16 22:33:55 2022", 
		"27C-June 16 22:34:55 2022", 
		"27C-June 16 22:35:55 2022", 
		"27C-June 16 22:36:55 2022", 
		"27C-June 16 22:37:55 2022"
	};
	char *umidades[10] = {
		"27C-June 16 22:32:55 2022", 
		"27C-June 16 22:33:55 2022", 
		"27C-June 16 22:34:55 2022", 
		"27C-June 16 22:35:55 2022", 
		"27C-June 16 22:36:55 2022", 
		"27C-June 16 22:37:55 2022"
	};
	char *pressoes[10] = {
		"27C-June 16 22:32:55 2022", 
		"27C-June 16 22:33:55 2022", 
		"27C-June 16 22:34:55 2022", 
		"27C-June 16 22:35:55 2022", 
		"27C-June 16 22:36:55 2022", 
		"27C-June 16 22:37:55 2022"
	};
	char *luminosidades[10] = {
		"27C-June 16 22:32:55 2022", 
		"27C-June 16 22:33:55 2022", 
		"27C-June 16 22:34:55 2022", 
		"27C-June 16 22:35:55 2022", 
		"27C-June 16 22:36:55 2022", 
		"27C-June 16 22:37:55 2022"
	};

    cJSON *dado = cJSON_CreateObject();

	cJSON *array_temperaturas = NULL;
	cJSON *array_umidades = NULL;
	cJSON *array_pressoes = NULL; 
	cJSON *array_luminosidades = NULL;   

    array_temperaturas = cJSON_AddArrayToObject(dado, "temperaturas");
    if (array_temperaturas == NULL)
    {
        return 0;
    }

	array_umidades = cJSON_AddArrayToObject(dado, "umidades");
    if (array_umidades == NULL)
    {
        return 0;
    }

	array_pressoes = cJSON_AddArrayToObject(dado, "pressoes");
    if (array_pressoes == NULL)
    {
        return 0;
    }

	array_luminosidades = cJSON_AddArrayToObject(dado, "luminosidades");
    if (array_luminosidades == NULL)
    {
        return 0;
    }

    for (index = 0; index < (sizeof(temperaturas)/8); ++index)
    { 
		if(temperaturas[index] != NULL){
			cJSON *temp = NULL;
			temp = cJSON_CreateString(temperaturas[index]);
			cJSON_AddItemToArray(array_temperaturas, temp);
		}        
    }

	for (index = 0; index < (sizeof(pressoes)/8); ++index)
    { 
		if(pressoes[index] != NULL){
			cJSON *press = NULL;
			press = cJSON_CreateString(pressoes[index]);
			cJSON_AddItemToArray(array_pressoes, press);
		}        
    }

	for (index = 0; index < (sizeof(umidades)/8); ++index)
    { 
		if(umidades[index] != NULL){
			cJSON *umid = NULL;
			umid = cJSON_CreateString(umidades[index]);
			cJSON_AddItemToArray(array_umidades, umid);
		}        
    }

	for (index = 0; index < (sizeof(luminosidades)/8); ++index)
    { 
		if(luminosidades[index] != NULL){
			cJSON *lum = NULL;
			lum = cJSON_CreateString(luminosidades[index]);
			cJSON_AddItemToArray(array_luminosidades, lum);
		}        
    }



    string = cJSON_Print(dado);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

    printf("%s", string);
	return 0;
}