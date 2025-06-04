#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;  // Criar uma instância do ADS1115

// Estruturas para armazenar valores de distância e pinos
struct PontoA1 {
    float distanciaA1; // em cm
    uint16_t valorA1;  // valor lido do pino
};

struct PontoA2 {
    float distanciaA2; // em cm
    uint16_t valorA2;  // valor lido do pino
};

struct PontoA3 {
    float distanciaA3; // em cm
    uint16_t valorA3;  // valor lido do pino
};

// Arrays de pontos para cada sensor
//sup
PontoA1 pontosA1[] = {
    
    {10.0, 8750},   // 10 cm
    {20.0, 12500},   // 20 cm
    {30.0, 17500},  // 30 cm
    {40.0, 24200},  // 40 cm
    {50.0, 33300}   // 50 cm
};
//front
PontoA2 pontosA2[] = {
    {10.0, 11200},   // 10 cm
    {20.0, 19150},   // 20 cm
    {30.0, 26450},  // 30 cm
    {39.0, 32900},  // 40 cm
    
    
};
//esq
PontoA3 pontosA3[] = {
    {10.0, 7480},   // 10 cm
    {20.0, 13610},   // 20 cm
    {30.0, 19950},  // 30 cm
    {40.0, 26210},  // 40 cm
    {50.0, 32460}   // 50 cm
};

// Coeficientes para cada sensor
float aA1 = 0.0, bA1 = 0.0;
float aA2 = 0.0, bA2 = 0.0;
float aA3 = 0.0, bA3 = 0.0;

void setup() {
    Serial.begin(9600);
    Serial.println("Inicializando o ADS1115...");

    Wire.begin();
    ads.begin();  // Inicializa o ADS1115
    ads.setGain(GAIN_ONE); // Configura o ganho (±0.256V)
    
    // Calcular coeficientes para cada entrada
    calcularCoeficientesA1();
    calcularCoeficientesA2();
    calcularCoeficientesA3();
}

void calcularCoeficientesA1() {
    int nA1 = sizeof(pontosA1) / sizeof(pontosA1[0]);
    float somaxA1 = 0, somayA1 = 0, somaxyA1 = 0, somax2A1 = 0;

    for (int iA1 = 0; iA1 < nA1; iA1++) {
        somaxA1 += pontosA1[iA1].valorA1;
        somayA1 += pontosA1[iA1].distanciaA1;
        somaxyA1 += pontosA1[iA1].valorA1 * pontosA1[iA1].distanciaA1;
        somax2A1 += pontosA1[iA1].valorA1 * pontosA1[iA1].valorA1;
    }

    aA1 = (nA1 * somaxyA1 - somaxA1 * somayA1) / (nA1 * somax2A1 - somaxA1 * somaxA1);
    bA1 = (somayA1 - aA1 * somaxA1) / nA1;

    Serial.print("Coeficientes calculados para A1: a = ");
    Serial.print(aA1);
    Serial.print(", b = ");
    Serial.println(bA1);
}
void calcularCoeficientesA2() {
    int nA2 = sizeof(pontosA2) / sizeof(pontosA2[0]);
    float somaxA2 = 0, somayA2 = 0, somaxyA2 = 0, somax2A2 = 0;

    for (int iA2 = 0; iA2 < nA2; iA2++) {
        somaxA2 += pontosA2[iA2].valorA2;
        somayA2 += pontosA2[iA2].distanciaA2;
        somaxyA2 += pontosA2[iA2].valorA2 * pontosA2[iA2].distanciaA2;
        somax2A2 += pontosA2[iA2].valorA2 * pontosA2[iA2].valorA2;
    }

    aA2 = (nA2 * somaxyA2 - somaxA2 * somayA2) / (nA2 * somax2A2 - somaxA2 * somaxA2);
    bA2 = (somayA2 - aA2 * somaxA2) / nA2;

    Serial.print("Coeficientes calculados para A2: a = ");
    Serial.print(aA2);
    Serial.print(", b = ");
    Serial.println(bA2);
}

void calcularCoeficientesA3() {
    int nA3 = sizeof(pontosA3) / sizeof(pontosA3[0]);
    float somaxA3 = 0, somayA3 = 0, somaxyA3 = 0, somax2A3 = 0;

    for (int iA3 = 0; iA3 < nA3; iA3++) {
        somaxA3 += pontosA3[iA3].valorA3;
        somayA3 += pontosA3[iA3].distanciaA3;
        somaxyA3 += pontosA3[iA3].valorA3 * pontosA3[iA3].distanciaA3;
        somax2A3 += pontosA3[iA3].valorA3 * pontosA3[iA3].valorA3;
    }

    aA3 = (nA3 * somaxyA3 - somaxA3 * somayA3) / (nA3 * somax2A3 - somaxA3 * somaxA3);
    bA3 = (somayA3 - aA3 * somaxA3) / nA3;

    Serial.print("Coeficientes calculados para A3: a = ");
    Serial.print(aA3);
    Serial.print(", b = ");
    Serial.println(bA3);
}

float calcularDistancia(float a, float b, uint16_t valor) {
    return a * valor + b;
}

float arredondar(float distancia) {
    float parteDecimal = distancia - floor(distancia);
    
    if (parteDecimal <= 0.30) {
        return floor(distancia); // Arredonda para baixo
    } else if (parteDecimal <= 0.69) {
        return floor(distancia) + 0.50; // Arredonda para 0.50
    } else {
        return ceil(distancia); // Arredonda para cima
    }
}

void loop() {
    static uint32_t lastSample = 0;
    uint32_t now = millis();

    // Faz uma leitura a cada 1 segundo
    if (now - lastSample >= 500) {
        lastSample = now;

        // Lê os valores dos pinos A1, A2 e A3 
        uint16_t valorA1 = ads.readADC_SingleEnded(1); // Lê do pino A1
        uint16_t valorA2 = ads.readADC_SingleEnded(2); // Lê do pino A2
        uint16_t valorA3 = ads.readADC_SingleEnded(3); // Lê do pino A3

        // Calcula as distâncias para cada pino
        float distanciaA1 = calcularDistancia(aA1, bA1, valorA1);
        float distanciaA2 = calcularDistancia(aA2, bA2, valorA2);
        float distanciaA3 = calcularDistancia(aA3, bA3, valorA3);
        
        // Arredonda as distâncias
        distanciaA1 = arredondar(distanciaA1);
        distanciaA2 = arredondar(distanciaA2);
        distanciaA3 = arredondar(distanciaA3);
        
        // // Imprimir valores e distâncias para cada pino
        // Serial.print("Valor do Pino A1: ");
        // Serial.print(valorA1);
        // Serial.print("\tDistância A1 Superior: ");
        // Serial.print(53 - distanciaA1, 2); // Imprimir com 2 casas decimais
        // Serial.println(" cm");

        // Serial.print("Valor do Pino A2: ");
        // Serial.print(valorA2);
        // Serial.print("\tDistância A2 Frontal: ");
        // Serial.print(45 - distanciaA2, 2); // Imprimir com 2 casas decimais
        // Serial.println(" cm");

        // Serial.print("Valor do Pino A3: ");
        // Serial.print(valorA3);
        // Serial.print("\tDistância A3 Esquerda: ");
        // Serial.print(40 - distanciaA3, 2); // Imprimir com 2 casas decimais
        // Serial.println(" cm");

       // Serial.println("-------------------"); // Separador para clareza

      // Calcule as distâncias
    float distanciaFrontal =  (45 - distanciaA2);
    float distanciaSuperior = (53 - distanciaA1);
    float distanciaEsquerda = 0; // Ou alguma outra lógica
    float distanciaDireita = (40 - distanciaA3);

    // Saída formatada
  //  Serial.print("ini(");
  ///  Serial.println(distanciaSuperior); // Superior
  //  Serial.print("|");
  //  Serial.println(distanciaFrontal);  // Frontal
 //   Serial.print("|");
 //   Serial.println(distanciaDireita);   // Direita
//    Serial.print(")fim$");
    Serial.print(distanciaSuperior); // Superior
    Serial.print("|");
    Serial.print(distanciaFrontal);  // Frontal
    Serial.print("|");
    Serial.println(distanciaDireita);   // Direita
    



    }
}