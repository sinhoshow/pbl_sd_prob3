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
 
#include "ads1115_rpi.h"
#include <mosquitto.h>

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

// void publish(MQTTClient client, char* topic, char* payload);
// int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

int lcd;
int dht11_dat[5] = {0, 0, 0, 0, 0};
int intervalo_medicao = 1000; //em s
int temperatura = 0;
int umidade = 0;
int pressao = 0;
int luminosidade = 0;
char* temperaturas[10];


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
        time_t now;
        time(&now);
        arq = fopen("historicos/temperaturas.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        fprintf(arq, "%d - %s", temperatura,  ctime(&now));
        fclose(arq);
}

void gravarUmidade(){
        FILE *arq;
        time_t now;
        time(&now);
        arq = fopen("historicos/umidades.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        fprintf(arq, "%d - %s", umidade,  ctime(&now));
        fclose(arq);
}
void gravarPressao(){
        FILE *arq;
        time_t now;
        time(&now);
        arq = fopen("historicos/pressoes.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        fprintf(arq, "%d - %s", pressao,  ctime(&now));
        fclose(arq);
}

void gravarLuminosidade(){
        FILE *arq;
        time_t now;
        time(&now);
        arq = fopen("historicos/luminosidades.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        fprintf(arq, "%d - %s", luminosidade,  ctime(&now));
        fclose(arq);
}

void lerArqTemperatura(){
        char linha[100];
        FILE *arq;
        char *result, ch;
        int numero_de_linhas = 0;
       // char *token = strtok(temperaturas, " ");
        arq = fopen("historicos/temperaturas.txt", "a+");
        if (arq == NULL){
                printf("Problema ao abrir arquivo");
        }
        printf("Lendo arquivo de temperatura\n");
        ch=fgetc(arq);
        while( ch != NULL ){                
                if(ch == '\n'){
                     numero_de_linhas++;
                     printf("numero_de_linhas: %d\n", numero_de_linhas);   
                }
                ch=fgetc(arq);             
                
        }
        printf("Numero de linhas: %d", numero_de_linhas);
                
        int j = 0;
        for (int i = 0; i < numero_de_linhas; i++)
        {       
                result = fgets(linha, 100, arq);
                if (numero_de_linhas - i < 10 && result){                        
                        temperaturas[j] = result;
                        j++;
                        printf("%s", temperaturas[j]);
                }                        
        }

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
                //PERSISTIR OS DADOS
        } 
}

void *getMeasurement(){
        while (1)
        {
                read_dht11_dat();
                potenciometro();
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

// Função onde é alterado o tempo de medição na interface local
void mostrarSelecaoDeIntervalo(int tempo){
        lcdClear(lcd);
        lcdPosition(lcd, 0, 0);
        lcdPrintf(lcd, "Medicao (em s)");
        lcdPosition(lcd, 0, 1);
        lcdPrintf(lcd, "%d s", tempo);
}



int main(void)
{               
        // int rc;
        //MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

        // /* Inicializacao do MQTT (conexao & subscribe) */
        // MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        // MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

        // rc = MQTTClient_connect(client, &conn_opts);

        // if (rc != MQTTCLIENT_SUCCESS)
        // {
        //      printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
        // }
        
        
        wiringPiSetup(); // inicializando a wiringPi
        mosquitto_lib_init();

        //MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC, 0);        
        lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);

        pthread_t tid;
        pthread_create(&tid, NULL, getMeasurement, NULL); // thread que fica sempre pedindo dados dos sensores 


        pthread_t t_subscribe;
        pthread_create(&t_subscribe, NULL, subscribe_mosquitto, NULL); // thread que fica sempre ouvindo o mosquitto

        //Logica dos botoes
        // DOIS MODOS NO DISPLAY //
                // MODO MEDIDAS
                // MODO INTERVALO
        int modo = 1;

        pinMode(BUTTON_0, INPUT);           


        while(1){ 
                if(digitalRead(BUTTON_0) == 0){
                        modo = !modo;                                               
                        delay(20);
                        while(digitalRead(BUTTON_0) == 0); // aguarda enquato chave ainda esta pressionada           
                        delay(20);
                }
                if(modo == 1) mostrarMedidas();
                if (modo == 0){
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
                              
                }
                
                delay(1000);            
        }

        
        
        return(0);      
}
