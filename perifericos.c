// PROVA FINAL - SADI - AD E TIMER
// SEMESTRE 2021.1
// ALUNO: DANRLEY SANTOS FELIX - 11328463

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/debug.h"
#include "inc/tm4c1294ncpdt.h"


#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line){
}
#endif


int timeMili = 5;                               // 5 milissegundos
int valorAD;
int convAD = 0;
int controleTimer = 0;

void confAD(void);
void initTimer1(void);


int main(void){

    confAD();
    initTimer1();

    while(1){
        if (controleTimer == 1){
            ADC0_PSSI_R |= (1<<2);              // Inicia a conversão com o sequencializador de amostras número 2
            while (convAD==0);                  // Espera o AD finalizar a conversão e gerar a interrupção
            convAD=0;                           // Limpa a variável após conversão
        }
    }
}


void confAD(void){

    // Table 15-7. ADC Register Map
    // Table 26-3. Signals by Signal Name
    // CANAIS = 1,3 E 16
    // AMOSTRAS = 0, 1 E 2
    // Fim da sequência  = Amostra 2 (canal 16)
    // AIN1 = PE2 (amostra 0)
    // AIN3 = PE0 (amostra 1)
    // AIN16 = PK0 (amostra 2)

    SYSCTL_RCGCADC_R= 0x1;                              // Habilita o clock do conversor AD
    SYSCTL_RCGCGPIO_R |= (1<<4) | (1<<9);               // Habilita as portas E e K

    GPIO_PORTE_AHB_AFSEL_R |=  (1<<0) | (1<<2);         // Coloca os pinos 0 e 2 da porta E como funções alternativas
    GPIO_PORTE_AHB_DEN_R &= ~(1<<2);                    // PE2 não é pino digital
    GPIO_PORTE_AHB_DEN_R &= ~(1<<0);                    // PE0 não é pino digital
    GPIO_PORTE_AHB_AMSEL_R |= (1<<0) | (1<<2);          // Função analógica habilitada no PE0 e PE2

    GPIO_PORTK_AFSEL_R |=  (1<<0) | (1<<2);             // Coloca o PK0 como função alternativa
    GPIO_PORTK_DEN_R &= ~(1<<0);                        // PK0 não é pino digital
    GPIO_PORTK_AMSEL_R |= (1<<0);                       // Função analógica habilitada no PK0

    // Configuração da fila

    ADC0_ACTSS_R &= ~(1<<2);                            // Desabilita o SS2 do AD0 antes da inicialização
    ADC0_EMUX_R = 0x000;                                // EM2 - A conversão é iniciada definindo o bit SSn no registrador do ADCPSSI
    ADC0_SSEMUX2_R |= 0x100;                            // AIN16 -> Amostra 2 -> 19:16 bits. As outras estão em 15:0 bits.
    ADC0_SSMUX2_R |= (0x1) | (0x30);                    // Canal 1 no MUX0, canal 3 no MUX1
    ADC0_SSMUX2_R |= (0x100);                           // Canal 16 no MUX2

    ADC0_SSCTL2_R |= (1<<9);                            // A terceira amostra é a última da sequência
    ADC0_ACTSS_R |= (1<<2);                             // Habilita a o SS2 do AD0

    // Interrupção do AD

    ADC0_SSCTL2_R |= (1<<10);               // A terceira amostra habilita a interrupção
    ADC0_IM_R |= (1<<2);                    // Propaga a interrupção para o sequencializador de amostras número 2
    NVIC_EN0_R |= 1<<17;                    // Interrupção do AD na NVIC

}

void initTimer1(void){

    SYSCTL_RCGCTIMER_R |= (0x2);

    TIMER1_CTL_R &= ~0x1;                   // Desabilita o Timer antes da inicialização
    TIMER1_CFG_R = 0x0;                     // Escolhe o timer 32 bits para uma maior temporização
    TIMER1_TAMR_R = 0x2;                    // Modo periódico e o contador é decrescente
    TIMER1_TAILR_R = 120*1000*timeMili;     // Intervalo em milissegundos
    TIMER1_CTL_R |= (0x1);                  // Habilita o temporizador 1A
    TIMER1_IMR_R = (0x1);                   // Habilita a interrupção por timeout
    NVIC_EN0_R |= (0x200000);               // Configura a interrupção na NVIC de T1A

}

void intTimer1Handler(void){

    TIMER1_ICR_R=0x1;                       // Limpa a interrupção do timer
    controleTimer=1;                        // Variável de controle no loop do main

}

void intAD0(void){

    ADC0_ISC_R |= (1<<2);                   // Limpa a interrupção do SS2 do AD0
    valorAD=ADC0_SSFIFO2_R;                 // Resultado da conversão no SS2
    convAD=1;                               // Habilita o encerramento da conversão

}
