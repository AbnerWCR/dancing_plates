#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
//#include <stdlib.h>

#define NUM_PLATES 9
#define RECOVER_ENERGY 0.020

const float FPS = 60;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int GRASS_H = 150;

const int STICK_W = 6;
const int STICK_H = 250;

const int PLATE_W = 25;
const int PLATE_H = 5;
const int SPACE_BETWEEN = 40;

const float PLAYER_W = 80;
const float PLAYER_H = 40;

typedef struct Player {	
	float x, y;
	int balancing;
	int mov_l, mov_r;
	ALLEGRO_COLOR color;
	float speed;
	float score;
	
} Player;

typedef struct Plate {
	float x, y;
	
	/* um valor entre 0 e 1, em que 
		0 = prato equilibrado e
	   	1 = prato com maxima energia, prestes a cair */
	float energy;
	int time_to_show;//segundos
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR stick_color;
	
} Plate;

void drawBackdrop();

void initPlayer(Player *player);

void drawPlayer(Player player);

void updatePlayer(Player *player);

int getPlateTime(int i);

void initPlates(Plate *plates);

void drawPlates(Plate *plates);

void statusPlate(Plate *plates);

float getPoints(int seconds);

void updatePlate(Plate *plates, int seconds);

void addPointsPlayer(Player *player, int seconds);

void setDefaultStickColor(Plate *plates);

void resetPlates(Plate *plates, Player player);

int checkPlates(Plate *plates);

void drawScore(ALLEGRO_FONT *font, Player player);

void drawFinalScreen(ALLEGRO_FONT *font, Player player, int is_record);

int decision(int x, int y);

int setRecorde(Player player);
 
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

	al_set_window_title(display, "Dancing Plates");
	
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	//instala o teclado
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to install mouse!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	ALLEGRO_FONT *size_14 = al_load_font("arial.ttf", 14, 1);   
	if(size_14 == NULL) {
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
	al_register_event_source(event_queue, al_get_mouse_event_source());
	
	//PLAYER
	Player player;
	initPlayer(&player);
	
	//PLATES
	Plate *plates = (Plate*)malloc(NUM_PLATES*sizeof(Plate));
	initPlates(plates);

	//inicia o temporizador
	al_start_timer(timer);	

	int end_game = 1;
	int i;
	int playing = 1;
	while(playing) {		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);
		
		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER && end_game != 2) {
			if(end_game == 0){
				printf("\nCaiu no end_game 0");
				free(plates);
				al_stop_timer(timer);

				int is_record = 0;
				is_record = setRecorde(player);
				drawFinalScreen(size_14, player, is_record);
				al_flip_display();
				
				continue;
			}		

			int status = 0;
			status = checkPlates(plates);
			if (status == 0){
				end_game = 0;
			}

			drawBackdrop();		
			drawScore(size_12, player);	
			updatePlayer(&player);			
			drawPlayer(player);	
			resetPlates(plates, player);				
			drawPlates(plates);
			setDefaultStickColor(plates);

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			int segundos = (int)(al_get_timer_count(timer)/FPS);
			addPointsPlayer(&player, segundos);

			if(al_get_timer_count(timer)%(int)FPS == 0){
				printf("\n%d segundos se passaram...", segundos);
				updatePlate(plates, segundos);
			}
		}
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			if (end_game == 0)
				playing = 0;
			else 
				end_game = 0;
		}		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN && end_game != 2) {
			switch (ev.keyboard.keycode){
				case ALLEGRO_KEY_A:
					player.mov_l = 1;
					break;
				case ALLEGRO_KEY_D:
					player.mov_r = 1;
					break;
				case ALLEGRO_KEY_SPACE:					
					player.balancing = 1;
					break;
			}			
		}
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP && end_game != 2) {
			switch (ev.keyboard.keycode){
				case ALLEGRO_KEY_A:
					player.mov_l = 0;
					break;
				case ALLEGRO_KEY_D:
					player.mov_r = 0;
					break;
				case ALLEGRO_KEY_SPACE:
					player.balancing = 0;
					break;
			}			
		}	
		// click mouse
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && end_game != 1){
			int player_decision = decision(ev.mouse.x, ev.mouse.y);

			if(player_decision == 2){
				end_game = player_decision;
				continue;
			}
			else if(player_decision == 1){
				playing = player_decision;
				end_game = player_decision;

				//PLAYER
				initPlayer(&player);

				//PLATES
				free(plates);
				plates = (Plate*)malloc(NUM_PLATES*sizeof(Plate));
				initPlates(plates);

				al_destroy_timer(timer);
				timer = al_create_timer(1.0 / FPS);
				if(!timer) {
					fprintf(stderr, "failed to create timer!\n");
					return -1;
				}

				al_register_event_source(event_queue, al_get_timer_event_source(timer));

				//inicia o temporizador
				al_start_timer(timer);
			}
			else{
				playing = 0;
			}
		}
	}
	free(plates);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
 
	return 0;
}

int getPlateTime(int i) {
	return i*6;
}

int checkPlates(Plate *plates){
	int i;
	for(i = 0; i < NUM_PLATES; i++){
		if(plates[i].energy >= 1){
			return 0;
		}
	}
	return 1;
}

float getPoints(int seconds){
	float points = 0;

	if(seconds <= 20)
		points = (float)0.016;
	else if(seconds > 20 && seconds <= 40)
		points = (float)0.02;
	else if(seconds > 40 && seconds <= 80)
		points = (float)0.03;
	else if(seconds > 80 && seconds <= 180)
		points = (float)0.05;
	else if(seconds > 180 && seconds <= 200)
		points = (float)0.07;
	else if(seconds > 200)
		points = (float)0.09;

	return points;
}

int decision(int x, int y){
	if(!(y >= SCREEN_H/2 + 20 && y <= SCREEN_H/2 + 45))
		return 2;
	if(x >= SCREEN_W/2 - 145 && x <= SCREEN_W/2 - 5)
		return 0;
	if(x >= SCREEN_W/2+5 && x <= SCREEN_W/2 + 140)
		return 1;
	return 2;
}

int setRecorde(Player player){
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

	if(player.score < record){
		fclose(fp);
		return 0;
	}

	char *score = (char*)malloc(10001*sizeof(char));
	sprintf(score, "%.3f\n", player.score);
	fputs(score, fp);
	
	free(score);
	fclose(fp);
	return 1;
}

void drawBackdrop() {
	
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

void drawScore(ALLEGRO_FONT *font, Player player){
	char *score = (char*)malloc(10001*sizeof(char));
	sprintf(score, "%.3f", player.score);

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

void addPointsPlayer(Player *player, int seconds){
	player->score += getPoints(seconds);
}

void initPlayer(Player *player) {
	player->x = SCREEN_W / 2;
	player->y = (SCREEN_H - GRASS_H / 5) - PLAYER_H;
	player->balancing = 0;
	player->color = al_map_rgb(0, 0, 102);//al_map_rgb(25, 0, 51);
	player->mov_l = 0;
	player->mov_r = 0;
	player->speed = 2;
	player->score = (float)0;
}

void drawPlayer(Player player) {
	
	al_draw_filled_triangle(player.x, player.y, 
							player.x - PLAYER_W/2, player.y + PLAYER_H,
							player.x + PLAYER_W/2, player.y + PLAYER_H,
							player.color);	
	
}

void updatePlayer(Player *player) {
	if(player->mov_l) {
		if(player->x - player->speed > 0)
			player->x -= player->speed;
	}
	if(player->mov_r) {
		if(player->x + player->speed < SCREEN_W)
			player->x += player->speed;
	}	
}

void initPlates(Plate *plates) {	
	printf("\nIniciando pratos\n");
	int i;
	for(i=0; i<NUM_PLATES; i++) {
		float x = 0;
		if(i%2 == 0){
			x = (SCREEN_W / 2) + (i/2 * (PLATE_W + SPACE_BETWEEN));
		}
		else{
			x = (SCREEN_W / 2) - ((i/2 + 1) * (PLATE_W + SPACE_BETWEEN));
		}
		if (x < PLATE_W || x > SCREEN_W - PLATE_W){
			printf("\nExcede o tamanho da tela!");
			printf("\nlimite de %d pratos", i-1);
			break;
		}

		printf("\nprato %d - x: %f", i, x);
		printf("\ntempo para aparecer: %d", getPlateTime(i));
		plates[i].x = x;
		plates[i].y = (SCREEN_H - GRASS_H / 5) - STICK_H;
		plates[i].time_to_show = getPlateTime(i);
		plates[i].energy = 0;
		plates[i].color = al_map_rgb(204, 255, 255);
		plates[i].stick_color = al_map_rgb(51, 25, 0);
	}
	printf("\nFinalizando pratos\n");
}

void drawPlates(Plate *plates){
	
	int i;
	for(i = 0; i<NUM_PLATES; i++){
		if (plates[i].time_to_show > 0)
			continue;

		al_draw_filled_rectangle(plates[i].x - STICK_W/2, (SCREEN_H - GRASS_H / 5) - STICK_H,
								plates[i].x + STICK_W/2, (SCREEN_H - GRASS_H / 5) - PLAYER_H,
								plates[i].stick_color);

		al_draw_filled_ellipse(plates[i].x, plates[i].y, 
								PLATE_W, PLATE_H,
								plates[i].color);
	}
}

void updatePlate(Plate *plates, int seconds){
	int i;
	for(i = 0; i < NUM_PLATES; i++){
		if(plates[i].time_to_show <= 0)
			continue;
		plates[i].time_to_show--;
	}

	for(i = 0; i < NUM_PLATES; i++){
		if(plates[i].time_to_show > 0)
			continue;	

		if(plates[i].energy + getPoints(seconds) > 1)
			plates[i].energy = 1;
		else
			plates[i].energy += getPoints(seconds);
	}
	statusPlate(plates);
}

void statusPlate(Plate *plates){
	int i;
	for(i = 0; i < NUM_PLATES; i++){
		if(plates[i].time_to_show > 0)
			continue;

		if(plates[i].energy < 0.3){
			plates[i].color = al_map_rgb(204, 255, 255);
		} 
		if(plates[i].energy >= 0.3 && plates[i].energy < 0.5){
				plates[i].color = al_map_rgb(255, 102, 102);//warning
		}
		if(plates[i].energy >= 0.5 && plates[i].energy < 0.8){
				plates[i].color = al_map_rgb(255, 51, 51);//warning 2
		}
		if(plates[i].energy >= 0.8 && plates[i].energy < 1){
			plates[i].color = al_map_rgb(153, 0, 0);//danger
		}
		if(plates[i].energy >= 1){
			plates[i].color = al_map_rgb(0, 0, 0);//lose
			plates[i].y =  (SCREEN_H - GRASS_H / 5) - PLAYER_H;
		}
	}
}

void resetPlates(Plate *plates, Player player){
	if(player.mov_r != 0 || player.mov_l != 0 || player.balancing == 0)
		return;
	
	ALLEGRO_COLOR original_color_stick = al_map_rgb(51, 25, 0);
	int i;
	for(i = 0; i < NUM_PLATES; i++){
		if(player.x >= plates[i].x - 5 && player.x <= plates[i].x + 5 && plates[i].energy < 1){
			if(plates[i].energy - (float)RECOVER_ENERGY < 0)
				plates[i].energy = 0;
			else 
				plates[i].energy -= (float)RECOVER_ENERGY;

			ALLEGRO_COLOR success = al_map_rgb(102, 0, 204);
			plates[i].stick_color = success;
		}
	}	
}

void setDefaultStickColor(Plate *plates){
	ALLEGRO_COLOR default_color_stick = al_map_rgb(51, 25, 0);
	int i;
	for(i = 0; i < NUM_PLATES; i++){
		plates[i].stick_color = default_color_stick;
	}
}

void drawFinalScreen(ALLEGRO_FONT *font, Player player, int is_record){
	char *score = (char*)malloc(10001*sizeof(char));
	sprintf(score, "%.3f", player.score);
	char *text = (char*)malloc(50*sizeof(char));

	if(is_record == 1){			
		strcpy(text, "Novo recorde: ");
		strcat(text, score);
	}
	else{
		strcpy(text, "Score final: ");
		strcat(text, score);
	}
	al_draw_filled_rectangle(SCREEN_W/2 - 150, SCREEN_H/2 - 60,
							SCREEN_W/2 + 150, SCREEN_H/2 + 60,
							al_map_rgb(0, 0, 0));
	al_draw_text(font,
				al_map_rgb(255, 255, 255), SCREEN_W/2 - 100, SCREEN_H/2 - 30, ALLEGRO_ALIGN_LEFT,
				text);

	al_draw_filled_rectangle(SCREEN_W/2 - 145, SCREEN_H/2 + 20,
							SCREEN_W/2 - 5, SCREEN_H/2 + 45,
							al_map_rgb(255, 255, 255));
	al_draw_text(font,
				al_map_rgb(0, 0, 0), SCREEN_W/2 - 140, SCREEN_H/2 + 25, ALLEGRO_ALIGN_LEFT,
				"Sair");

	al_draw_filled_rectangle(SCREEN_W/2+5, SCREEN_H/2 + 20,
							SCREEN_W/2 + 140, SCREEN_H/2 + 45,
							al_map_rgb(255, 255, 255));
	al_draw_text(font,
				al_map_rgb(0, 0, 0), SCREEN_W/2 + 10, SCREEN_H/2 + 25, ALLEGRO_ALIGN_LEFT,
				"Jogar novamente");
	free(text);
	free(score);
}