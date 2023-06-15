#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

const float FPS = 60;  

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int SPACE_W = 540;
const int SPACE_H = 300;

const int GRASS_W = 540;
const int GRASS_H = 250;

const int PLAYER_W = 20;
const int PLAYER_H = 40;


const int PLATE_W = 30;
const int PLATE_H = 10;

const int STICK_W = 5;
const int STICK_H = 160;

typedef struct Plate {
	float x, y;
	int is_up;
	ALLEGRO_TIMER *timer;
	ALLEGRO_COLOR cor;
	ALLEGRO_COLOR cor_stick;
} Plate;

typedef struct Player {
	float x, y;
	float x_vel, y_vel;
	float score;
	int dir, esq, cima, baixo;
	ALLEGRO_COLOR cor;
} Player;

void destroy_game(ALLEGRO_TIMER *timer, ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue);
void draw_score(ALLEGRO_FONT *font, char *text);
void draw_final_score(ALLEGRO_FONT *font, char *text);
void draw_cenario();

void init_plate(Plate *plate);
void draw_plate(Plate plate);
int change_plate(Plate *plate);
int reset_plate(Plate *plate, Player player);

void init_player(Player *player);
void draw_player(Player player);
void update_player(Player *player);

int main(int argc, char **argv){
    ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

    //----------------------- rotinas de inicializacao ---------------------------------------
    //inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

    //inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	

    //cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
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

	Plate plate;
	init_plate(&plate);
	al_start_timer(plate.timer);

	Player player;
	init_player(&player);

    //inicia o temporizador
	al_start_timer(timer);

    int playing = 1;
	while(playing) {
        ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

        //se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			draw_cenario();

			char *pontuacao = (char*)malloc(10001*sizeof(char));
			sprintf(pontuacao, "%.2f", player.score);
			draw_score(size_12, pontuacao);

			draw_plate(plate);

			update_player(&player);
			draw_player(player);

			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0){
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
				player.score += (0.01*al_get_timer_count(timer)/FPS);

				if (player.score >= 0.1){
					char *pontuacao_final = (char*)malloc(10001*sizeof(char));
					sprintf(pontuacao_final, "%.2f", player.score);

					printf("Fim de jogo!");
					printf("Total de pontos: %.2f", player.score);
					draw_final_score(size_12, pontuacao);
					al_rest(10);
					return 0;
				}
			}

			plate.is_up = change_plate(&plate);
			playing = plate.is_up;
		}
        //se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
        //se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			int sucesso = 0;
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_A:
					player.esq = 1;
					break;
				case ALLEGRO_KEY_D:					
					player.dir = 1;
					break;
				case ALLEGRO_KEY_W:
					player.cima = 1;
					break;
				case ALLEGRO_KEY_S:					
					player.baixo = 1;
					break;
				case ALLEGRO_KEY_SPACE:
					sucesso = reset_plate(&plate, player);
					if (sucesso < 0)
						return sucesso;
					break;
			}
		}    
		//se o tipo de evento for um soltar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
				case ALLEGRO_KEY_A:
					player.esq = 0;
					break;
				case ALLEGRO_KEY_D:					
					player.dir = 0;
					break;
				case ALLEGRO_KEY_W:
					player.cima = 0;
					break;
				case ALLEGRO_KEY_S:					
					player.baixo = 0;
					break;
			}
		}    
    }

    //procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)
    destroy_game(timer, display, event_queue);

	return 0;
}

void draw_final_score(ALLEGRO_FONT *font, char *score){
	char *text = (char*)malloc(50*sizeof(char));
	strcpy(text, "Pontuacao total: ");
	strcat(text, score);
	float x_ini = SCREEN_W/2 - 50;
	float y_ini = SCREEN_H/2 - 50;
	al_draw_filled_rectangle(x_ini, y_ini,
							x_ini + 100, y_ini + 100,
							al_map_rgb(0, 0, 0));
	al_draw_text(font,
				al_map_rgb(255, 255, 255), x_ini + 10, y_ini + 10, ALLEGRO_ALIGN_LEFT,
				text);
}

void draw_score(ALLEGRO_FONT *font, char *score){
	char *text = (char*)malloc(50*sizeof(char));
	strcpy(text, "Score: ");
	strcat(text, score);
	al_draw_filled_rectangle(0, 0,
							100, 30,
							al_map_rgb(0, 0, 0));
	al_draw_text(font,
				al_map_rgb(255, 255, 255), 2, 2, ALLEGRO_ALIGN_LEFT,
				text);
}

void destroy_game(ALLEGRO_TIMER *timer, ALLEGRO_DISPLAY *display, ALLEGRO_EVENT_QUEUE *event_queue){
    al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
}

void draw_cenario(){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_filled_rectangle(0, 0,
							SCREEN_W, SPACE_H,
							al_map_rgb(102, 178, 255));
	al_draw_filled_rectangle(0, SPACE_H,
							SCREEN_W, SCREEN_H,
							al_map_rgb(51, 255, 51));
}

void init_plate(Plate *plate){
	plate->x = SCREEN_W/2;
	plate->y = SCREEN_H - GRASS_H/2;
	plate->is_up = 1;
	plate->timer = al_create_timer(1.0 / FPS);
	plate->cor = al_map_rgb(102, 0, 204);
	plate->cor_stick = al_map_rgb(51, 0, 0);
}

void draw_plate(Plate plate){
	al_draw_filled_rectangle(plate.x, plate.y, 
							plate.x - STICK_W, plate.y - STICK_H,
							plate.cor_stick);

	al_draw_filled_ellipse(plate.x - STICK_W, plate.y - STICK_H, 
							PLATE_W, PLATE_H,
   							plate.cor);
}

int change_plate(Plate *plate){
	if(al_get_timer_count(plate->timer)%(int)FPS == 0){
			int plate_timer = (int)(al_get_timer_count(plate->timer)/FPS);
			if (plate_timer >= 10 && plate_timer < 15){
				plate->cor = al_map_rgb(204, 102, 0);//warning
			}
			else if (plate_timer >= 15 && plate_timer < 20){
				plate->cor = al_map_rgb(153, 0, 0);//danger
			}
			else if (plate_timer > 20){
				plate->cor = al_map_rgb(0, 0, 0);//lose
				return 0;
			}
		}
	return 1;
}

void init_player(Player *player){
	player->x = SCREEN_W/2;
	player->y = SCREEN_H;
	player->x_vel = 2;
	player->y_vel = 2;
	player->dir = 0;
	player->esq = 0;
	player->cima = 0;
	player->baixo = 0;
	player->score = 0;
	player->cor = al_map_rgb(0, 25, 82);
}

void draw_player(Player player){
	al_draw_filled_rectangle(player.x, player.y, 
							player.x - PLAYER_W, player.y - PLAYER_H,
							player.cor);
}

void update_player(Player *player){
	if(player->dir && (player->x + player->x_vel <= SCREEN_W)){
		player->x += player->x_vel;
	} 
	if(player->esq && (player->x - player->x_vel >= PLAYER_W)){
		player->x -= player->x_vel;
	}

	if(player->cima && (player->y - player->y_vel - PLAYER_H >= SPACE_H)){
		player->y -= player->y_vel;
	}
	if(player->baixo && (player->y + player->y_vel <= SCREEN_H)){
		player->y += player->y_vel;
	}
}

int reset_plate(Plate *plate, Player player){
	if (player.dir != 0 || player.esq != 0 || player.cima != 0 || player.baixo != 0)
		return 0;
	if(player.x >= plate->x - 5 && player.x <= plate->x + 5){
		plate->cor = al_map_rgb(102, 0, 204);//ok
		al_destroy_timer(plate->timer);

		plate->timer = al_create_timer(1.0 / FPS);
		if(!plate->timer) {
			fprintf(stderr, "failed to create timer!\n");
			return -1;
		}

		al_start_timer(plate->timer);
		return 1;
	}
	else if(player.y >= plate->y - 5 && player.y <= plate->y + 5){
		plate->cor = al_map_rgb(102, 0, 204);//ok
		al_destroy_timer(plate->timer);

		plate->timer = al_create_timer(1.0 / FPS);
		if(!plate->timer) {
			fprintf(stderr, "failed to create timer!\n");
		}

		al_start_timer(plate->timer);
		return 1;
	}

	return 0;
}
