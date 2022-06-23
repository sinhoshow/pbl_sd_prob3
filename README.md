# Sistemas Digitais Problema 3

Foi solicitado um sistema para ser implementado na raspberry pi zero, onde medirá temperatura, umidade, pressão atmosférica e luminosidade. Esses dados e a visualização do histórico com as 10 últimas medições de cada sensor terão que ser apresentados no display lcd e na interface remota. Além disto, o sistema deve permitir o ajuste local e remoto do intervalo de tempo que serão realizadas as medições. Para comunicação com a interface remota foi utilizado o protocolo MQTT.

# Equipe
Bianca Santana de A. Silva
Nilson ...

# Fluxo

# Interface local
Na interface local temos: 
Sensor dht11
Temos que esperar o tempo de resposta de acordo com sua documentação, 
Potenciômetro
Usamos a biblioteca ads1115_rpi, utilizando o protocolo I2C para comunicação. Através deste protocolo podemos configurar o dispositivo e fazer a leitura de seus dados de conversão  Analógica-Digital .
MultiThread
Foi utilizado multithread para poder ficar sempre lendo os dados dos sensores.
Display LCD e Botão
É possível ver o tempo de medição e alterá-lo utilizando os botões 
São mostrados os dados dos sensores em tempo real e os históricos de cada um.
Histórico
A persistência de dados dos históricos são feitas por meio de arquivo de texto.
MQTT e Broker
Foi utilizada a biblioteca mosquitto.h, ela precisa ser instalada pois não vem nativa da linguagem C 
Os arquivos pertecentes a essa interface são os que terminam com .h (são as bibliotecas), .c (os executaveis) e a pasta histórico (encontramos os arquivos de texto .txt que guardam os históricos de cada sensor)  

# Interface remota

# Como executar

# Teste 
