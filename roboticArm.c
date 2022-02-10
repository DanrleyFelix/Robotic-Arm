#include <stdint.h>
#include <stdbool.h>
#include "driverlib/debug.h"
#include "inc/tm4c1294ncpdt.h"
#include "periferico.h"


#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line){
}
#endif


uint32_t valorAD;
int th = 2000;                           // Intervalo de 2000 milissegundos
float erro;
float posicao = 100;

float controlador(float erro);
float uc;
float kp = 1;
int convAd=0;
int controle=0;
float posAtual;

float getPosicao(void);


int main(void){

    confAD();
    confPWM();
    initTimer1();

    while(1){
        if (controle==1)
        {
            posAtual=getPosicao();
            erro = posicao-posAtual;
            uc = controlador(erro);
            if (uc>100)
                uc=100;
            else if (uc<-100)
                uc=-100;
            uc=50+uc/2;
            setPWM(uc);
            controle=0;
        }
    }
}


float controlador(float erro){

    float uc1;

    uc1 = (erro*kp);
    return uc1;
}


float getPosicao(void){

    ADC0_PSSI_R |= (1<<3);              // Inicia a conversão com o sequencializador de amostras número 3
    while (convAd==0);                  // Espera o AD finalizar a conversão e gerar a interrupção
    convAd=0;                           // Limpa a variável após conversão
    return (valorAD*100/4095);          // Retorna o valor percentual da posição

}


void intTimer1Handler(void){

    TIMER1_ICR_R=0x1;           // Limpa a interrupção do timer
    controle=1;                 // Variável de controle no loop do main

}