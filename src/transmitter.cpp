// 8 Channel Transmitter
// Input pin A5

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
const uint64_t pipeOut = 000322; // Código do controle (mesmo no receptor)
RF24 radio(9, 10);               // PINO  CE,CSN
int update = 3000;
long last = 0;

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
void ResetData()
{
    data.throttle = 0;
    data.wheel = 127;
    data.axis1 = 0;
    data.axis2 = 0;
    data.aux1 = 0; // Posição caso perda de sinal
    data.aux2 = 0;
    data.aux3 = 0;
    data.aux4 = 0;
}
void setup()
{
    Serial.begin(115200);
    pinMode(39, INPUT);
    pinMode(40, INPUT);
    radio.begin();
    radio.openWritingPipe(pipeOut); // Código controle <-> Receptor
    radio.setChannel(100);
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS); // Baixa velocidade, maior alcance e mais estavel
    radio.setPALevel(RF24_PA_MAX);   // Máximo de potência
    radio.stopListening();           // Inicia modo Transmissor
    ResetData();
}
// Função de ajuste de valores dos potenciometros e sentido
// SAIDA 8 bits
int Border_Map(int val, int lower, int middle, int upper, bool reverse)
{
    val = constrain(val, lower, upper); // restringe valores máximos e mínimos
    if (val < middle)
        val = map(val, lower, middle, 0, 128);
    else
        val = map(val, middle, upper, 128, 255);
    return (reverse ? 255 - val : val);
}

void loop()
{

    // Calibração dos Controles

    data.throttle = Border_Map(analogRead(20), 0, 512, 1023, true); // "true" ou "false" invertendo sentido
                                                                    // data.throttle = Border_Map( analogRead(A1),0, 512, 1023, false ); // Para ESC Bidirectional
    data.wheel = Border_Map(analogRead(4), 0, 512, 1023, true);

    //                                      min, mid,max
    data.axis1 = Border_Map(analogRead(8), 570, 800, 1023, false); // For Single side ESC | Tek yönlü ESC için
    data.axis2 = Border_Map(analogRead(5), 0, 512, 1023, true);
    data.aux1 = Border_Map(analogRead(6), 0, 512, 1023, true);  // "true" or "false" for change signal direction | "true" veya "false" sinyal yönünü değiştirir.
    data.aux2 = Border_Map(analogRead(18), 0, 512, 1023, true); // "true" or "false" for change signal direction | "true" veya "false" sinyal yönünü değiştirir.
    data.aux3 = digitalRead(39);
    data.aux4 = digitalRead(40);

    radio.write(&data, sizeof(Signal));
    Serial.print("/\\ ");
    Serial.print(analogRead(20));
    Serial.print("   <- -> ");
    Serial.print(analogRead(4));
    Serial.print("   A2 ");
    Serial.print(analogRead(8));
    Serial.print("   A3 ");
    Serial.println(analogRead(5));
}
