#include <wiringPi.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
 
#include "ads1115_rpi.h"
#include <mosquitto.h>
#include "cJSON.h"

//#include <MQTTClient.h>


//USE WIRINGPI PIN NUMBERS
#define LCD_RS  6               //Register select pin
#define LCD_E   31               //Enable Pin
#define LCD_D4  26               //Data pin 4
#define LCD_D5  27               //Data pin 5
#define LCD_D6  28               //Data pin 6
#define LCD_D7  29

#define BUTTON_0 21
#define BUTTON_1 24
#define BUTTON_2 25

//Data pin 7
#define MAXTIMINGS 85
#define DHTPIN 4
/* Caso desejar utilizar outro broker MQTT, substitua o endereco abaixo */
#define MQTT_ADDRESS   "tcp://iot.eclipse.org"
/* Substitua este por um ID unico em sua aplicacao */
#define CLIENTID       "MQTTCClientID"
//MQTTClient client;

#define MQTT_PUBLISH_TOPIC     "tp_03_g04/#"
#define MQTT_SUBSCRIBE_TOPIC   "tp_03_g04/intervalo"

int lcd;
int dht11_dat[5] = {0, 0, 0, 0, 0};
int intervalo_medicao = 1000; //em s
int temperatura = 0;
int umidade = 0;
int pressao = 0;
int luminosidade = 0;

int indice_temperaturas = 0;
char temperaturas[11][100];

int indice_luminosidades = 0;
char luminosidades[11][100];

int indice_pressoes = 0;
char pressoes[11][100];

int indice_umidades = 0;
char umidades[11][100];

char char_intervalo_medicao[10];
int rc;
struct mosquitto * mosq;

void connect_broker(){    
                
        mosquitto_username_pw_set(mosq, "aluno", "aluno*123");  
        
        rc = mosquitto_connect(mosq, "10.0.0.101", 1883, 60);
        if(rc != 0){
                printf("Client could not connect to broker! Error Code: %d\n", rc);
                mosquitto_destroy(mosq);
        }
        printf("We are now connected to the broker!\n");

}

void desconect_mosquitto(){
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
}

void publish_mosquitto(char *topic, int payload_size ,char *payload){
        printf("%s", payload);
        mosq = mosquitto_new("publisher-test", true, NULL);
        connect_broker();
        mosquitto_publish(mosq, NULL, topic, payload_size, payload, 0, false);
        desconect_mosquitto();        
}


void on_connect(struct mosquitto *mosq, void *obj, int rc) {
        printf("ID: %d\n", * (int *) obj);
        if(rc){
                printf("Error with result code: %d\n", rc);
                exit(-1);
        }
        mosquitto_subscribe(mosq, NULL, MQTT_SUBSCRIBE_TOPIC, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
        printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
        intervalo_medicao = atoi(msg->payload);
}

void *subscribe_mosquitto(){
        int id=12;
        mosq = mosquitto_new("subscribe-test", true, &id);       
        
        mosquitto_connect_callback_set(mosq, on_connect);
        mosquitto_message_callback_set(mosq, on_message);
        connect_broker();

        mosquitto_loop_start(mosq);
        printf("Press Enter to quit...\n");
        getchar();
        mosquitto_loop_stop(mosq, true);
        desconect_mosquitto();
}

void gravarTemperatura(){
        FILE *arq;
        char date_now[80];
        time_t now;
        time(&now);
        struct tm * timeinfo;
        timeinfo = localtime (&now);
        arq = fopen("historicos/temperaturas.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        strftime (date_now,80,"%d/%m/%y %X%n",timeinfo);
        fprintf(arq, "%dC-%s", temperatura, date_now);
        fclose(arq);
}

void gravarUmidade(){
        FILE *arq;
        char date_now[80];
        time_t now;
        time(&now);
        struct tm * timeinfo;
        timeinfo = localtime (&now);
        arq = fopen("historicos/umidades.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        strftime (date_now,80,"%d/%m/%y %X%n",timeinfo);
        fprintf(arq, "%d%%-%s", umidade, date_now);
        fclose(arq);
}
void gravarPressao(){
        FILE *arq;
        char date_now[80];
        time_t now;
        time(&now);
        struct tm * timeinfo;
        timeinfo = localtime (&now);
        arq = fopen("historicos/pressoes.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        strftime (date_now,80,"%d/%m/%y %X%n",timeinfo);
        fprintf(arq, "%datm-%s", pressao, date_now);
        fclose(arq);
}

void gravarLuminosidade(){
        FILE *arq;
        char date_now[80];
        time_t now;
        time(&now);
        struct tm * timeinfo;
        timeinfo = localtime (&now);
        arq = fopen("historicos/luminosidades.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        strftime (date_now,80,"%d/%m/%y %X%n",timeinfo);
        fprintf(arq, "%d%%-%s", luminosidade, date_now);
        fclose(arq);
}

void lerArqTemperatura(){
    FILE *in;
    int count = 0;
    long int pos;
    char s[100];

    in = fopen("historicos/temperaturas.txt", "a+");
    /* always check return of fopen */
    if (in == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(in, 0, SEEK_END);
    pos = ftell(in);
    /* Don't write each char on output.txt, just search for '\n' */
    while (pos) {
        fseek(in, --pos, SEEK_SET); /* seek from begin */
        if (fgetc(in) == '\n') {
            if (count++ == 10) break;
        }
    }

    /* Write line by line, is faster than fputc for each char */
    int i = 0; 
    while (fgets(s, sizeof(s), in) != NULL) {
        strcpy(temperaturas[i], s);
        i++;         
    }
    fclose(in);
}

void lerArqUmidade(){
    FILE *in;
    int count = 0;
    long int pos;
    char s[100];

    in = fopen("historicos/umidades.txt", "a+");
    /* always check return of fopen */
    if (in == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(in, 0, SEEK_END);
    pos = ftell(in);
    /* Don't write each char on output.txt, just search for '\n' */
    while (pos) {
        fseek(in, --pos, SEEK_SET); /* seek from begin */
        if (fgetc(in) == '\n') {
            if (count++ == 10) break;
        }
    }

    /* Write line by line, is faster than fputc for each char */
    int i = 0; 
    while (fgets(s, sizeof(s), in) != NULL) {
        strcpy(umidades[i], s);
        i++;
               
    }
    fclose(in);
}

void lerArqPressoes(){
    FILE *in;
    int count = 0;
    long int pos;
    char s[100];

    in = fopen("historicos/pressoes.txt", "a+");
    /* always check return of fopen */
    if (in == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(in, 0, SEEK_END);
    pos = ftell(in);
    /* Don't write each char on output.txt, just search for '\n' */
    while (pos) {
        fseek(in, --pos, SEEK_SET); /* seek from begin */
        if (fgetc(in) == '\n') {
            if (count++ == 10) break;
        }
    }

    /* Write line by line, is faster than fputc for each char */
    int i = 0; 
    while (fgets(s, sizeof(s), in) != NULL) {
        strcpy(pressoes[i], s);
        i++;       
    }
    fclose(in);
}

void lerArqLuminosidade(){
    FILE *in;
    int count = 0;
    long int pos;
    char s[100];

    in = fopen("historicos/luminosidades.txt", "a+");
    /* always check return of fopen */
    if (in == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fseek(in, 0, SEEK_END);
    pos = ftell(in);
    /* Don't write each char on output.txt, just search for '\n' */
    while (pos) {
        fseek(in, --pos, SEEK_SET); /* seek from begin */
        if (fgetc(in) == '\n') {
            if (count++ == 10) break;
        }
    }

    /* Write line by line, is faster than fputc for each char */
    int i = 0; 
    while (fgets(s, sizeof(s), in) != NULL) {
        strcpy(luminosidades[i], s);
        i++;       
    }
    fclose(in);
}


int potenciometro(){
      
        if(openI2CBus("/dev/i2c-1") == -1)
        {
                return EXIT_FAILURE;
        }
        setI2CSlave(0x48);
        pressao = (readVoltage(0) * 100)/3;
        luminosidade = (readVoltage(3) * 100)/3;
        gravarPressao();
        gravarLuminosidade();
        lerArqPressoes();
        lerArqLuminosidade();

        return EXIT_SUCCESS;
}

void read_dht11_dat()
{

        uint8_t laststate = HIGH;
        uint8_t counter = 0;
        uint8_t j = 0, i;
        float f; 

        dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
        wiringPiSetup();
        pinMode(DHTPIN, OUTPUT);
        digitalWrite(DHTPIN, LOW);
        delay(18);
        
        digitalWrite(DHTPIN, HIGH);
        delayMicroseconds(50);
        
        pinMode(DHTPIN, INPUT);

        for (i = 0; i < MAXTIMINGS; i++)
        {
                counter = 0;
                while (digitalRead(DHTPIN) == laststate)
                {
                        counter++;
                        delayMicroseconds(1);
                        if (counter == 255)
                        {
                                break;
                        }
                }
                laststate = digitalRead(DHTPIN);

                if (counter == 255)
                        break;

                if ((i >= 4) && (i % 2 == 0))
                {
                        dht11_dat[j / 8] <<= 1;
                        if (counter > 16)
                                dht11_dat[j / 8] |= 1;
                        j++;
                }
         }

        if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)))
        {
                f = dht11_dat[2] * 9. / 5. + 32;
               
                temperatura = dht11_dat[2];
                umidade = dht11_dat[0];
                gravarTemperatura();
                gravarUmidade();
                lerArqTemperatura();
                lerArqUmidade();
                //PERSISTIR OS DADOS
        } 
}

void enviarDados(){
        char *payload = NULL;
        size_t index = 0;
        cJSON *dado = cJSON_CreateObject();

        cJSON *array_temperaturas = NULL;
        cJSON *array_umidades = NULL;
        cJSON *array_pressoes = NULL; 
        cJSON *array_luminosidades = NULL;   

        array_temperaturas = cJSON_AddArrayToObject(dado, "temperaturas");
        if (array_temperaturas == NULL)
        {
                fprintf(stderr, "Failed to convertJson.\n");
        }

        array_umidades = cJSON_AddArrayToObject(dado, "umidades");
        if (array_umidades == NULL)
        {
                fprintf(stderr, "Failed to convertJson.\n");
        }

        array_pressoes = cJSON_AddArrayToObject(dado, "pressoes");
        if (array_pressoes == NULL)
        {
                fprintf(stderr, "Failed to convertJson.\n");
        }

        array_luminosidades = cJSON_AddArrayToObject(dado, "luminosidades");
        if (array_luminosidades == NULL)
        {
                fprintf(stderr, "Failed to convertJson.\n");
        }

        for (index = 0; index < (sizeof(temperaturas)/8); ++index)
        { 
                if(temperaturas[index] != NULL){
                        cJSON *temp = NULL;
                        temp = cJSON_CreateString(&temperaturas[index]);
                        // cJSON_AddItemToArray(array_temperaturas, temp);
                }        
        }

        // for (index = 0; index < (sizeof(pressoes)/8); ++index)
        // { 
        //         if(pressoes[index] != NULL){
        //                 cJSON *press = NULL;
        //                 press = cJSON_CreateString(pressoes[index]);
        //                 cJSON_AddItemToArray(array_pressoes, press);
        //         }        
        // }

        // for (index = 0; index < (sizeof(umidades)/8); ++index)
        // { 
        //         if(umidades[index] != NULL){
        //                 cJSON *umid = NULL;
        //                 umid = cJSON_CreateString(umidades[index]);
        //                 cJSON_AddItemToArray(array_umidades, umid);
        //         }        
        // }

        // for (index = 0; index < (sizeof(luminosidades)/8); ++index)
        // { 
        //         if(luminosidades[index] != NULL){
        //                 cJSON *lum = NULL;
        //                 lum = cJSON_CreateString(luminosidades[index]);
        //                 cJSON_AddItemToArray(array_luminosidades, lum);
        //         }        
        // }

        // payload = cJSON_Print(dado);
        // if (payload == NULL)
        // {
        //         fprintf(stderr, "Failed to convertJson.\n");
        // }
        // printf("\n%s", payload);
        // size_t len = strlen(payload);
        // publish_mosquitto("tp_03_g04/dados", len, char_intervalo_medicao);
}

void *getMeasurement(){
        while (1)
        {
                read_dht11_dat();
                potenciometro();
                printf("Chegou aqui");                
                enviarDados();               
                delay(intervalo_medicao); 
        }
}

// Função que printa no display os dados dos sensores
void mostrarMedidas(){
        lcdClear(lcd);    
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "H: %d T: %d C ", umidade, temperatura); // printando sensor de umidade e temperatura no display lcd
        lcdPosition(lcd, 0, 1);
        lcdPrintf(lcd, "P: %d L: %d", pressao, luminosidade); // printando sensor de pressão e luminosidade no display lcd

}

void mostrarTemperaturas(){
        char *temperatura_dado = strtok(temperaturas[indice_temperaturas], "\n"); 
        lcdClear(lcd);    
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "%d-Tmp:%s ", indice_temperaturas + 1, temperatura_dado); // printando sensor de umidade e temperatura no display lcd
}

void mostrarUmidades(){
        char *umidade_dado = strtok(umidades[indice_umidades], "\n");    
        lcdClear(lcd);    
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "%d-Umi:%s", indice_umidades + 1, umidade_dado); // printando sensor de umidade e umidade no display lcd        
}

void mostrarLuminosidades(){
        char *luminosidade_dado = strtok(luminosidades[indice_luminosidades], "\n");
        lcdClear(lcd);    
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "%d-Lum:%s ", indice_luminosidades + 1, luminosidade_dado); // printando sensor de umidade e luminosidade no display lcd
}

void mostrarPressoes(){
        char *pressao_dado = strtok(pressoes[indice_pressoes], "\n");        
        lcdClear(lcd);    
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "%d-Prs:%s ", indice_pressoes + 1, pressao_dado); // printando sensor de umidade e pressao no display lcd
}

// Função onde é alterado o tempo de medição na interface local
void mostrarSelecaoDeIntervalo(int tempo){
        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "Medicao (em s)");
        lcdPosition(lcd, 0, 1);
        lcdPrintf(lcd, "%d s", tempo);
}

void tela_principal(){
        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "Monitoramento");
        lcdPosition(lcd, 0, 1);
        lcdPrintf(lcd, "Sensores");
}
void medidas_tempo_real(){
        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "T: %dC U: %d%%", temperatura, umidade);
        lcdPosition(lcd, 0, 1);
        lcdPrintf(lcd, "P:%datm L:%d%%", pressao, luminosidade);
}
void tela_historicos(){
        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "   Historicos");
}
int main(void)
{     
        wiringPiSetup(); // inicializando a wiringPi
        mosquitto_lib_init(); //inicialização da lib do mosquitto

        //Envia o intervalo de medição atual ao broker
        sprintf(char_intervalo_medicao, "%d", intervalo_medicao);
        int len = sprintf(char_intervalo_medicao, "%d", intervalo_medicao);

        publish_mosquitto("tp_03_g04/intervalo", len, char_intervalo_medicao);

        //Inicia o LCD      
        lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);

        // cria thread que ler os sensores 
        pthread_t tid;
        pthread_create(&tid, NULL, getMeasurement, NULL); 
        // cria thread de subscribe
        pthread_t t_subscribe;
        pthread_create(&t_subscribe, NULL, subscribe_mosquitto, NULL); // thread que fica sempre ouvindo o mosquitto

        int modo = 0;
        pinMode(BUTTON_0, INPUT);
                

        while(1){ 
                if(digitalRead(BUTTON_0) == 0){
                        modo = modo + 1 ;
                        if (modo > 7){
                                modo = 0;
                        }                                               
                        delay(20);
                        printf("%d", modo);
                        while(digitalRead(BUTTON_0) == 0); // aguarda enquato chave ainda esta pressionada           
                        delay(20);

                }
                if (modo == 0){
                        tela_principal();                  
                              
                }else
                if (modo == 1){
                        mostrarSelecaoDeIntervalo(intervalo_medicao);
                        if(digitalRead(BUTTON_2) == 0){
                                intervalo_medicao = intervalo_medicao + 1000;                                            
                                delay(20);
                                while(digitalRead(BUTTON_2) == 0); // aguarda enquato chave ainda esta pressionada           
                                delay(20);
                                char char_intervalo_medicao[10];
                                sprintf(char_intervalo_medicao, "%d", intervalo_medicao);
                                int len = sprintf(char_intervalo_medicao, "%d", intervalo_medicao);
                                publish_mosquitto("tp_03_g04/intervalo", len, char_intervalo_medicao);                               

                        }
                        if(digitalRead(BUTTON_1) == 0){
                                intervalo_medicao = intervalo_medicao - 1000;
                                if (intervalo_medicao < 1000){
                                        intervalo_medicao = 1000;
                                }                                             
                                delay(20);
                                while(digitalRead(BUTTON_1) == 0); // aguarda enquato chave ainda esta pressionada           
                                delay(20);
                                char char_intervalo_medicao[10];
                                sprintf(char_intervalo_medicao, "%d", intervalo_medicao);
                                int len = sprintf(char_intervalo_medicao, "%d", intervalo_medicao);
                                publish_mosquitto("tp_03_g04/intervalo", len,char_intervalo_medicao); 

                        }                        
                              
                }else
                if(modo == 2) 
                        medidas_tempo_real();

                else if(modo == 3)
                        tela_historicos();
                else if(modo == 4) {                        
                        mostrarUmidades();
                        if(indice_umidades < 9)
                                indice_umidades = indice_umidades + 1;
                        else 
                                indice_umidades = 0;
                }else
                if(modo == 5) {
                        mostrarPressoes();
                        if(indice_pressoes < 9)
                                indice_pressoes = indice_pressoes + 1;
                        else 
                                indice_pressoes = 0;                     
                }else
                if(modo == 6) {
                        mostrarLuminosidades();
                        if(indice_luminosidades < 9)
                                indice_luminosidades = indice_luminosidades + 1;
                        else 
                                indice_luminosidades = 0;
                }else
                if(modo == 7) {
                        mostrarTemperaturas();
                        if(indice_temperaturas < 9)
                                indice_temperaturas = indice_temperaturas + 1;
                        else 
                                indice_temperaturas = 0;
                }
                
                delay(1000);            
        }

        
        
        return(0);      
}
