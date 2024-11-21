int buz = 42;      // Pino do buzzer
int tempo = 2;     // Duração do tom
int minimo = 1250; // Frequência mínima
int maximo = 2250; // Frequência máxima
int i = 0;         // Variável de iteração
int b1 = 19;
int b2 = 20;
int b3 = 21;
int b4 = 47;
int b = 0;
int ledVermelho = 1; // Pino do LED vermelho
int ledAzul = 2;     // Pino do LED azul

//  8 Channel Receiver | 8 Kanal Alıcı

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP32Servo.h>

int ch_width_1 = 0;
int ch_width_2 = 0;
int ch_width_3 = 0;
int ch_width_4 = 0;
int ch_width_5 = 0;
int ch_width_6 = 0;
int ch_width_7 = 0;
int ch_width_8 = 0;

Servo ch1;
Servo ch2;
Servo ch3;
Servo ch4;
Servo ch5;
Servo ch6;
Servo ch7;
Servo ch8;

struct Signal
{

    byte throttle;
    byte wheel;
    byte axis1;
    byte axis2;
    byte aux1;
    byte aux2;
    byte aux3;
    byte aux4;
};

Signal data;

const uint64_t pipeIn = 000322; // Código Transmissor Receptor
RF24 radio(9, 10);              // Pinos do NRF

void ResetData()
{
    // Valores iniciais para os eixos
    data.throttle = 0;
    data.wheel = 127;
    data.axis1 = 127; //??
    data.axis2 = 127; //??
    data.aux1 = 0;
    data.aux2 = 0;
    data.aux3 = 0;
    data.aux4 = 0;
}

void setup()
{
    // Pinos para cada eixo
    ch1.attach(0); // throttle;
    ch2.attach(2); // wheel;
    ch3.attach(3); // axis1
    ch4.attach(4); // axis2
    ch5.attach(5); // aux1
    ch6.attach(6); // aux2
    ch7.attach(7); // aux3
    ch8.attach(8); // aux4
    ResetData();
    radio.begin();
    radio.openReadingPipe(1, pipeIn);
    radio.setChannel(100);
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS); // Baixa taxa de transmissão para maior alcance e estabilidade
    radio.setPALevel(RF24_PA_MAX);   // Potência Máxima
    radio.startListening();          // Inicia comunicação como receptor

    Serial.begin(115200);
    pinMode(buz, OUTPUT);
    pinMode(ledVermelho, OUTPUT);
    pinMode(b1, INPUT_PULLUP);
    pinMode(b2, INPUT_PULLUP);
    pinMode(b3, INPUT_PULLUP);
    pinMode(b4, INPUT_PULLUP);
    pinMode(ledAzul, OUTPUT);
    Serial.println("Teste");
}

unsigned long lastRecvTime = 0;

void recvData()
{
    while (radio.available())
    {
        radio.read(&data, sizeof(Signal)); // Recebe comunicação em "data"
        lastRecvTime = millis();
    }
}

void loop()
{
    recvData(); // recebe comunicação
    unsigned long now = millis();
    if (now - lastRecvTime > 1000)
    {                // se demorar mais de 1 segunda para receber
        ResetData(); // significa que perdeu sinal e controles são zerados..
    }

    ch_width_1 = map(data.throttle, 0, 255, 1000, 2000);
    ch_width_2 = map(data.wheel, 0, 255, 1000, 2000);
    ch_width_3 = map(data.axis1, 0, 255, 1000, 2000);
    ch_width_4 = map(data.axis2, 0, 255, 1000, 2000);
    ch_width_5 = map(data.aux1, 0, 255, 1000, 2000);
    ch_width_6 = map(data.aux2, 0, 255, 1000, 2000);
    ch_width_7 = map(data.aux3, 0, 1, 1000, 2000);
    ch_width_8 = map(data.aux4, 0, 1, 1000, 2000);

    ch1.writeMicroseconds(ch_width_1); // Ajusta posição do servo correspondente
    ch2.writeMicroseconds(ch_width_2); // Ajusta posição do servo correspondente
    ch3.writeMicroseconds(ch_width_3); // Ajusta posição do servo correspondente
    ch4.writeMicroseconds(ch_width_4); // Ajusta posição do servo correspondente
    ch5.writeMicroseconds(ch_width_5); // Ajusta posição do servo correspondente
    ch6.writeMicroseconds(ch_width_6); // Ajusta posição do servo correspondente
    ch7.writeMicroseconds(ch_width_7); // Ajusta posição do servo correspondente
    ch8.writeMicroseconds(ch_width_8); // Ajusta posição do servo correspondente
}

void toggleLed();

int button();

void sirene(int choice)
{
    switch (choice)
    {
    case 1:
        for (i = minimo; i < maximo; i++)
        {
            tone(buz, i, tempo);
        }
        for (i = maximo; i > minimo; i--)
        {
            tone(buz, i, tempo);
        }
        break;
    case 2:
        float som;
        for (i = 0; i < 180; i++)
        {
            som = sin(i * 3.1416 / 180);
            int freq = abs(minimo + (maximo - minimo) * som);
            tone(buz, freq, tempo * 10);
        }

        break;
    case 3:
        for (i = 0; i < 4; i++)
        {
            tone(buz, maximo, 300);
            tone(buz, minimo, 300);
        }
        break;
    case 4:
        toggleLed();
        break;
    }
}

void toggleLed()
{
    if (digitalRead(ledVermelho))
    {
        digitalWrite(ledVermelho, LOW);
        digitalWrite(ledAzul, HIGH);
    }
    else
        digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledAzul, LOW);
}

int button()
{
    if (!digitalRead(b1))
        return 1;
    else if (!digitalRead(b2))
        return 2;
    else if (!digitalRead(b3))
        return 3;
    else if (!digitalRead(b4))
        return 4;
    return 0;
}