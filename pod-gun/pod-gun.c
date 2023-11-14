#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>

#include <GL/glut.h>

#include "./rand.h"
#include "./point.h"
#include "./print-lines.h"

const float GLOBAL_MOVE_RATE = 1.0;
const float XLIM = 700;
const float YLIM = 650;

bool KEYBOARD_STATUS[256];


/* PRINT-LINES::CONFIGS */
#define PRINT_BULLET_FOOTPRINTS 3
#define PRINT_BLOCK_FOOTPRINTS 4
#define PRINT_ROBO_DEFENSE 1
#define PRINT_PODGUN_DEFENSE 2

void *__THREAD_PRINT_LINES__(void *arg) {
	while(1) {
		sleep(1);
		__PRINT_LINES__(4);
	}
}

#define CARTIDGE_EMPTY 0
#define CARTIDGE_BULLET 1
#define CARTIDGE_BLOCK 2
#define CARTIDGE_SLOT_SIZE 3

#define OBJECTS_SIZE 1000


/* PODGUN::CONSTANTS */
const float SHAPE_SCALE = 7.0;
const float PODGUN_COLOR[3] = {0.10f, 0.80f, 0.10f};
const float PODGUN_POINT_SIZE = 5.0 * SHAPE_SCALE;
const float POD_WIDTH = 10.0 * SHAPE_SCALE;
const float POD_HEIGHT = 4.0 * SHAPE_SCALE;
const float GUN_WIDTH = 2.0 * SHAPE_SCALE;
const float GUN_HEIGHT = 3.5 * SHAPE_SCALE;
const float PODGUN_MOVE_SPEED = 0.025 * SHAPE_SCALE;
const float PODGUN_DEFENSE = 100.0f;
#define PODGUN_LINE_WIDTH 2.5f

/* CARTIDGE::CONSTANTS */
const float CARTIDGE_SLOT_PADDING = 1.0 * SHAPE_SCALE;
const float CARTIDGE_SLOT_WIDTH_PADDING = CARTIDGE_SLOT_PADDING; // *(POD_WIDTH/POD_HEIGHT);
const float CARTIDGE_SLOT_HEIGHT_PADDING = CARTIDGE_SLOT_PADDING; // *(POD_HEIGHT/POD_WIDTH);
const float CARTIDGE_SLOT_WIDTH = (POD_WIDTH-(CARTIDGE_SLOT_WIDTH_PADDING*(CARTIDGE_SLOT_SIZE+1)))/CARTIDGE_SLOT_SIZE;
const float CARTIDGE_SLOT_HEIGHT = (POD_HEIGHT-(CARTIDGE_SLOT_HEIGHT_PADDING*2));

/* BULLET::CONSTANTS */
const float BULLET_DAMAGE_VAL = 1.0f;
const float BULLET_MOVE_SPEED = 0.5f;
const float BULLET_WIDTH = GUN_WIDTH;
const float BULLET_HEIGHT = GUN_HEIGHT;
const float BULLET_COLOR[3] = {0.25f, 0.25f, 0.50f};

/* BLOCK::CONSTANTS */
const float BLOCK_DAMAGE_VAL = 0.005f;
const float BLOCK_MOVE_SPEED = 0.5f;
const float BLOCK_WIDTH = GUN_WIDTH;
const float BLOCK_HEIGHT = GUN_WIDTH;
const float BLOCK_ALPHA_DECREASE_RATE = 0.01f;
const float BLOCK_COLOR[3] = {0.50f, 0.25f, 0.25f};

const float ROBO_COLOR[3] = {0.50f, 0.30f, 0.20f};

// Print dets
unsigned int TOTAL_BULLETS_CREATED = 0;
unsigned int TOTAL_BULLETS_DESTROYED = 0;
unsigned int TOTAL_BLOCKS_CREATED = 0;
unsigned int TOTAL_BLOCKS_DESTROYED = 0;


/* PREDEFS */
// PODGUN::PREDEFS
typedef struct PodGun PodGun;
PodGun *newPodGun(Point position);
int _getRandomCartidgeSlot(PodGun *self);

// BULLET::PREDEFS
typedef struct Bullet Bullet;
Bullet *newBullet(Point position, float move_speed, char direction, PodGun *sender, PodGun *receiver);
void displayBullet(Bullet *bullet);

// BLOCK::PREDEFS
typedef struct Block Block;
Block *newBlock(Point position, float move_speed, char direction, PodGun *sender, PodGun *receiver);
void displayBlock(Block *block);

void __KEYBOARD_SPECIAL_FUNC__run(void);

PodGun *Player;
PodGun *Robo;


// PODGUN::STRUCT
typedef struct PodGun{
	Point bl;
	Point br;
	Point ur;
	Point umbr;
	Point umur;
	Point umul;
	Point umbl;
	Point ul;
	Point position;
	char cartidge[CARTIDGE_SLOT_SIZE];
	float defense;

	void (* render)(struct PodGun *self);
	void (* calculatePoints)(struct PodGun *self);
	void (* loadCartidge)(struct PodGun *self);
	void (* movePodGun)(struct PodGun *self, char hdirection);
	int (* _getRandomCartidgeSlot)(struct PodGun *self);
	void (* _renderCartidgeBullet)(struct PodGun *self, int slot_num);
	void (* _renderCartidgeBlock)(struct PodGun *self, int slot_num);
	int (* checkCartidgeAvailability)(PodGun *self, char cartidge_type);
	void (* releaseBullet)(PodGun *self, PodGun *enemy, char direction);
	void (* releaseBlock)(PodGun *self, PodGun *enemy, char direction);

	// ROBO
	void (* roboRender)(struct PodGun *robo);
	void (* roboCalculatePoints)(struct PodGun *robo);
	void (* _roboRenderCartidgeBullet)(struct PodGun *robo, int slot_num);
	void (* _roboRenderCartidgeBlock)(struct PodGun *robo, int slot_num);
	void (* moveRobo)(struct PodGun *robo, struct PodGun *player);
	void (* moveRoba)(struct PodGun *robo);

	void (* robaShoot)(PodGun *self, PodGun *enemy);
} PodGun;

// BULLET::STRUCT
typedef struct Bullet{
	Point bl;
	Point br;
	Point um;
	Point position;
	char direction;
	float move_speed;

	PodGun *sender;
	PodGun *receiver;

	void (* calculatePoints)(struct Bullet *self);
	int (* inferHit)(struct Bullet *self);
	void (* render)(struct Bullet *self);
	void (* move)(struct Bullet *self);

	struct Bullet *next;
} Bullet;

// BLOCK::STRUCT
typedef struct Block{
	Point bl;
	Point br;
	Point ur;
	Point ul;
	Point position;
	char direction;
	float alpha_val;
	float shape_scale;
	float move_speed;
	bool move_flag;
	int mf_p;
	PodGun *sender;
	PodGun *receiver;

	void (* calculatePoints)(struct Block *self);
	int (* inferHit)(struct Block *self);
	void (* render)(struct Block *self);
	void (* move)(struct Block *self);
	struct Block *next;
} Block;


/* B OBJECTS */
Bullet *BULLETS=NULL;
unsigned int lengthBullets(void) {
	unsigned int len=0;
	for(Bullet *c=BULLETS; c!=NULL; c=c->next) len += 1;
	return len;
}

void displayBullets(void) {
	printf("\n(disp bullets)\n");
	for(Bullet *c=BULLETS; c!=NULL; c=c->next)
		printf("%p\n", c);
}
void AddBullet(Bullet *bullet) {
	Bullet *current = BULLETS;
	if(current==NULL) {
		BULLETS = bullet;
		return;
	}
	while(current->next != NULL)
		current = current->next;
	current->next = bullet;
}

void DeleteBullet(Bullet *bullet) {
	if(BULLETS==bullet) {
		BULLETS = bullet->next;
		free(bullet);
		return;
	}
	Bullet *current = BULLETS;
	while(current!=NULL && current->next!=bullet)
		current = current->next;
	if(current!=NULL)
		current->next = bullet->next;
	free(bullet);
}

Block *BLOCKS=NULL;
unsigned int lengthBlocks(void) {
	unsigned int len=0;
	for(Block *c=BLOCKS; c!=NULL; c=c->next) len += 1;
	return len;
}
void AddBlock(Block *block) {
	Block *current = BLOCKS;
	if(current==NULL) {
		BLOCKS = block;
		return;
	}
	while(current->next != NULL)
		current = current->next;
	current->next = block;
}

void DeleteBlock(Block *block) {
	if(BLOCKS==block) {
		BLOCKS = block->next;
		free(block);
		return;
	}
	Block *current = BLOCKS;

	while(current!=NULL && current->next!=block)
		current = current->next;
	if(current!=NULL)
		current->next = block->next;
	free(block);
}

/* PODGUN */

int _getRandomCartidgeSlot(PodGun *self) {
	// CARTIDGE_BULLET=1, CARTIDGE_BLOCK=2
	return randInt(0,10)>9 ? CARTIDGE_BLOCK : CARTIDGE_BULLET;
}


void loadCartidge(PodGun *self) {
	for(int i=0; i<3; i++)
		if(self->cartidge[i]==CARTIDGE_EMPTY)
			self->cartidge[i] = (char) self->_getRandomCartidgeSlot(self);
}


void calculatePodGunPoints(PodGun *self) {
	// Position is the PodGun's lower edge

	float phw = ((POD_WIDTH)/2);
	float phh = ((POD_HEIGHT));
	float ghw = ((GUN_WIDTH)/2);
	float ghh = ((GUN_HEIGHT));

	// assign attributes values
	self->bl = newPoint(self->position.x-phw, self->position.y);
	self->br = newPoint(self->position.x+phw, self->position.y);
	self->ur = newPoint(self->position.x+phw, self->position.y+phh);
	self->umbr = newPoint(self->position.x+ghw, self->position.y+phh);
	self->umur = newPoint(self->position.x+ghw, self->position.y+phh+ghh);
	self->umul = newPoint(self->position.x-ghw, self->position.y+phh+ghh);
	self->umbl = newPoint(self->position.x-ghw, self->position.y+phh);
	self->ul = newPoint(self->position.x-phw, self->position.y+phh);
}

void _renderCartidgeBlock(PodGun *self, int slot_num) {
	Point bl = {
		self->bl.x+((CARTIDGE_SLOT_WIDTH_PADDING)*(slot_num)+(CARTIDGE_SLOT_WIDTH)*(slot_num-1)),
		self->bl.y+(CARTIDGE_SLOT_HEIGHT_PADDING)
	};

	Point br = {
		bl.x+(CARTIDGE_SLOT_WIDTH),
		bl.y
	};

	Point ur = {
		br.x,
		br.y+(CARTIDGE_SLOT_HEIGHT)
	};

	Point ul = {
		bl.x,
		ur.y
	};

	glBegin(GL_LINE_LOOP);
		glColor3f(BLOCK_COLOR[0], BLOCK_COLOR[1], BLOCK_COLOR[2]);
		glVertex2f(bl.x, bl.y);
		glVertex2f(br.x, br.y);
		glVertex2f(ur.x, ur.y);
		glVertex2f(ul.x, ul.y);
	glEnd();
}


void _renderCartidgeBullet(PodGun *self, int slot_num) {
	Point bl = {
		self->bl.x+((CARTIDGE_SLOT_WIDTH_PADDING)*(slot_num)+(CARTIDGE_SLOT_WIDTH)*(slot_num-1)),
		self->bl.y+(CARTIDGE_SLOT_HEIGHT_PADDING)
	};

	Point br = {
		bl.x+(CARTIDGE_SLOT_WIDTH),
		bl.y
	};

	Point um = {
		bl.x+((br.x-bl.x)/2),
		bl.y+(CARTIDGE_SLOT_HEIGHT)
	};

	glBegin(GL_LINE_LOOP);
		glColor3f(BULLET_COLOR[0], BULLET_COLOR[1], BULLET_COLOR[2]);
		glVertex2f(bl.x, bl.y);
		glVertex2f(br.x, br.y);
		glVertex2f(um.x, um.y);
	glEnd();
}

void dp(char *x, Point p) {
	//printf("%s.x:  %.2f, %s.y: %.2f\n", x, p.x, x, p.y);
}

void renderPodGun(PodGun *self) {
	static int count=0;
	// __ALLOCATE_PRINT__(RENDER_PODGUN, 20);
	// sprintf(PRINT_LINES[RENDER_PODGUN], "(podgun)# count: %10d", count++);

	self->calculatePoints(self);

	glLineWidth(PODGUN_LINE_WIDTH);
	glBegin(GL_LINE_LOOP);
		glColor3f(PODGUN_COLOR[0], PODGUN_COLOR[1], PODGUN_COLOR[3]);
		glPointSize(PODGUN_POINT_SIZE);
		glVertex2f(self->bl.x, self->bl.y);
		glVertex2f(self->br.x, self->br.y);
		glVertex2f(self->ur.x, self->ur.y);
		glVertex2f(self->umbr.x, self->umbr.y);
		glVertex2f(self->umur.x, self->umur.y);
		glVertex2f(self->umul.x, self->umul.y);
		glVertex2f(self->umbl.x, self->umbl.y);
		glVertex2f(self->ul.x, self->ul.y);
	glEnd();

	self->loadCartidge(self);
	for(int slot_num=1; slot_num<=CARTIDGE_SLOT_SIZE; slot_num++) {
		switch(self->cartidge[slot_num-1]) {
			case CARTIDGE_BULLET:
				self->_renderCartidgeBullet(self, slot_num);
				break;
			case CARTIDGE_BLOCK:
				self->_renderCartidgeBlock(self, slot_num);
				break;
			default:
				printf("# cartidge slot: %d empty found!\n", slot_num);
		}
	}
}

void movePodGun(PodGun *self, char hdirection) {
	switch(hdirection) {
		case 'l':
			if(self->bl.x-PODGUN_MOVE_SPEED >= 1.0f)
				self->position.x -= PODGUN_MOVE_SPEED;
			else
				self->position.x -= (self->bl.x-1.00f); // as bl.x is already been calculated based on POD WIDTH
			break;
		case 'r':
			if(self->br.x+PODGUN_MOVE_SPEED <= (XLIM-1.0f))
				self->position.x += PODGUN_MOVE_SPEED;
			else
				self->position.x += (XLIM-1.0f-self->br.x);
			break;
		default:
			printf("unknown podgun hdirection: %c\n", hdirection);
	}
}

// -1 for NO, >=0 means index
int checkCartidgeAvailability(PodGun *self, char cartidge_type) {
	for(int i=0; i<CARTIDGE_SLOT_SIZE; i++) 
		if(self->cartidge[i]==cartidge_type)
			return i;
	return -1;
}

void releaseBullet(PodGun *self, PodGun *enemy, char direction) {

	int indx = self->checkCartidgeAvailability(self, CARTIDGE_BULLET);
	if(indx<0) return;

	// new bullet at gun nozzel + (bullets_width, bullets_height)
	Point bullet_position = newPoint(
		self->umur.x+(self->umul.x-self->umur.x)/2, 
		self->umur.y+(BULLET_HEIGHT/2)
	);
	if(direction=='d')
		bullet_position.y -= (POD_HEIGHT+GUN_HEIGHT+(BULLET_HEIGHT*2));
	Bullet *bullet = newBullet(bullet_position, BULLET_MOVE_SPEED, direction, self, enemy);

	// commence bullet releasing
	// load new cartidge
	self->cartidge[indx] = self->_getRandomCartidgeSlot(self);
	AddBullet(bullet);
	TOTAL_BULLETS_CREATED += 1;
}

void releaseBlock(PodGun *self, PodGun *enemy, char direction) {
	int indx = self->checkCartidgeAvailability(self, CARTIDGE_BLOCK);
	if(indx<0) return;


	// new block at gun nozzle
	Point block_position = newPoint(
		self->umur.x+(self->umul.x-self->umur.x)/2,
		self->umur.y+(BLOCK_HEIGHT/2)
	);
	if(direction=='d')
		block_position.y  -= (POD_HEIGHT+GUN_HEIGHT+(BLOCK_HEIGHT*2));
	Block *block = newBlock(block_position, BLOCK_MOVE_SPEED, direction, self, enemy);

	// commence block releasing
	// load new cartidge
	self->cartidge[indx] = self->_getRandomCartidgeSlot(self);
	AddBlock(block);
	TOTAL_BLOCKS_CREATED += 1;
}

/* ============================= ROBO ============================== */

void roboCalculatePoints(PodGun *robo) {
	// Position is the Robo's upper edge

	float phw = ((POD_WIDTH)/2);
	float phh = ((POD_HEIGHT));
	float ghw = ((GUN_WIDTH)/2);
	float ghh = ((GUN_HEIGHT));

	// assign robo attributes values
	// robo->position.x = XLIM/2, robo->position.y = YLIM
	// assign attributes values
	/*
		YLIM              umul------------umur
							  |			 |
				gun			  |			 |		phh
							  |			 |
						  umbl----    ----umbr
							  	ul|  |ur
				pod 		  	  |	 |			ghh
							  	bl----br
	*/

	robo->bl = newPoint(robo->position.x-ghw, robo->position.y-(phh+ghh));
	robo->br = newPoint(robo->position.x+ghw, robo->position.y-(phh+ghh));
	robo->ur = newPoint(robo->position.x+ghw, robo->position.y-phh);
	robo->umbr = newPoint(robo->position.x+phw, robo->position.y-phh);
	robo->umur = newPoint(robo->position.x+phw, robo->position.y);
	robo->umul = newPoint(robo->position.x-phw, robo->position.y);
	robo->umbl = newPoint(robo->position.x-phw, robo->position.y-phh);
	robo->ul = newPoint(robo->position.x-ghw, robo->position.y-phh);
}


// for robo block only the Y-axis brought over
void _roboRenderCartidgeBlock(PodGun *robo, int slot_num) {
	Point bl = {
		robo->umul.x+((CARTIDGE_SLOT_WIDTH_PADDING)*(slot_num)+(CARTIDGE_SLOT_WIDTH)*(slot_num-1)),
		robo->umul.y-(CARTIDGE_SLOT_HEIGHT_PADDING)-CARTIDGE_SLOT_HEIGHT
	};

	Point br = {
		bl.x+(CARTIDGE_SLOT_WIDTH),
		bl.y
	};

	Point ur = {
		br.x,
		br.y+(CARTIDGE_SLOT_HEIGHT)
	};

	Point ul = {
		bl.x,
		ur.y
	};

	glBegin(GL_LINE_LOOP);
		glColor3f(0.50f, 0.25f, 0.25f);
		glVertex2f(bl.x, bl.y);
		glVertex2f(br.x, br.y);
		glVertex2f(ur.x, ur.y);
		glVertex2f(ul.x, ul.y);
	glEnd();
}


// robo bullet turned upside->down
void _roboRenderCartidgeBullet(PodGun *robo, int slot_num) {
	// bl has become bottom middle
	// first calculate bl as upper left point
	Point bl = {
		robo->umul.x+((CARTIDGE_SLOT_WIDTH_PADDING)*(slot_num)+(CARTIDGE_SLOT_WIDTH)*(slot_num-1)),
		robo->umul.y-(CARTIDGE_SLOT_HEIGHT_PADDING)
	};

	// br has become upper right
	// just bring in the temporary bl
	Point br = {
		bl.x+(CARTIDGE_SLOT_WIDTH),
		bl.y
	};

	// um has become upper left
	// temporarily calculate as bottom middle
	Point um = {
		bl.x+((br.x-bl.x)/2),
		bl.y-(CARTIDGE_SLOT_HEIGHT)
	};

	// swap bl and um
	Point temp = um;
	um = bl;
	bl = temp;

	glBegin(GL_LINE_LOOP);
		glColor3f(0.25f, 0.25f, 0.50f);
		glVertex2f(bl.x, bl.y);
		glVertex2f(br.x, br.y);
		glVertex2f(um.x, um.y);
	glEnd();
}

void roboRender(PodGun *robo) {
	static int count=0;
	// sprintf(PRINT_LINES[RENDER_ROBO], "(robo)# count: %10d", count++);

	robo->roboCalculatePoints(robo);

	glLineWidth(PODGUN_LINE_WIDTH);
	glBegin(GL_LINE_LOOP);
		glColor3f(ROBO_COLOR[0], ROBO_COLOR[1], ROBO_COLOR[3]);
		glPointSize(PODGUN_POINT_SIZE);
		glVertex2f(robo->bl.x, robo->bl.y);
		glVertex2f(robo->br.x, robo->br.y);
		glVertex2f(robo->ur.x, robo->ur.y);
		glVertex2f(robo->umbr.x, robo->umbr.y);
		glVertex2f(robo->umur.x, robo->umur.y);
		glVertex2f(robo->umul.x, robo->umul.y);
		glVertex2f(robo->umbl.x, robo->umbl.y);
		glVertex2f(robo->ul.x, robo->ul.y);
	glEnd();

	robo->loadCartidge(robo);
	for(int slot_num=1; slot_num<=CARTIDGE_SLOT_SIZE; slot_num++) {
		switch(robo->cartidge[slot_num-1]) {
			case CARTIDGE_BULLET:
				robo->_roboRenderCartidgeBullet(robo, slot_num);
				break;
			case CARTIDGE_BLOCK:
				robo->_roboRenderCartidgeBlock(robo, slot_num);
				break;
			default:
				printf("(robo)# cartidge slot: %d empty found!\n", slot_num);
		}
	}
}


void moveRobo(PodGun *robo, PodGun *player) {
	float move_speed = PODGUN_MOVE_SPEED;
	float pos_dif = (player->position.x-robo->position.x);
	char hdirection = pos_dif >= 0 ? 'r' : 'l';
	printf("hd: %c, pos_dif: %.2f, player: (%.2f, %.2f), robo: (%.2f, %.2f)\n", hdirection, pos_dif, player->position.x, 
		player->position.y, robo->position.x, robo->position.y);
	// calculate dynamic move_speed rate
	// using randomFloat and XLIM ratio
	/* First use XLIM to find the ratio pos_dif/XLIM
		to infer how much it is compared to XLIM.
		If the ratio is above 0.5 means the space between
		the robo and the player is larger based on the world space,
		so as to robo has to move towards the player the move_speed
		rate needs to be with much speed. To gain the much speed, let's
		the move_speed to be scaled as:
			 move_speed*(1+(pos_dif/XLIM))
			as it will ensure towards twice faster;
		and to gain a lower speed let's do the same;

	   And to gain a randomness let's assume 0.7 favorability towards
	   the current direction;
	 */
	move_speed *= (3+(pos_dif/XLIM));
//	if(randFloat(0.0, 1.0)>0.8)
//		hdirection = hdirection=='l' ? 'r' : 'l';

	switch(hdirection) {
		case 'l':
			if(robo->umul.x-move_speed >= 1.00f)
				robo->position.x -= move_speed;
			else {
				robo->position.x -= (robo->umul.x-1.00f);
				// hdirection = 'r';
			}
			break;
		case 'r':
			if(robo->umur.x+move_speed <= XLIM-1.00f)
				robo->position.x += move_speed;
			else {
				robo->position.x += (XLIM-1.0f-robo->umur.x);
				// hdirection = 'l';
			}
			break;
		default:
			printf("(robo) unknown podgun hdirection: %c\n", hdirection);
	}
}


void moveRoba(PodGun *robo) {
	float move_speed = PODGUN_MOVE_SPEED*0.50f;
	static char hdirection = 'l';

	switch(hdirection) {
		case 'l':
			if(robo->umul.x-move_speed >= 1.0f)
				robo->position.x -= move_speed;
			else {
				robo->position.x -= (robo->umul.x-1.00f);
				hdirection = 'r';
			}
			break;
		case 'r':
			if(robo->umur.x+move_speed < (XLIM-1.0f))
				robo->position.x += move_speed;
			else {
				robo->position.x += (XLIM-1.0f-robo->umur.x);
				hdirection = 'l';
			}
			break;
		default:
			printf("(robo) unknown podgun hdirection: %c\n", hdirection);
	}
}


void robaShoot(PodGun *self, PodGun *enemy) {
	// first select whether it should shoot
	if(randInt(0, 2000000)<=1900) {
		// now select whether it should shoot a block or a bullet
		// P(bullet) > P(block)
		if(randInt(0, 10)<= 7) {
			self->releaseBullet(self, enemy, 'd');
		}
		else {
			self->releaseBlock(self, enemy, 'd');
		}
	}
}


/* ============================= ROBO ============================== */

PodGun *newPodGun(Point position) {
	PodGun *new_podgun = (PodGun *)malloc(sizeof(PodGun));

	// assign attributes values
	new_podgun->position = position;
	new_podgun->cartidge[0]=CARTIDGE_EMPTY;
	new_podgun->cartidge[1]=CARTIDGE_EMPTY;
	new_podgun->cartidge[2]=CARTIDGE_EMPTY;
	new_podgun->defense = PODGUN_DEFENSE;

	// register methods
	new_podgun->render = renderPodGun;
	new_podgun->calculatePoints = calculatePodGunPoints;
	new_podgun->loadCartidge = loadCartidge;
	new_podgun->movePodGun = movePodGun;
	new_podgun->_getRandomCartidgeSlot = _getRandomCartidgeSlot;
	new_podgun->_renderCartidgeBullet = _renderCartidgeBullet;
	new_podgun->_renderCartidgeBlock = _renderCartidgeBlock;
	new_podgun->checkCartidgeAvailability = checkCartidgeAvailability;
	new_podgun->releaseBullet = releaseBullet;
  	new_podgun->releaseBlock = releaseBlock;

	// for ROBO
	new_podgun->roboRender = roboRender;
	new_podgun->roboCalculatePoints = roboCalculatePoints;
	new_podgun->_roboRenderCartidgeBullet = _roboRenderCartidgeBullet;
	new_podgun->_roboRenderCartidgeBlock = _roboRenderCartidgeBlock;
	new_podgun->moveRobo = moveRobo;
	new_podgun->moveRoba = moveRoba;
	new_podgun->robaShoot = robaShoot;

	return new_podgun;
}

/* BULLET */

void calculateBulletPoints(Bullet *self) {
	// Position is the Bullet's center

	float hw = (BULLET_WIDTH)/2;
	float hh = (BULLET_HEIGHT)/2;
	self->bl = newPoint(
		self->position.x-hw,
		self->position.y-hh
	);
	self->br = newPoint(
		self->position.x+hw,
		self->bl.y
	);
	self->um = newPoint(
		self->position.x,
		self->position.y+hh
	);
	if(self->direction=='d') {
		self->bl.x = self->position.x;
		self->br.y = self->position.y+hh;
		self->um.x = self->position.x-hw;
	}
}


int inferHitBullet(Bullet *self) {
	switch(self->direction) {
		case 'd':
			if((self->um.x >= self->receiver->bl.x && self->um.x <= self->receiver->br.x) &&
				(self->um.y >= self->receiver->bl.y && self->um.y <= self->receiver->umul.y)) {
				self->receiver->defense -= BULLET_DAMAGE_VAL;
				DeleteBullet(self);
				TOTAL_BULLETS_DESTROYED += 1;
				return 1;
			}
			break;
		case 'u':
			if((self->um.x >= self->receiver->umbl.x && self->um.x <= self->receiver->umbr.x) &&
				(self->um.y >= self->receiver->bl.y && self->um.y <= self->receiver->umul.y)) {
				self->receiver->defense -= BULLET_DAMAGE_VAL;
				DeleteBullet(self);
				TOTAL_BULLETS_DESTROYED += 1;
				return 1;
			}
			break;
		default:
			printf("(inferHit) unknown direction: %c\n", self->direction);
	}
	return 0;
}

void displayBullet(Bullet *bullet) {
	printf("\n");
	printf("Bullet: %p [(width:height) => (%.2f, %.2f)]\n", bullet, BULLET_WIDTH, BULLET_HEIGHT);
	printf("sender: %p, receiver: %p\n", bullet->sender, bullet->receiver);
	printf("position(x, y): (%.2f, %.2f)\n", bullet->position.x, bullet->position.y);
	printf("(bl.x, ur.y): (%.2f, %.2f)\n", bullet->bl.x, bullet->um.y);
	printf("\n");
}
void displayBlock(Block *block) {
	printf("\n");
	printf("Block: %p [(width:height) => (%.2f, %.2f)]\n", block, BLOCK_WIDTH, BLOCK_HEIGHT);
	printf("sender: %p, receiver: %p\n", block->sender, block->receiver);
	printf("position(x, y): (%.2f, %.2f)\n", block->position.x, block->position.y);
	printf("(bl.x, ur.y): (%.2f, %.2f)\n", block->bl.x, block->ur.y);
	printf("\n");
}

void moveBullet(Bullet *self) {
	if(self->inferHit(self))
		return;
	if((self->position.y>YLIM && self->direction=='u') || (self->position.y<0.0 && self->direction=='d')) {
		DeleteBullet(self);
		TOTAL_BULLETS_DESTROYED += 1;
		return;
	}

	self->position.y += self->move_speed*(self->direction=='d' ? -1 : 1);
}

void renderBullet(Bullet *self) {
	self->calculatePoints(self);

	glBegin(GL_LINE_LOOP);
		glColor3f(BULLET_COLOR[0], BULLET_COLOR[1], BULLET_COLOR[2]);
		glVertex2f(self->bl.x, self->bl.y);
		glVertex2f(self->br.x, self->br.y);
		glVertex2f(self->um.x, self->um.y);
	glEnd();
}

Bullet *newBullet(Point position, float move_speed, char direction, PodGun *sender, PodGun *receiver) {
	Bullet *new_bullet = (Bullet *)malloc(sizeof(Bullet));
	new_bullet->sender = sender;
	new_bullet->receiver = receiver;
	new_bullet->move_speed = move_speed;
	new_bullet->position = position;
	new_bullet->direction = direction;

	new_bullet->calculatePoints = calculateBulletPoints;
	new_bullet->inferHit = inferHitBullet;
	new_bullet->render = renderBullet;
	new_bullet->move = moveBullet;

	new_bullet->next = NULL;
	return new_bullet;
}


/* BLOCK */

void calculateBlockPoints(Block *self) {
	// Position is the Bullet's center

	float hw = (BLOCK_WIDTH*self->shape_scale)/2;
	float hh = (BLOCK_HEIGHT*self->shape_scale)/2;
	self->bl = newPoint(
		self->position.x-hw,
		self->position.y-hh
	);
	self->br = newPoint(
		self->position.x+hw,
		self->bl.y
	);
	self->ur = newPoint(
		self->br.x,
		self->position.y+hh
	);
	self->ul = newPoint(
		self->bl.x,
		self->ur.y
	);
}


int inferHitBlock(Block *self) {
	switch(self->direction) {
		case 'd':
			/*
		ul	.       . ur
				  .   .
					  .
					.	  .
		bl	.				. br
			*/
			if((self->receiver->position.x >=  self->ul.x && self->receiver->position.x <= self->br.x) &&
				 (self->receiver->position.y >= self->bl.y && self->receiver->position.y <= self->ur.y )) {
				self->receiver->defense -= BLOCK_DAMAGE_VAL;
			}
			break;
		case 'u':
			if((self->receiver->position.x >=  self->ul.x && self->receiver->position.x <= self->br.x) &&
				 (self->receiver->position.y >= self->bl.y && self->receiver->position.y <= self->ur.y )) {
				self->receiver->defense -= BLOCK_DAMAGE_VAL;
			}
			break;
		default:
			printf("(inferHit) unknown direction: (%p, %c)\n", self, self->direction);
	}
	return 0;
}

void renderBlock(Block *self) {
	self->calculatePoints(self);

	glBegin(GL_QUADS);
		glColor4f(BLOCK_COLOR[0], BLOCK_COLOR[1], BLOCK_COLOR[2], self->alpha_val);
		glVertex2f(self->bl.x, self->bl.y);
		glVertex2f(self->br.x, self->br.y);
		glVertex2f(self->ur.x, self->ur.y);
		glVertex2f(self->ul.x, self->ul.y);
	glEnd();
}

void moveBlock(Block *self) {
	self->inferHit(self);
	if(self->move_flag) {
		switch (self->direction){
			case 'u':
				if(self->ur.y+self->move_speed <= (YLIM-0.00f)){
					self->position.y += self->move_speed;
					self->shape_scale += 0.004;
				}
				else {
					self->position.y += (YLIM - self->ur.y - 0.00f);
					self->move_flag = false;
					self->mf_p = 7;
				}
				break;
			case 'd':
				if(self->br.y-self->move_speed >= 0.00f){
					self->position.y -= self->move_speed;
					self->shape_scale += 0.004;
				}
				else {
					self->position.y -= (self->br.y-0.00f);
					self->move_flag = false;
					self->mf_p = 3;
				}
				break;

			default:
				break;
		}
	}
	else self->alpha_val -= 0.00008;

	if(self->alpha_val <= 0.5) {
		DeleteBlock(self);
		TOTAL_BLOCKS_DESTROYED += 1;
	}
}

Block *newBlock(Point position, float move_speed, char direction, PodGun *sender, PodGun *receiver) {
	Block *new_block = (Block *)malloc(sizeof(Block));
	new_block->alpha_val = 1.0f;
	new_block->direction = direction;
	new_block->position = position;
	new_block->move_speed = move_speed;
	new_block->shape_scale = 1.0;
	new_block->sender = sender;
	new_block->receiver = receiver;
	new_block->move_flag = true;

	new_block->calculatePoints = calculateBlockPoints;
	new_block->inferHit = inferHitBlock;
	new_block->render = renderBlock;
	new_block->move = moveBlock;

	new_block->next = NULL;
	return new_block;
}

/* display routine */
void __PODGUN_DISP_FUNC__(void) {
	// printf("inside podgun disp..\n");
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(PODGUN_POINT_SIZE);

	Robo->roboRender(Robo);
	Player->render(Player);

	// render bullets
	for(Bullet *bullet = BULLETS; bullet!=NULL; bullet=bullet->next)
		bullet->render(bullet);

	// render blocks
	for(Block *block=BLOCKS; block!=NULL; block=block->next)
		block->render(block);

	glFlush();
}


/* animation routine */
void __PODGUN_IDLE_FUNC__(void) {
	__KEYBOARD_SPECIAL_FUNC__run();
	// move bullets
	for(Bullet *bullet = BULLETS; bullet!=NULL; bullet=bullet->next)
		bullet->move(bullet);
	// move blocks
	for(Block *block = BLOCKS; block!=NULL; block=block->next)
		block->move(block);
	Robo->moveRoba(Robo);
	Robo->robaShoot(Robo, Player);
	glutPostRedisplay();
}

void __PODGUN_TIMER_FUNC__(int val) {
	Robo->robaShoot(Robo, Player);
	// Robo->moveRoba(Robo);
	sprintf(PRINT_LINES[PRINT_ROBO_DEFENSE], "ROBO DEFENSE: %.2f%%", Robo->defense);
	sprintf(PRINT_LINES[PRINT_PODGUN_DEFENSE], "PODGUN DEFENSE: %.2f%%", Player->defense);
	sprintf(PRINT_LINES[PRINT_BULLET_FOOTPRINTS], "BULLETS: (C: %4u, D: %4u, L: %3u)", TOTAL_BULLETS_CREATED, TOTAL_BULLETS_DESTROYED, lengthBullets());
	sprintf(PRINT_LINES[PRINT_BLOCK_FOOTPRINTS], "BLOCKS: (C: %4u, D: %4u, L: %3u)", TOTAL_BLOCKS_CREATED, TOTAL_BLOCKS_DESTROYED, lengthBlocks());
	__PRINT_LINES__(4);
	/* for(Block *b=BLOCKS; b!=NULL; b=b->next) {
		if(!b->move_flag) 
			printf("#> %p : %c : %.2f : (a:%.2f) -> (mf_p:%d) -> (ms:%.2f) -> 7:%.2f, 3:%.2f\n", 
						b, b->direction, b->position.y, b->alpha_val, b->mf_p, b->move_speed, b->ur.y, b->br.y);
	}*/
	glutTimerFunc(2000, __PODGUN_TIMER_FUNC__, 0);
}

/* routine for keyboard keys: * */
void __KEYBOARD_FUNC__(unsigned char key, int x, int y) {
	switch(key) {
		case 'e':
			exit(0);
			break;
		default:
			printf("# key: %d, x: %d, y: %d\n", key, x, y);
	}
}


/* routine for event special keys: F*, up, down, left, right key etc. */
void __KEYBOARD_SPECIAL_FUNC__(int key, int x, int y) {
	KEYBOARD_STATUS[key] = true;
}

void __KEYBOARD_SPECIAL_UP_FUNC__(int key, int x, int y) {
	KEYBOARD_STATUS[key] = false;
}


void __KEYBOARD_SPECIAL_FUNC__run(void) {
	if(KEYBOARD_STATUS[GLUT_KEY_LEFT]) {
		Player->movePodGun(Player, 'l');
		// Robo->moveRobo(Robo, Player);
	}
	if(KEYBOARD_STATUS[GLUT_KEY_RIGHT]) {
		Player->movePodGun(Player, 'r');
		// Robo->moveRobo(Robo, Player);
	}
	if(KEYBOARD_STATUS[GLUT_KEY_DOWN]) {
		Player->releaseBlock(Player, Robo, 'u');
		KEYBOARD_STATUS[GLUT_KEY_DOWN] = false;
	}
	if(KEYBOARD_STATUS[GLUT_KEY_UP]) {
		Player->releaseBullet(Player, Robo, 'u');
		KEYBOARD_STATUS[GLUT_KEY_UP] = false;
	}
}

/* initialize local glut */
void initGlut(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, XLIM, 0.0f, YLIM);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}


/* initialize local routines */
void initLocal(void) {
	printf("SHAPE_SCALE: %.2f\n", SHAPE_SCALE);
	printf("POD_WIDTH: %.2f\n", POD_WIDTH);
	printf("POD_HEIGHT: %.2f\n", POD_HEIGHT);
	printf("GUN_WIDTH: %.2f\n", GUN_WIDTH);
	printf("GUN_HEIGHT: %.2f\n", GUN_HEIGHT);
	printf("CARTIDGE_SLOT_PADDING: %.2f\n", CARTIDGE_SLOT_PADDING);
	printf("CARTIDGE_SLOT_WIDTH_PADDING: %.2f\n", CARTIDGE_SLOT_WIDTH_PADDING);
	printf("CARTIDGE_SLOT_HEIGHT_PADDING: %.2f\n", CARTIDGE_SLOT_HEIGHT_PADDING);
	printf("CARTIDGE_SLOT_WIDTH: %.2f\n", CARTIDGE_SLOT_WIDTH);
	printf("CARTIDGE_SLOT_HEIGHT: %.2f\n", CARTIDGE_SLOT_HEIGHT);
	srand((unsigned int)time(NULL));
	Player = newPodGun(newPoint(XLIM/2, 1.0f));
	Robo = newPodGun(newPoint(XLIM/2, YLIM-1));
	__INIT_PRINT_LINES__();
//	pthread_t t;
//	pthread_create(&t, NULL, __THREAD_PRINT_LINES__, NULL);
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE |  GLUT_RGB);
	glutInitWindowSize(XLIM, YLIM);
	glutInitWindowPosition(300, 300);
	glutCreateWindow("Pod Gun");
	initGlut();

	initLocal();

	glutDisplayFunc(__PODGUN_DISP_FUNC__);
	glutIdleFunc(__PODGUN_IDLE_FUNC__);
	glutIgnoreKeyRepeat(0);
	glutKeyboardFunc(__KEYBOARD_FUNC__);
	glutSpecialFunc(__KEYBOARD_SPECIAL_FUNC__);
	glutSpecialUpFunc(__KEYBOARD_SPECIAL_UP_FUNC__);
	glutTimerFunc(2000, __PODGUN_TIMER_FUNC__, 0);
	glutMainLoop();

	return 0;
}
