#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"

extern uint32_t valorAD;
extern int th;
extern int convAd;


void confAD(void){

    SYSCTL_RCGCADC_R= 0x1;
    SYSCTL_RCGCGPIO_R |= (1<<4);
    GPIO_PORTE_AHB_AFSEL_R |= (1<<3);
    GPIO_PORTE_AHB_DEN_R &= ~(1<<3);
    GPIO_PORTE_AHB_AMSEL_R |= (1<<3);

    // Configurar sequencializador

    ADC0_ACTSS_R &= ~(1<<3);
    ADC0_EMUX_R = 0x0;
    ADC0_SSEMUX3_R = 0x0;
    ADC0_SSMUX3_R = 0x0;

    ADC0_SSCTL3_R = 0x2;
    ADC0_ACTSS_R |= (1<<3);

    // Interrupção do AD

    ADC0_SSCTL3_R |= (1<<2);
    ADC0_IM_R |= (1<<3);
    NVIC_EN0_R |= 1<<17;
}

void intAD0(void){

    ADC0_ISC_R |= (1<<3);
    valorAD=ADC0_SSFIFO3_R;
    convAd=1;

}

void confPWM(void){

    SYSCTL_RCGCPWM_R =0x1;
    SYSCTL_RCGCGPIO_R |= (1<<5);

    GPIO_PORTF_AHB_AFSEL_R |= ((1<<2) | (1<<3));
    GPIO_PORTF_AHB_PCTL_R |= 0x6600;
    GPIO_PORTF_AHB_DEN_R |= ((1<<2) | (1<<3));
    PWM0_CC_R = 0x101;
    PWM0_1_CTL_R = 0x0;
    PWM0_1_DBCTL_R |= 0x1;
    PWM0_1_GENA_R = 0x8C;
    PWM0_1_GENB_R = 0x80C;
    PWM0_1_LOAD_R = 0x752F;
    PWM0_1_CMPA_R = 0x3A97;
    PWM0_1_CMPB_R = 0x57E3;
    PWM0_1_CTL_R = 0x1;


    PWM0_ENABLE_R |= ((1<<2)| (1<<3));
}

void setPWM(float valor){

    PWM0_1_CMPA_R = (1-valor/100)*(PWM0_1_LOAD_R-1);

}

void initTimer1(void){

    SYSCTL_RCGCTIMER_R |= (0x2);

    TIMER1_CTL_R &= ~0x1;                   // Desabilita o Timer antes da inicialização
    TIMER1_CFG_R = 0x0;                     // Timer 32 bits
    TIMER1_TAMR_R = 0x2;                    // Modo periódico e contador decrescente
    TIMER1_TAILR_R = 120*1000*th;           // Intervalo em milissegundos
    TIMER1_CTL_R |= (0x1);                  // Habilita o temporizador 1A
    TIMER1_IMR_R = (0x1);                   // Habilita a interrupção por timeout
    NVIC_EN0_R |= (0x200000);               // Configura a interrupção na NVIC de T1 A

}
