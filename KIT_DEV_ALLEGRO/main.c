#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
//#include <stdlib.h>

#define NUM_PRATOS 9

const float FPS = 60;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int GRASS_H = 150;

const int STICK_W = 5;
const int STICK_H = 150;

const int PRATO_W = 20;
const int PRATO_H = 5;
const int SPACE_BETWEEN = 40;

const float JOGADOR_W = 80;
const float JOGADOR_H = 40;

typedef struct Jogador {	
	float x, y;
	int equilibrando;
	int mov_esq, mov_dir;
	ALLEGRO_COLOR cor;
	float vel;
	float score;
	
} Jogador;

typedef struct Prato {
	float x;
	
	/* um valor entre 0 e 1, em que 
		0 = prato equilibrado e
	   	1 = prato com maxima energia, prestes a cair */
	float energia;
	int tempoParaAparecer;//segundos
	ALLEGRO_COLOR cor;
	ALLEGRO_COLOR stick_color;
	
} Prato;


void desenha_cenario() {
	
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0,76,153);
	ALLEGRO_COLOR GRASS_COLOR = al_map_rgb(51, 255, 51);

	//desenhando ceu
	al_draw_filled_rectangle(0, 0,
							SCREEN_W, SCREEN_H - GRASS_H,
							BKG_COLOR);	
	
	//desenhando grama
	al_draw_filled_rectangle(0, SCREEN_H - GRASS_H,
							SCREEN_W, SCREEN_H,
							GRASS_COLOR);	
}

void InicializaJogador(Jogador *j) {
	j->x = SCREEN_W / 2;
	j->y = (SCREEN_H - GRASS_H / 5) - JOGADOR_H;
	j->equilibrando = 0;
	j->cor = al_map_rgb(0, 0, 102);//al_map_rgb(25, 0, 51);
	j->mov_esq = 0;
	j->mov_dir = 0;
	j->vel = 2;
	j->score = (float)0;
}

void desenha_jogador(Jogador j) {
	
	al_draw_filled_triangle(j.x, j.y, 
							j.x - JOGADOR_W/2, j.y + JOGADOR_H,
							j.x + JOGADOR_W/2, j.y + JOGADOR_H,
							j.cor);	
	
}

void atualizaJogador(Jogador *j) {
	if(j->mov_esq) {
		if(j->x - j->vel > 0)
			j->x -= j->vel;
	}
	if(j->mov_dir) {
		if(j->x + j->vel < SCREEN_W)
			j->x += j->vel;
	}	
}

int geraTempoPrato(int i) {
	return i*6;
}

void inicializaPratos(Prato *pratos) {	
	printf("\nIniciando pratos\n");
	int i;
	for(i=0; i<NUM_PRATOS; i++) {
		float x = 0;
		if(i%2 == 0){
			x = (SCREEN_W / 2) + (i/2 * (PRATO_W + SPACE_BETWEEN));
		}
		else{
			x = (SCREEN_W / 2) - ((i/2 + 1) * (PRATO_W + SPACE_BETWEEN));
		}
		if (x < PRATO_W || x > SCREEN_W - PRATO_W){
			printf("\nExcede o tamanho da tela!");
			printf("\nlimite de %d pratos", i-1);
			break;
		}

		printf("\nprato %d - x: %f", i, x);
		printf("\ntempo para aparecer: %d", geraTempoPrato(i));
		pratos[i].x = x;
		pratos[i].tempoParaAparecer = geraTempoPrato(i);
		pratos[i].energia = 0;
		pratos[i].cor = al_map_rgb(204, 255, 255);
		pratos[i].stick_color = al_map_rgb(51, 25, 0);
	}
	printf("\nFinalizando pratos\n");
}

void desenhar_pratos(Prato *pratos){
	
	int i;
	for(i = 0; i<NUM_PRATOS; i++){
		if (pratos[i].tempoParaAparecer > 0)
			continue;

		al_draw_filled_rectangle(pratos[i].x, (SCREEN_H - GRASS_H / 5) - STICK_H,
								pratos[i].x + STICK_W, (SCREEN_H - GRASS_H / 5) - JOGADOR_H,
								pratos[i].stick_color);

		al_draw_filled_ellipse(pratos[i].x, (SCREEN_H - GRASS_H / 5) - STICK_H, 
								PRATO_W, PRATO_H,
								pratos[i].cor);
	}
}

void status_prato(Prato *pratos){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(pratos[i].tempoParaAparecer > 0)
			continue;

		if(pratos[i].energia < 0.3){
			pratos[i].cor = al_map_rgb(204, 255, 255);
		} 
		if(pratos[i].energia >= 0.3 && pratos[i].energia < 0.5){
				pratos[i].cor = al_map_rgb(255, 102, 102);//warning
		}
		if(pratos[i].energia >= 0.5 && pratos[i].energia < 0.8){
				pratos[i].cor = al_map_rgb(255, 51, 51);//warning 2
		}
		if(pratos[i].energia >= 0.8 && pratos[i].energia < 1){
			pratos[i].cor = al_map_rgb(153, 0, 0);//danger
		}
		if(pratos[i].energia >= 1){
			pratos[i].cor = al_map_rgb(0, 0, 0);//lose
		}
	}
}

float get_pontos(int segundos){
	float pontos = 0;

	if(segundos <= 20)
		pontos = (float)0.016;
	else if(segundos > 20 && segundos <= 40)
		pontos = (float)0.02;
	else if(segundos > 40 && segundos <= 80)
		pontos = (float)0.03;
	else if(segundos > 80)
		pontos = (float)0.5;

	return pontos;
}

void update_prato(Prato *pratos, int segundos){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(pratos[i].tempoParaAparecer <= 0)
			continue;
		pratos[i].tempoParaAparecer--;
	}

	for(i = 0; i < NUM_PRATOS; i++){
		if(pratos[i].tempoParaAparecer > 0)
			continue;	

		pratos[i].energia += get_pontos(segundos);
	}
	status_prato(pratos);
}

void add_points_jogador(Jogador *j, int segundos){
	j->score += get_pontos(segundos);
}

void set_default_stick_color(Prato *pratos){
	ALLEGRO_COLOR default_color_stick = al_map_rgb(51, 25, 0);
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		pratos[i].stick_color = default_color_stick;
	}
}

void reset_plates(Prato *pratos, Jogador j){
	if(j.mov_dir != 0 || j.mov_esq != 0)
		return;
	
	ALLEGRO_COLOR original_color_stick = al_map_rgb(51, 25, 0);
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(j.x >= pratos[i].x - 5 && j.x <= pratos[i].x + 5 && pratos[i].energia < 1){
			pratos[i].energia = 0;

			ALLEGRO_COLOR success = al_map_rgb(102, 0, 204);
			pratos[i].stick_color = success;
		}			
	}
	desenhar_pratos(pratos);
	al_flip_display();
	set_default_stick_color(pratos);
	al_rest(0.5);
}

int check_plates(Prato *pratos){
	int i;
	for(i = 0; i < NUM_PRATOS; i++){
		if(pratos[i].energia >= 1){
			return 0;
		}
	}
	return 1;
}

void draw_score(ALLEGRO_FONT *font, Jogador jogador){
	char *score = (char*)malloc(10001*sizeof(char));
	sprintf(score, "%.3f", jogador.score);

	char *text = (char*)malloc(50*sizeof(char));
	strcpy(text, "Score: ");
	strcat(text, score);
	al_draw_filled_rectangle(0, 0,
							100, 30,
							al_map_rgb(0, 0, 0));
	al_draw_text(font,
				al_map_rgb(255, 255, 255), 2, 2, ALLEGRO_ALIGN_LEFT,
				text);
	free(score);
	free(text);
}

void set_record(Jogador jogador){
	FILE *fp;
	fp = fopen("recorde_jogador.txt", "a+");
	if(fp == NULL){
		printf("Erro ao abrir arquivo!");
		return;
	}

	float value = (float)0;
	float record = (float)0;
	while((fscanf(fp, "%f", &value)) != EOF){
		printf("\nScore recorde: %f", value);
		if (record < value)
			record = value;

		if(feof(fp) || record == 0)
			break;
	}
	printf("\nrecorde: %.3f", record);

	if(jogador.score < record)
		return;

	char *score = (char*)malloc(10001*sizeof(char));
	sprintf(score, "%.3f\n", jogador.score);
	fputs(score, fp);
	
	fclose(fp);
}
 
int main(int argc, char **argv){
	
	ALLEGRO_DISPLAY *display = NULL;	
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}	
	
    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}	
	
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}	
	
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
    ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);   
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}	

	ALLEGRO_FONT *size_12 = al_load_font("arial.ttf", 12, 1);   
	if(size_12 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}	
	
 	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}	
	
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source()); 	
	
	
	//JOGADOR
	Jogador jogador;
	InicializaJogador(&jogador);
	
	//PRATOS
	Prato *pratos = (Prato*)malloc(NUM_PRATOS*sizeof(Prato));
	inicializaPratos(pratos);
	
	//inicia o temporizador
	al_start_timer(timer);	
	
	int i;
	int playing=1;
	while(playing) {
		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {					
			int status = 0;
			status = check_plates(pratos);
			if (status == 0)
				break;

			desenha_cenario();		
			draw_score(size_12, jogador);	
			atualizaJogador(&jogador);			
			desenha_jogador(jogador);			
			desenhar_pratos(pratos);

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			int segundos = (int)(al_get_timer_count(timer)/FPS);
			add_points_jogador(&jogador, segundos);

			if(al_get_timer_count(timer)%(int)FPS == 0){
				printf("\n%d segundos se passaram...", segundos);
				update_prato(pratos, segundos);
			}
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode){
				case ALLEGRO_KEY_A:
					jogador.mov_esq = 1;
					break;
				case ALLEGRO_KEY_D:
					jogador.mov_dir = 1;
					break;
				case ALLEGRO_KEY_SPACE:
					printf("\nTecla: %d pressionada", ev.keyboard.keycode);
					reset_plates(pratos, jogador);
					break;
			}			
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode){
				case ALLEGRO_KEY_A:
					jogador.mov_esq = 0;
					break;
				case ALLEGRO_KEY_D:
					jogador.mov_dir = 0;
					break;
			}			
		}		
		
		
	}
	
	set_record(jogador);
	free(pratos);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
 
	return 0;
}