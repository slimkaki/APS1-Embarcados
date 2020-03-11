/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"
#include "musicas.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

#define LED_PIO           PIOC						// periferico que controla o LED
#define LED_PIO_ID        12					    // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX       8							// ID do LED no PIO
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)		// Mascara para CONTROLARMOS o LED 

// Configuracoes do botao
// #define BUT_PIO		PIOA
// #define BUT_PIO_ID	
// #define BUT_PIO_IDX	4
// #define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX)

// Configuracao do buzzer
#define BUZ_PIO		PIOC
#define BUZ_PIO_ID ID_PIOC
#define BUZ_PIO_IDX	13
#define BUZ_PIO_IDX_MASK (1u << BUZ_PIO_IDX)
/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void freq(float);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/



void freq(float freq) {
	float dec = 1000000.0/(2.0*freq);
	pio_set(LED_PIO, LED_PIO_IDX_MASK);
	pio_clear(BUZ_PIO, BUZ_PIO_IDX_MASK);
	delay_ms(dec);
	pio_clear(LED_PIO, LED_PIO_IDX_MASK);
	pio_set(BUZ_PIO, BUZ_PIO_IDX_MASK);
	delay_ms(dec);
	return;
}


// Função de inicialização do uC
void init(void)
{
	sysclk_init();

	// Desativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO na qual o LED foi conectado
	// para que possamos controlar o LED.
	pmc_enable_periph_clk(LED_PIO_ID);
	
	// Inicializa PIO do botao
	// pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUZ_PIO_ID);
	
	//Inicializa PC8 como saída
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	
	// configura pino ligado ao botão como entrada com um pull-up.
	//pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, PIO_DEFAULT);
	
	//pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, PIO_DEFAULT);
	
	pio_set_output(BUZ_PIO, BUZ_PIO_IDX_MASK, 0, 0, 0);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
	init();

	// super loop
	// aplicacoes embarcadas não devem sair do while(1).
	int i = 0;
	while (1)
	{	
		int t = sizeof(imperial_march_tempo)/sizeof(int);
		for(int n = 0; n < i; n++) {
			if(imperial_march_notes[i] != 0)
			freq((int)imperial_march_notes[i]*TRESQUARTOSTEMPO);
		}
		if(i < t){
			i++;
		}else{
			i = 0;
		}
	  
		//pio_set(PIOC, LED_PIO_IDX_MASK);
		//pio_set(PIOC, BUZ_PIO_IDX_MASK);      // Coloca 1 no pino LED
		//delay_ms(500);          
		//pio_clear(PIOC, BUZ_PIO_IDX_MASK);    // Coloca 0 no pino do LED
		//pio_clear(PIOC, LED_PIO_IDX_MASK);
		//delay_ms(500);                        // Delay por software de 200 ms
	}
	return 0;
}
