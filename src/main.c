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

#define LED_PIO           PIOC					  // periferico que controla o LED
#define LED_PIO_ID        12					  // ID do periférico PIOC (controla LED)
#define LED_PIO_IDX       8						  // ID do LED no PIO
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)	  // Mascara para CONTROLARMOS o LED 

#define XLED1_PIO           PIOA                  // periferico que controla o LED1
#define XLED1_PIO_ID        10                    // ID do periférico PIOC (controla LED)
#define XLED1_PIO_IDX		0u                    // ID do LED no PIO
#define XLED1_PIO_IDX_MASK  (1u << XLED1_PIO_IDX) // Mascara para CONTROLARMOS o LED

#define XLED2_PIO           PIOC                  // periferico que controla o LED2
#define XLED2_PIO_ID        12                    // ID do periférico PIOC (controla LED)
#define XLED2_PIO_IDX		30u                   // ID do LED no PIO
#define XLED2_PIO_IDX_MASK  (1u << XLED2_PIO_IDX) // Mascara para CONTROLARMOS o LED

#define XLED3_PIO           PIOB                  // periferico que controla o LED3
#define XLED3_PIO_ID        11                    // ID do periférico PIOC (controla LED)
#define XLED3_PIO_IDX		2u                    // ID do LED no PIO
#define XLED3_PIO_IDX_MASK  (1u << XLED3_PIO_IDX) // Mascara para CONTROLARMOS o LED

// Configuracao do buzzer
#define BUZ_PIO				PIOC
#define BUZ_PIO_ID			ID_PIOC
#define BUZ_PIO_IDX			13
#define BUZ_PIO_IDX_MASK	(1u << BUZ_PIO_IDX)

//Config Buts
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_PIO_IDX         11
#define BUT_PIO_IDX_MASK (1 << BUT_PIO_IDX)

#define XBUT1_PIO PIOD //
#define XBUT1_PIO_ID ID_PIOD
#define XBUT1_PIO_IDX 28
#define XBUT1_PIO_IDX_MASK (1u << XBUT1_PIO_IDX)


#define XBUT2_PIO PIOC //
#define XBUT2_PIO_ID 12
#define XBUT2_PIO_IDX 31
#define XBUT2_PIO_IDX_MASK (1u << XBUT2_PIO_IDX)


#define XBUT3_PIO PIOA //
#define XBUT3_PIO_ID  ID_PIOA//
#define XBUT3_PIO_IDX 19
#define XBUT3_PIO_IDX_MASK (1u << XBUT3_PIO_IDX)

/************************************************************************/
/* constants                                                            */
/************************************************************************/
#define CORTEMP_SW 375  //Correcao temporal StarWar
#define CORTEMP_UW 950  //Correcao temporal UNDER WORLD
#define CORTEMP_PC 650  //Correcao temporal Piratas do Caribe
#define McrToMli 100 //Convercao Micro to Mili
#define BUT_PRIOR 5 //Prioridade botao da placa
#define XBUT1_PRIOR 5 //Prioridade botao XOLED1
#define XBUT2_PRIOR 5 //Prioridade botao XOLED2
#define XBUT3_PRIOR 5 //Prioridade botao XOLED3

#define new_song(song, n, t)                    \
{                                           \
	song.notes = n;                         \
	song.tempos = t;                        \
}

/************************************************************************/
/* structs                                                              */
/************************************************************************/
typedef struct
{
	int *notes;
	int *tempos;
} song;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);
void freq(int,int);
int check(void);
int check_pause(void);
void BUT_callback(void);
void XBUT1_callback(void);
void XBUT2_callback(void);
void XBUT3_callback(void);
void norm_play(int notes[],int tempos[],int correcao,int* last_but,int* last_n,int* pause,int len);
void pause_play(int notes[],int tempos[],int correcao,int* last_but,int* last_n,int* pause,int len);
/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

volatile char BUT_flag = 0;
volatile char XBUT1_flag = 0;
volatile char XBUT2_flag = 0;
volatile char XBUT3_flag = 0;


void BUT_callback(void)
{	

	BUT_flag = 1;
	
}

void XBUT1_callback(void)
{
	XBUT1_flag = 1;
}

void XBUT2_callback(void)
{
	XBUT2_flag = 1;
}

void XBUT3_callback(void)
{
	XBUT3_flag = 1;
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void freq(int fr,int temp) {
	int dec_us = McrToMli/(fr);
	int tempo_us = temp;
	int j = 0;
	while (j<tempo_us/(dec_us)){
		pio_clear(LED_PIO, LED_PIO_IDX_MASK);
		pio_set(BUZ_PIO, BUZ_PIO_IDX_MASK);
		delay_us(dec_us);
		pio_set(LED_PIO, LED_PIO_IDX_MASK);
		pio_clear(BUZ_PIO, BUZ_PIO_IDX_MASK);
		delay_us(dec_us);
		j++; 
		
	}
	return;
}

void pause_play(int notes[],int tempos[],int correcao,int* last_but,int* last_n,int* pause,int len){
	BUT_flag = 0;
	for(int n = *last_n; n < len; n++) {
		freq(notes[n],tempos[n]);
		int PBN = tempos[n]*correcao; //Pause Between Notes
		delay_us(PBN);
		freq(0,0);
		if (BUT_flag){
			*last_n = n;
			*pause = 1;
			if(correcao==650){
				*last_but = 1;
			}
			else if(correcao==375){
				*last_but = 2;
			}
			else{
				*last_but = 3;
			}
			return;
		}
		
	}
	*pause = 0;
	*last_but = 0;
	*last_n = 0;
	return;
}

void norm_play(int notes[],int tempos[],int correcao,int* last_but,int* last_n,int* pause,int len){
	BUT_flag = 0;
	for(int n = 0; n < len; n++) {
		freq(notes[n],tempos[n]);
		int PBN = tempos[n]*correcao; //Pause Between Notes
		delay_us(PBN);
		freq(0,0);
		if (BUT_flag){
			*last_n = n;
			*pause = 1;
			if(correcao==650){
				*last_but = 1;
			}
			else if(correcao==375){
				*last_but = 2;
			}
			else{
				*last_but = 3;
			}
			return;
		}
		
	}
	*pause = 0;
	*last_but = 0;
	*last_n = 0;
	return;
}


// Função de inicialização do uC
void init(void)
{	
	sysclk_init();

	// Desativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Ativa o PIO do LED da placa
	pmc_enable_periph_clk(LED_PIO_ID);
	
	// Inicializa PIO do botao da placa
	// pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUZ_PIO_ID);
	
	//Inicializa PIO dos Botoes X
	pmc_enable_periph_clk(XBUT1_PIO_ID);
	
	pmc_enable_periph_clk(XBUT2_PIO_ID);
	
	pmc_enable_periph_clk(XBUT3_PIO_ID);
	
	//Inicializa PIO dos LED X
	pmc_enable_periph_clk(XLED1_PIO_ID);
	
	pmc_enable_periph_clk(XLED2_PIO_ID);
	
	pmc_enable_periph_clk(XLED3_PIO_ID);
	
	// configura pino ligado ao botão como entrada com um pull-up.
	pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, PIO_DEFAULT);
	
	pio_set_input(XBUT1_PIO,XBUT1_PIO_IDX_MASK,PIO_DEFAULT);
	
	pio_set_input(XBUT2_PIO,XBUT2_PIO_IDX_MASK,PIO_DEFAULT);
	
	pio_set_input(XBUT3_PIO,XBUT3_PIO_IDX_MASK,PIO_DEFAULT);
	
	
	// configura pinos ligado aos botoes como entrada com um pull-up.
	pio_configure(BUT_PIO,PIO_INPUT,BUT_PIO_IDX_MASK,PIO_PULLUP);
	
	pio_configure(XBUT1_PIO, PIO_INPUT, XBUT1_PIO_IDX_MASK, PIO_PULLUP);
	
	pio_configure(XBUT2_PIO, PIO_INPUT, XBUT2_PIO_IDX_MASK, PIO_PULLUP);
	
	pio_configure(XBUT3_PIO, PIO_INPUT, XBUT3_PIO_IDX_MASK, PIO_PULLUP);
	
	//Set todos os LEDS
	pio_set(LED_PIO, LED_PIO_IDX_MASK);
	
	pio_set(XLED1_PIO, XLED1_PIO_IDX_MASK);
	
	pio_set(XLED2_PIO, XLED2_PIO_IDX_MASK);
	
	pio_set(XLED3_PIO, XLED3_PIO_IDX_MASK);
	
	//Set todos os OUTPUTS
	pio_set_output(BUZ_PIO, BUZ_PIO_IDX_MASK, 0, 0, 0);
	
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	
	pio_set_output(XLED1_PIO, XLED1_PIO_IDX_MASK, 0, 0, 0);
		
	pio_set_output(XLED2_PIO, XLED2_PIO_IDX_MASK, 0, 0, 0);
		
	pio_set_output(XLED3_PIO, XLED3_PIO_IDX_MASK, 0, 0, 0);
	
	//Set interruptores do projeto
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, BUT_PRIOR ); // Priority 1

	NVIC_EnableIRQ(XBUT1_PIO_ID);
	NVIC_SetPriority(XBUT1_PIO_ID, XBUT1_PRIOR); // Priority 2
	
	NVIC_EnableIRQ(XBUT2_PIO_ID);
	NVIC_SetPriority(XBUT2_PIO_ID, XBUT2_PRIOR); // Priority 2
	
	NVIC_EnableIRQ(XBUT3_PIO_ID);
	NVIC_SetPriority(XBUT3_PIO_ID, XBUT3_PRIOR); // Priority 2


	pio_enable_interrupt(BUT_PIO, BUT_PIO_IDX_MASK);
	pio_enable_interrupt(XBUT1_PIO, XBUT1_PIO_IDX_MASK);
	pio_enable_interrupt(XBUT2_PIO, XBUT2_PIO_IDX_MASK);
	pio_enable_interrupt(XBUT3_PIO, XBUT3_PIO_IDX_MASK);

	pio_handler_set(BUT_PIO, BUT_PIO_ID, BUT_PIO_IDX_MASK, PIO_IT_FALL_EDGE, BUT_callback);
	pio_handler_set(XBUT1_PIO, XBUT1_PIO_ID, XBUT1_PIO_IDX_MASK, PIO_IT_FALL_EDGE, XBUT1_callback);
	pio_handler_set(XBUT2_PIO, XBUT2_PIO_ID, XBUT2_PIO_IDX_MASK, PIO_IT_FALL_EDGE, XBUT2_callback);
	pio_handler_set(XBUT3_PIO, XBUT3_PIO_ID, XBUT3_PIO_IDX_MASK, PIO_IT_FALL_EDGE, XBUT3_callback);
	
	
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
	init();
	
	//Condicao do pause
	int pause = 1;
	
	// Botão play/pause  (BUTTON Board)
	BUT_flag = 0;
	// Musica 1  (BUTTON 1)
	XBUT1_flag = 0;
	// Musica 2	 (BUTTON 2)
	XBUT2_flag = 0;
	// Musica 3  (BUTTON 3)
	XBUT3_flag = 0;
	
	int last_n = 0;
	int last_but = 0;
	
	song s1, s2, s3;
	
	new_song(s1,pirate_notes,pirate_tempo);
	new_song(s2,imperial_march_notes,imperial_march_tempo);
	new_song(s3,underworld_melody,underworld_tempo);

	// super loop
	// aplicacoes embarcadas não devem sair do while(1).
	
	while (1)
	{	
		pio_set(XLED1_PIO, XLED1_PIO_IDX_MASK);
		pio_set(XLED2_PIO, XLED2_PIO_IDX_MASK);
		pio_set(XLED3_PIO, XLED3_PIO_IDX_MASK);
		

		
		if (pause) pmc_sleep(SAM_PM_SMODE_SLEEP_WFI); // Espera ate que um interrupitor ligue
		
		if (XBUT1_flag){
			pause = 0;
			XBUT1_flag = 0;
			last_but=0;
			pio_clear(XLED1_PIO, XLED1_PIO_IDX_MASK);
			int len = sizeof(pirate_notes)/sizeof(int);
			norm_play(s1.notes,s1.tempos,CORTEMP_PC,&last_but,&last_n,&pause,len);
		}
		if (XBUT2_flag){
			pause = 0;
			XBUT2_flag = 0;
			last_but=0;
			pio_clear(XLED2_PIO, XLED2_PIO_IDX_MASK);
			int len = sizeof(imperial_march_notes)/sizeof(int);
			norm_play(s2.notes,s2.tempos,CORTEMP_SW,&last_but,&last_n,&pause,len);
		}
		if (XBUT3_flag){
			pause = 0;
			XBUT3_flag = 0;
			last_but=0;
			pio_clear(XLED3_PIO, XLED3_PIO_IDX_MASK);
			int len = sizeof(underworld_melody)/sizeof(int);
			norm_play(s3.notes,s3.tempos,CORTEMP_UW,&last_but,&last_n,&pause,len);
		}
		if (BUT_flag){
			BUT_flag = 0;
			if (pause == 1){
				if (last_but!=0)
				{
					if (last_but==1)
					{
						pio_clear(XLED1_PIO, XLED1_PIO_IDX_MASK);
						int len = sizeof(pirate_notes)/sizeof(int);
						pause_play(s1.notes,s1.tempos,CORTEMP_PC,&last_but,&last_n,&pause,len);
					}
					if (last_but==2){
						pio_clear(XLED2_PIO, XLED2_PIO_IDX_MASK);
						int len = sizeof(imperial_march_notes)/sizeof(int);
						pause_play(s2.notes,s2.tempos,CORTEMP_SW,&last_but,&last_n,&pause,len);
					}
					if (last_but==3){
						pio_clear(XLED3_PIO, XLED3_PIO_IDX_MASK);
						int len = sizeof(underworld_melody)/sizeof(int);
						pause_play(s3.notes,s3.tempos,CORTEMP_UW,&last_but,&last_n,&pause,len);
					}
				}
				else{
					pause = 0;
				}
				
			}
			else {
				pause = 1;
			}
			
		}
		
		
	}
	return 0;
}
