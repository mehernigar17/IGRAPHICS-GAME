#define _CRT_SECURE_NO_WARNINGS
#include "iGraphics.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/* =============================== LEVEL 1 =============================== */
/* Cat collision box */
#define CAT_W 40
#define CAT_H 40
/* Cat draw size  */
#define CAT_DRAW_W 40
#define CAT_DRAW_H 40

/* ------music-------- */
bool musicOn = true;

/* Spawn */
const int START_X = 158;
const int START_Y = 534;

int cat1X = START_X;
int cat1Y = START_Y;

/* Screen */
int screenWidth = 1280;
int screenheight = 720;
int imgToShow;
/* Loading & Title */
int loadingProgress = 0, showLoading = 1, showTitle = 0;
int titleY = -50, titleTargetY = 360;
int bg, homepage, mainmenu, Scorepage, Helppage;
int start, start_h, score, score_h, help, help_h, Exit, Exit_h, aboutus, aboutus_h;
int overStart = 0, overScore = 0, overHelp = 0, overExit = 0, overaboutus = 0;
extern int scoreCount;
int scoreCount = 0;
/* Story */
int bgImage, nextBtn, next_h, backBtn, back_h; /* story screen back/next */
char* storyLines[] = {
	"A cunning dog has snatched away a helpless kitten from the alley.",
	"Guide the fierce mother cat through traps, sewers and maze.",
	"Outsmart the danger, rescue the kitten.",
	"Are you ready to play?"
};
char shownStory[4][300] = { "", "", "", "" };
const int totalStoryLines = 4;
int currentStoryLine = 0, charIndexInLine = 0;
bool storyFinished = false;
int storyTypingSpeed = 1;
int overNext = 0, overBack = 0;

/* Countdown */
static int countdownStep = 0, countdownDone = 0, lastUpdate = 0;
static char cdtext[100] = "Game starts in...";
int countdownBg;

/* Map & Levels */
int map, level1btn, level2btn, level3btn;
int overLevel1 = 0, overLevel2 = 0, overLevel3 = 0;
int level1;
int level2bg;   /* Level 2 background image id */

//**    lvl Transitions     **//
int transitionLevel = 1;  //1 for lvl 1, 2 for 2...
int winBg, loseBg;
int winBgL1, winBgL2;
int loseBgL1, loseBgL2;

typedef struct { int x; int y; int w; int h; } button;

//button win_playAgain = { 295, 265, 255, 60 };
button win_Back = { 50, 30, 210, 70 };
button win_Next = { 1000, 30, 210, 70 };

button lose_playAgain = { 530, 320, 215, 70 };
button lose_Back = { 50, 30, 210, 70 };
button lose_Exit = { 1000, 30, 210, 70 };


static inline int pointInButton(int mx, int my, button b){
	return (mx >= b.x && mx <= b.x + b.w && my >= b.y &&  my <= b.y + b.h);
}

void winTransition(void);
void loseTransition(void);
void openWinScreen(int level);
void openLoseScreen(int level);

/* ========= About Us Screen (new) ========= */
void aboutusScreen(void);
int aboutusPage;
int about_backbutton;
int about_back_h;
bool ishover = false;

/* ---------------music---------------- */
void playMusic(void);
void pauseMusic(void);


/* ===== SCORE SAVE/LOAD with High Score (backward compatible) ===== */
static int loadHighScoreFromFile(void){
	FILE *fp = fopen("score.txt", "r");
	if (!fp) return 0;

	int high = 0, last = 0;
	int n = fscanf(fp, "%d", &high);
	if (n == 1){
		/* Try to read a second line if present (new format) */
		if (fscanf(fp, "%d", &last) != 1){
			/* Old format: only one line existed; treat it as both high and last */
			last = high;
		}
	}
	fclose(fp);
	return (high < 0) ? 0 : high;
}

static int loadLastScoreFromFile(void){
	FILE *fp = fopen("score.txt", "r");
	if (!fp) return 0;

	int high = 0, last = 0;
	int n = fscanf(fp, "%d", &high);
	if (n == 1){
		if (fscanf(fp, "%d", &last) != 1){
			/* Old file had only one line (previous behavior) */
			last = high;
		}
	}
	fclose(fp);
	return (last < 0) ? 0 : last;
}

static void saveScoreToFile(void){
	/* Keep the highest score ever; write two lines: high, last */
	int high = loadHighScoreFromFile();
	if (scoreCount > high) high = scoreCount;

	FILE *fp = fopen("score.txt", "w");
	if (!fp) return;
	/* line 1: high score; line 2: last/current run score */
	fprintf(fp, "%d\n%d\n", high, scoreCount);
	fclose(fp);
}


/* Pipes (L1) */
typedef struct { int x, y, w, h; } pipe1;
pipe1 pipes[19];
int totalpipes1 = 19;

void initpipe(){
	pipes[0].x = 115; pipes[0].y = 69;  pipes[0].w = 15;  pipes[0].h = 424;
	pipes[1].x = 424; pipes[1].y = 289; pipes[1].w = 15;  pipes[1].h = 70;
	pipes[2].x = 891; pipes[2].y = 310; pipes[2].w = 15;  pipes[2].h = 63;
	pipes[3].x = 544; pipes[3].y = 185; pipes[3].w = 15;  pipes[3].h = 50;
	pipes[4].x = 892; pipes[4].y = 189; pipes[4].w = 15;  pipes[4].h = 50;
	pipes[5].x = 1083; pipes[5].y = 65;  pipes[5].w = 15;  pipes[5].h = 433;
	pipes[6].x = 596; pipes[6].y = 4;   pipes[6].w = 15;  pipes[6].h = 115;
	pipes[7].x = 696; pipes[7].y = 4;   pipes[7].w = 15;  pipes[7].h = 57;
	pipes[8].x = 685; pipes[8].y = 248; pipes[8].w = 15;  pipes[8].h = 50;

	pipes[9].x = 348;  pipes[9].y = 492; pipes[9].w = 932; pipes[9].h = 30;
	pipes[10].x = 270; pipes[10].y = 366; pipes[10].w = 612; pipes[10].h = 15;
	pipes[11].x = 139; pipes[11].y = 277; pipes[11].w = 287; pipes[11].h = 15;
	pipes[12].x = 913; pipes[12].y = 176; pipes[12].w = 328; pipes[12].h = 15;
	pipes[13].x = 1;   pipes[13].y = 482; pipes[13].w = 245; pipes[13].h = 30;

	pipes[14].x = 553; pipes[14].y = 227; pipes[14].w = 328; pipes[14].h = 15;
	pipes[15].x = 250; pipes[15].y = 161; pipes[15].w = 290; pipes[15].h = 30;
	pipes[16].x = 145; pipes[16].y = 49;  pipes[16].w = 445; pipes[16].h = 15;

	pipes[17].x = 0;   pipes[17].y = 0;   pipes[17].w = 0;   pipes[17].h = 0;
	pipes[18].x = 710; pipes[18].y = 43;  pipes[18].w = 369; pipes[18].h = 18;
}

bool isTouchingPipe(int nextX, int nextY, int w, int height) {
	for (int i = 0; i < totalpipes1; i++) {
		pipe1 p = pipes[i];
		bool xOverlap = ((nextX + w) > p.x) && (nextX < (p.x + p.w));
		bool yOverlap = ((nextY + height) > p.y) && (nextY < (p.y + p.h));
		if (xOverlap && yOverlap) return true;
	}
	return false;
}

/* Spikes (L1) */
typedef struct { int x, y, w, h; } Spike;
Spike spikes[64];
int totalSpikes = 0;

void initSpikes() {
	spikes[0].x = 139;  spikes[0].y = 301; spikes[0].w = 50;  spikes[0].h = 15;
	spikes[1].x = 458;  spikes[1].y = 384; spikes[1].w = 50;  spikes[1].h = 15;
	spikes[2].x = 776;  spikes[2].y = 390; spikes[2].w = 40;  spikes[2].h = 15;
	spikes[3].x = 0;    spikes[3].y = 0;   spikes[3].w = 0;   spikes[3].h = 0;
	spikes[4].x = 368;  spikes[4].y = 73;  spikes[4].w = 40;  spikes[4].h = 13;
	spikes[5].x = 819;  spikes[5].y = 75;  spikes[5].w = 50;  spikes[5].h = 15;
	spikes[6].x = 1011; spikes[6].y = 74;  spikes[6].w = 50;  spikes[6].h = 15;
	totalSpikes = 7;
}

bool isTouchingSpike(int nextX, int nextY, int w, int h) {
	for (int i = 0; i < totalSpikes; i++) {
		Spike s = spikes[i];
		bool xOverlap = ((nextX + w) > s.x) && (nextX < (s.x + s.w));
		bool yOverlap = ((nextY + h) > s.y) && (nextY < (s.y + s.h));
		if (xOverlap && yOverlap) return true;
	}
	return false;
}

/* Coins (L1) */
typedef struct { int x, y, w, h; int active; } Coin;
Coin coins[64];
int totalCoins = 0;
int coinImg = 0;


void initCoins() {
	coins[0].x = 330;  coins[0].y = 316;  coins[0].w = 32; coins[0].h = 32; coins[0].active = 1;
	coins[1].x = 383;  coins[1].y = 397;  coins[1].w = 32; coins[1].h = 32; coins[1].active = 1;
	coins[2].x = 638;  coins[2].y = 409;  coins[2].w = 32; coins[2].h = 32; coins[2].active = 1;
	coins[3].x = 1042; coins[3].y = 202;  coins[3].w = 32; coins[3].h = 32; coins[3].active = 1;
	coins[4].x = 767;  coins[4].y = 266;  coins[4].w = 32; coins[4].h = 32; coins[4].active = 1;
	coins[5].x = 497;  coins[5].y = 218;  coins[5].w = 32; coins[5].h = 32; coins[5].active = 1;
	coins[6].x = 191;  coins[6].y = 84;   coins[6].w = 32; coins[6].h = 32; coins[6].active = 1;
	coins[7].x = 507;  coins[7].y = 97;   coins[7].w = 32; coins[7].h = 32; coins[7].active = 1;
	coins[8].x = 758;  coins[8].y = 93;   coins[8].w = 32; coins[8].h = 32; coins[8].active = 1;
	coins[9].x = 949;  coins[9].y = 93;   coins[9].w = 32; coins[9].h = 32; coins[9].active = 1;
	totalCoins = 10;
}
void drawCoins() {
	for (int i = 0; i < totalCoins; i++)
	if (coins[i].active) iShowImage(coins[i].x, coins[i].y, coins[i].w, coins[i].h, coinImg);
}
void checkCoinCollection() {
	for (int i = 0; i < totalCoins; i++) {
		if (!coins[i].active) continue;
		bool xOverlap = ((cat1X + CAT_W) > coins[i].x) && (cat1X < (coins[i].x + coins[i].w));
		bool yOverlap = ((cat1Y + CAT_H) > coins[i].y) && (cat1Y < (coins[i].y + coins[i].h));
		if (xOverlap && yOverlap) {
			coins[i].active = 0; scoreCount += 1;
			saveScoreToFile();
			mciSendString("play \"coin.wav\"", NULL, 0, NULL);
		}
	}
}

/* End Point (L1 -> L2) */
typedef struct { int x, y, w, h; } EndPoint;
EndPoint endGoal1;
static inline void initEndPoint(){ endGoal1.x = 619; endGoal1.y = 30; endGoal1.w = 68; endGoal1.h = 25; }

/* Lives / Respawn */
int lives = 3;

/* Movement tuning (L1) */
const int WALK_STEP = 10;
const int JUMP_VSTEP = 8;
const int VERT_TOTAL = 200;
const int VERT_STEP = 15;
const int GRAVITY_STEP = 16;

/* Mid-air dash */
const int AIR_DASH_DIST = 240;
const int AIR_DASH_STEP = 30;

/* Movement state (L1 & reused by L2) */
int originalY;
int currentFrame2 = 0;
int direction2 = 0;       // 0=right, 1=left
bool isJumping2 = false;
int jumpDirection2 = 0;   // facing while jumping: 1 right, -1 left
int jumpMode = 0;         // 0=none, 1=vertical
int climbedUp = 0;

/* --- NEW: walk/idle latch (set on keypress, consumed by FrameChange1) --- */
bool isWalking = false;

/* dash state */
int airDashDir = 0;           // -1 left, 1 right
int airDashRemaining = 0;     // pixels left to dash
bool usedAirDashThisJump = false;

/* ======= 12 walk frames + jump poses ======= */
#define WALK_FRAMES 12
int rightWalk[WALK_FRAMES];
int leftWalk[WALK_FRAMES];
int rightJumpImg, leftJumpImg;

/* Screens (prototypes) */
void homePage();
void mainMenu();
void Startscreen();
void scorepage();
void helppage();
void aboutusScreen();       /* new */
void gameplayScreen();
void callmap();
void level1Screen();
void loadFrameslvl1();
void level2Screen();  /* calls l2_draw() */
void level3Screen();
void updateStoryTyping();
void handleStoryScreen(const char mode[]);
void mainTimer();
void l2_initCoins(void);

/* ==================== L2 Fishbone pickup & SHOOT ==================== */
typedef struct { int x, y, w, h; int active; } L2Pickup;
L2Pickup l2_fishbone;
int fishboneImg = 0;
int l2_canShoot = 0;

/* projectile ("bone") */
typedef struct { int x, y, w, h; int vx; int active; int travel; } L2Bone;
L2Bone l2_bone;
#define L2_BONE_SPEED 24
#define L2_BONE_MAX_TRAVEL 800

/* ---------- DOGS (moving targets, shared sizes) ---------- */
#define L2_TOTAL_FRAMES 6
#define L2_SPRITE_W 50
#define L2_SPRITE_H 50
#define L2_RANGE_MULT 3
int abtDog_W = 90;
int abtDog_H = 90;


/* L2 prototypes */
void l2_init(void);
void l2_draw(void);
void l2_updateRats(void);
void l2_updateAnimation(void);
void l2_handleSpecialKey(unsigned char key);
void l2_handleChar(unsigned char key);
void l2_reset(void);
static inline void l2_checkEndReached(void);
void(*currentScreen)(void) = homePage;
void(*previousScreen)(void) = NULL;

/* ================= About Us page DOG (new) ================= */
int abtDog_imagesRight[L2_TOTAL_FRAMES];
int abtDog_imagesLeft[L2_TOTAL_FRAMES];
int abtDog_currentFrame = 0;
int abtDog_baseX = 1100, abtDog_baseY = 200;
int abtDog_offsetX = 0, abtDog_dirX = 1;
int abtDog_maxOffsetX = 12;
int abtDog_speedX = 2;

void abtDog_update(void){
	abtDog_currentFrame = (abtDog_currentFrame + 1) % L2_TOTAL_FRAMES;
	abtDog_offsetX += abtDog_dirX * abtDog_speedX;
	if (abtDog_offsetX >= abtDog_maxOffsetX * L2_RANGE_MULT){
		abtDog_offsetX = abtDog_maxOffsetX * L2_RANGE_MULT;
		abtDog_dirX = -1;
	}
	if (abtDog_offsetX <= 0){
		abtDog_offsetX = 0;
		abtDog_dirX = 1;
	}
}

/* Utility / Lives */
bool isSupported(int x, int y, int w, int h) { return isTouchingPipe(x, y - 1, w, h); }
void loseLifeAndRespawn() {
	lives--;
	isJumping2 = false; jumpMode = 0; climbedUp = 0;
	airDashDir = 0; airDashRemaining = 0; usedAirDashThisJump = false;
	jumpDirection2 = 0;
	cat1X = START_X; cat1Y = START_Y; direction2 = 0;
	if (lives <= 0) { openLoseScreen(1); }
}
void checkSpikeAndHandle() { if (isTouchingSpike(cat1X, cat1Y, CAT_W, CAT_H)) loseLifeAndRespawn(); }

/* Draw routing (root) */
void iDraw() {
	iClear();
	if (showLoading) {
		iShowImage(0, 0, screenWidth, screenheight, bg);
		iSetColor(255, 173, 1); iRectangle(370, 330, 500, 30);
		iSetColor(153, 101, 21); iText(580, 300, "Loading...", GLUT_BITMAP_HELVETICA_18);
		iFilledRectangle(370, 330, loadingProgress * 5, 30);
	}
	else if (showTitle) {
		iShowImage(0, 0, screenWidth, screenheight, bg);
		iSetColor(153, 101, 21); iText(530, titleY, "The Felidae Quest", GLUT_BITMAP_HELVETICA_18);
		if (titleY >= titleTargetY) { showTitle = 0; currentScreen = homePage; }
	}
	else { currentScreen(); }
}
void checkEndReached() {
	bool xOverlap = ((cat1X + CAT_W) > endGoal1.x) && (cat1X < (endGoal1.x + endGoal1.w));
	bool yOverlap = ((cat1Y + CAT_H) > endGoal1.y) && (cat1Y < (endGoal1.y + endGoal1.h));
	if (xOverlap && yOverlap){ openWinScreen(1); }
}

/* Timer */
void mainTimer() {
	if (showLoading) { if (loadingProgress < 100) loadingProgress++; else { showLoading = 0; showTitle = 1; } }
	else if (showTitle && titleY < titleTargetY) titleY += 5;
}
void updateStoryTyping() { if (currentScreen == Startscreen) handleStoryScreen("update"); }

/* Mouse */
void iPassiveMouseMove(int mx, int my) {
	/* Main menu hovers */
	overStart = (mx >= 498 && mx <= 498 + 315 && my >= 450 && my <= 450 + 135);//135
	overScore = (mx >= 498 && mx <= 498 + 315 && my >= 350 && my <= 350 + 135);//135
	overHelp = (mx >= 498 && mx <= 498 + 315 && my >= 250 && my <= 250 + 135);
	overaboutus = (mx >= 498 && mx <= 498 + 325 && my >= 140 && my <= 140 + 120);//140+120
	overExit = (mx >= 498 && mx <= 498 + 315 && my >= 70 && my <= 135);//70+120

	overNext = (mx >= 980 && mx <= 1250 && my >= 50 && my <= 178);
	overBack = (mx >= 50 && mx <= 320 && my >= 50 && my <= 178);
	overLevel1 = (mx >= 308 && mx <= 595 && my >= 130 && my <= 180);
	overLevel2 = (mx >= 809 && mx <= 1095 && my >= 130 && my <= 186);


	if (currentScreen == aboutusScreen) {
		if (mx >= 15 && mx <= 300 && my >= 60 && my <= 130) ishover = true;
		else ishover = false;
	}
}
void iMouseMove(int mx, int my) {}

/* Clicks */
void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mciSendString("play \"click2.wav\"", NULL, 0, NULL);
		if (currentScreen == homePage) { previousScreen = currentScreen; currentScreen = mainMenu; }
		else if (currentScreen == mainMenu) {
			if (overStart) { handleStoryScreen("load"); handleStoryScreen("reset"); previousScreen = currentScreen; currentScreen = Startscreen; }
			else if (overScore) { previousScreen = currentScreen; currentScreen = scorepage; }
			else if (overHelp)  { previousScreen = currentScreen; currentScreen = helppage; }
			else if (overaboutus) { previousScreen = currentScreen; currentScreen = aboutusScreen; }
			else if (overExit)  { exit(0); }
		}
		else if (currentScreen == Startscreen) {
			if (overNext) { if (!storyFinished) while (!storyFinished) handleStoryScreen("update"); else { previousScreen = currentScreen; currentScreen = gameplayScreen; } }
			else if (overBack) { previousScreen = currentScreen; currentScreen = mainMenu; }
		}
		else if (currentScreen == callmap) {
			if (overLevel1) { previousScreen = currentScreen; currentScreen = level1Screen; }
			else if (overLevel2) { previousScreen = currentScreen; currentScreen = level2Screen; }

		}
		else if (currentScreen == aboutusScreen) {
			if (mx >= 15 && mx <= 300 && my >= 60 && my <= 130) {
				currentScreen = mainMenu;
			}
		}
		else if (currentScreen == winTransition){

			if (pointInButton(mx, my, win_Back)){
				if (transitionLevel == 2){
					lives = 3;
					l2_reset();
				}currentScreen = callmap;
			}
			else if (transitionLevel == 1 && pointInButton(mx, my, win_Next)) {
				l2_reset();
				currentScreen = level2Screen;
			}
		}
		else if (currentScreen == loseTransition) {
			if (pointInButton(mx, my, lose_playAgain)) {
				if (transitionLevel == 1) {
					lives = 3; scoreCount = 0;
					initCoins();
					cat1X = START_X; cat1Y = START_Y; direction2 = 0;
					currentScreen = level1Screen;
				}
				else if (transitionLevel == 2) {
					lives = 3;
					l2_initCoins();
					l2_reset();
					currentScreen = level2Screen;
				}
			}
			else if (pointInButton(mx, my, lose_Back)) {
				if (transitionLevel == 2){
					lives = 3;
					l2_reset();
				}  currentScreen = callmap;
			}
			//else if (transitionLevel == 1 && pointInButton(mx, my, lose_Exit)){
				//currentScreen = mainMenu;
			//}
		}
	}
}

/* Keyboard / Jump+Dash control */
void startVerticalJump() {
	isJumping2 = true;
	jumpMode = 1;
	climbedUp = 0;
	airDashDir = 0;
	airDashRemaining = 0;
	usedAirDashThisJump = false;
	originalY = cat1Y;
}

/* <-- important: route 'C' key to shooter while in Level 2 */
void iKeyboard(unsigned char key) {
	if (key == 'm' || key == 'M'){
		if (musicOn) pauseMusic();
		else playMusic();
		return;
	}

	/* Shooter key for Level 2 */
	if (currentScreen == level2Screen) {
		if (key == 'c' || key == 'C') { l2_handleChar(key); return; }
	}

	if (key == '\r') {
		if (currentScreen == homePage) { previousScreen = currentScreen; currentScreen = mainMenu; }
		else if (currentScreen == mainMenu) { handleStoryScreen("load"); handleStoryScreen("reset"); previousScreen = currentScreen; currentScreen = Startscreen; }
		else if (currentScreen == Startscreen && storyFinished) { previousScreen = currentScreen; currentScreen = gameplayScreen; }
	}
	else if (key == 27 && previousScreen) { void(*tmp)(void) = currentScreen; currentScreen = previousScreen; previousScreen = tmp; }
}

void iSpecialKeyboard(unsigned char key)
{
	/* Route Level 2 keys */
	if (currentScreen == level2Screen) { l2_handleSpecialKey(key); return; }

	/* Level 1 controls: mark walking ONLY when the move succeeds */
	if (!isJumping2){
		if (key == GLUT_KEY_RIGHT){
			int nx = cat1X + WALK_STEP;
			if (!isTouchingPipe(nx, cat1Y, CAT_W, CAT_H)) {
				cat1X = nx;
				isWalking = true;   /* <-- walk tick only on key press that actually moved */
			}
			direction2 = 0; jumpDirection2 = 1;
			checkCoinCollection(); checkSpikeAndHandle(); checkEndReached();
		}
		else if (key == GLUT_KEY_LEFT){
			int nx = cat1X - WALK_STEP;
			if (!isTouchingPipe(nx, cat1Y, CAT_W, CAT_H)) {
				cat1X = nx;
				isWalking = true;   /* <-- walk tick only on key press that actually moved */
			}
			direction2 = 1; jumpDirection2 = -1;
			checkCoinCollection(); checkSpikeAndHandle(); checkEndReached();
		}
		else if (key == GLUT_KEY_UP){
			startVerticalJump();
		}
	}
	else {
		if (jumpMode == 1 && !usedAirDashThisJump){
			if (key == GLUT_KEY_RIGHT){
				airDashDir = 1; airDashRemaining = AIR_DASH_DIST; usedAirDashThisJump = true;
				direction2 = 0; jumpDirection2 = 1;
			}
			else if (key == GLUT_KEY_LEFT){
				airDashDir = -1; airDashRemaining = AIR_DASH_DIST; usedAirDashThisJump = true;
				direction2 = 1; jumpDirection2 = -1;
			}
		}
	}
}

/* Screens */
void homePage() {
	iShowImage(0, 0, screenWidth, screenheight, homepage);
	iSetColor(255, 215, 0);
	iText(500, 35, "PRESS ENTER TO CONTINUE", GLUT_BITMAP_TIMES_ROMAN_24);
}

void mainMenu() {
	iShowImage(0, 0, screenWidth, screenheight, mainmenu);

	if (overStart)    iShowImage(498, 450, 315, 135, start_h);    else iShowImage(498, 450, 315, 135, start);
	if (overScore)    iShowImage(498, 350, 315, 135, score_h);    else iShowImage(498, 350, 315, 135, score);
	if (overHelp)     iShowImage(498, 250, 315, 135, help_h);     else iShowImage(498, 250, 315, 135, help);
	if (overaboutus)  iShowImage(498, 140, 325, 180, aboutus_h);  else iShowImage(498, 140, 325, 180, aboutus);
	if (overExit)     iShowImage(498, 70, 315, 135, Exit_h);      else iShowImage(498, 70, 315, 135, Exit);
}

void Startscreen() { handleStoryScreen("draw"); }


void drawGameWithCountdown() {
	static int switched = 0;
	iClear(); iShowImage(0, 0, screenWidth, screenheight, countdownBg);
	if (!countdownDone) {
		int currentTime = glutGet(GLUT_ELAPSED_TIME);
		if (currentTime - lastUpdate >= 1000) {
			lastUpdate = currentTime; countdownStep++;
			if (countdownStep == 1) strcat(cdtext, " 3...");
			else if (countdownStep == 2) strcat(cdtext, " 2...");
			else if (countdownStep == 3) strcat(cdtext, " 1...");
			else if (countdownStep == 4) countdownDone = 1;
		}
		iSetColor(255, 255, 0); iText(480, 360, cdtext, GLUT_BITMAP_HELVETICA_18);
	}
	else if (!switched) { currentScreen = callmap; switched = 1; }
}
void gameplayScreen() { drawGameWithCountdown(); }
void callmap() {
	iShowImage(0, 0, screenWidth, screenheight, map);

}

/* ======= Load 12 walk frames + jump poses ======= */
void loadFrameslvl1() {
	char path[64];

	for (int i = 0; i < WALK_FRAMES; i++) {
		sprintf(path, "cat_right_%d.png", i);
		rightWalk[i] = iLoadImage(path);
	}
	for (int i = 0; i < WALK_FRAMES; i++) {
		sprintf(path, "cat_left_%d.png", i);
		leftWalk[i] = iLoadImage(path);
	}

	rightJumpImg = iLoadImage("cat_jump_right.png");
	leftJumpImg = iLoadImage("cat_jump_left.png");
}

void drawHUD() {
	iSetColor(255, 0, 0);
	char lifeText[64]; sprintf(lifeText, "Lives: %d", lives);
	iText(20, screenheight - 30, lifeText, GLUT_BITMAP_HELVETICA_18);

	iSetColor(255, 255, 0);
	char scoreText[64]; sprintf(scoreText, "Score: %d", scoreCount);
	iText(140, screenheight - 30, scoreText, GLUT_BITMAP_HELVETICA_18);
}
void level1Screen() {
	iClear(); iShowImage(0, 0, screenWidth, screenheight, level1);
	drawCoins(); drawHUD();

	/* choose sprite for this frame */
	if (isJumping2) {
		int faceRight = (jumpDirection2 == 1) || (direction2 == 0);
		imgToShow = faceRight ? rightJumpImg : leftJumpImg;
	}
	else {
		/* idle = frame 0 unless a keypress just moved us */
		int frame = isWalking ? currentFrame2 : 0;
		if (direction2 == 1) imgToShow = leftWalk[frame];
		else                 imgToShow = rightWalk[frame];
	}

	iShowImage(cat1X, cat1Y, CAT_DRAW_W, CAT_DRAW_H, imgToShow);
}

/* Physics / Animation (L1) */
void FrameChange1(){
	/* advance walk animation only if a move key was pressed since last tick */
	bool advance = isWalking;
	isWalking = false;  /* consume latch so idle resumes immediately when no key pressed */

	if (!isJumping2 && advance)
		currentFrame2 = (currentFrame2 + 1) % WALK_FRAMES;

	/* vertical jump ascent */
	if (isJumping2 && jumpMode == 1){
		int step = VERT_STEP;
		if (climbedUp + step > VERT_TOTAL) step = VERT_TOTAL - climbedUp;

		int movedUp = 0;
		for (int d = 1; d <= step; d++){ if (!isTouchingPipe(cat1X, cat1Y + d, CAT_W, CAT_H)) movedUp = d; else break; }
		if (movedUp > 0) { cat1Y += movedUp; climbedUp += movedUp; }

		/* apply mid-air dash if active */
		if (airDashRemaining > 0){
			int dir = airDashDir;
			int stepDash = AIR_DASH_STEP;
			if (stepDash > airDashRemaining) stepDash = airDashRemaining;

			int movedH = 0;
			for (int d = 1; d <= stepDash; d++){
				int nx = cat1X + dir;
				if (!isTouchingPipe(nx, cat1Y, CAT_W, CAT_H)){ cat1X = nx; movedH++; }
				else { airDashRemaining = 0; break; }
			}
			airDashRemaining -= movedH;
			if (movedH > 0){ direction2 = (dir > 0) ? 0 : 1; jumpDirection2 = (dir > 0) ? 1 : -1; }
		}

		if (climbedUp >= VERT_TOTAL || movedUp == 0){
			isJumping2 = false; jumpMode = 0; climbedUp = 0;
			airDashDir = 0; airDashRemaining = 0; usedAirDashThisJump = false;
		}
	}

	/* gravity */
	if (!isJumping2){
		if (!isSupported(cat1X, cat1Y, CAT_W, CAT_H)){
			int fall = GRAVITY_STEP;
			for (int d = 1; d <= GRAVITY_STEP; d++){
				int tryY = cat1Y - d;
				if (isTouchingPipe(cat1X, tryY, CAT_W, CAT_H)){ fall = d - 1; break; }
			}
			if (fall > 0) cat1Y -= fall;
		}
	}

	/* interactions */
	checkCoinCollection();
	checkSpikeAndHandle();
	checkEndReached();
}

/* Story handler */
void handleStoryScreen(const char mode[]) {
	if (strcmp(mode, "load") == 0) {
		bgImage = iLoadImage("newstory1.png");
		nextBtn = iLoadImage("next.png"); next_h = iLoadImage("next_h.png");
		backBtn = iLoadImage("back.png"); back_h = iLoadImage("back_h.png");
		countdownBg = iLoadImage("countdownpage.png");
		bg = iLoadImage("bufferpage.png");
	}
	else if (strcmp(mode, "reset") == 0) {
		currentStoryLine = charIndexInLine = 0; storyFinished = false;
		for (int i = 0; i < totalStoryLines; i++) shownStory[i][0] = '\0';
		countdownStep = countdownDone = lastUpdate = 0;
		strcpy(cdtext, "Game starts in...");
	}
	else if (strcmp(mode, "update") == 0) {
		if (storyFinished) return;
		for (int i = 0; i < storyTypingSpeed; i++) {
			char c = storyLines[currentStoryLine][charIndexInLine];
			if (c) { shownStory[currentStoryLine][charIndexInLine++] = c; shownStory[currentStoryLine][charIndexInLine] = '\0'; }
			else { currentStoryLine++; charIndexInLine = 0; if (currentStoryLine >= totalStoryLines) storyFinished = true; break; }
		}
	}
	else if (strcmp(mode, "draw") == 0) {
		iShowImage(0, 0, screenWidth, screenheight, bgImage);
		if (overBack) iShowImage(50, 50, 270, 128, back_h); else iShowImage(50, 50, 270, 128, backBtn);
		if (overNext) iShowImage(980, 50, 270, 128, next_h); else iShowImage(980, 50, 270, 128, nextBtn);
		iSetColor(255, 255, 0);
		int baseY = 160, gap = 35;
		for (int i = 0; i <= currentStoryLine && i < totalStoryLines; i++) {
			int w = (int)strlen(shownStory[i]) * 10;
			int x = (screenWidth - w) / 2;
			int y = baseY + (3 - i) * gap + 60;
			iText(x, y, shownStory[i], GLUT_BITMAP_HELVETICA_18);
		}
	}
}

/* =============================== LEVEL 2 =============================== */
/* level 2 cat + coins */
int l2_catImg = 0;
int l2_catW = 40, l2_catH = 40;
int l2_catX = 560, l2_catY = 693;

/* ---------- L2 geometry: pipes & spikes ---------- */
typedef struct{ int x; int y; int width; int height; } pipe2;
pipe2 pipes2[32];
int totalpipes_lvl2 = 32;
//level 2 endpoint 
/* End Point (L2) */
EndPoint endGoal2;
static inline void initEndPoint2(){

	endGoal2.x = 1251;
	endGoal2.y = 354;
	endGoal2.w = 60;
	endGoal2.h = 40;
}

void initpipe_lvl2(){
	/* ... unchanged pipe data ... */
	pipes2[0].x = 3; pipes2[0].y = 37; pipes2[0].width = 14; pipes2[0].height = 586;
	pipes2[1].x = 489; pipes2[1].y = 536; pipes2[1].width = 14; pipes2[1].height = 184;
	pipes2[2].x = 626; pipes2[2].y = 681; pipes2[2].width = 14; pipes2[2].height = 39;
	pipes2[3].x = 1257; pipes2[3].y = 477; pipes2[3].width = 14; pipes2[3].height = 141;
	pipes2[4].x = 1257; pipes2[4].y = 29; pipes2[4].width = 14; pipes2[4].height = 294;
	pipes2[5].x = 971; pipes2[5].y = 32; pipes2[5].width = 14; pipes2[5].height = 148;
	pipes2[6].x = 572; pipes2[6].y = 186; pipes2[6].width = 14; pipes2[6].height = 241;
	pipes2[7].x = 131; pipes2[7].y = 325; pipes2[7].width = 14; pipes2[7].height = 99;
	pipes2[8].x = 1005; pipes2[8].y = 632; pipes2[8].width = 14; pipes2[8].height = 44;
	pipes2[9].x = 966; pipes2[9].y = 479; pipes2[9].width = 14; pipes2[9].height = 49;
	pipes2[10].x = 755; pipes2[10].y = 372; pipes2[10].width = 14; pipes2[10].height = 51;
	pipes2[11].x = 1165; pipes2[11].y = 275; pipes2[11].width = 14; pipes2[11].height = 60;
	pipes2[12].x = 1084; pipes2[12].y = 378; pipes2[12].width = 14; pipes2[12].height = 94;
	pipes2[13].x = 314; pipes2[13].y = 431; pipes2[13].width = 14; pipes2[13].height = 66;
	pipes2[14].x = 347; pipes2[14].y = 293; pipes2[14].width = 14; pipes2[14].height = 26;
	pipes2[15].x = 423; pipes2[15].y = 294; pipes2[15].width = 14; pipes2[15].height = 27;
	pipes2[16].x = 8;   pipes2[16].y = 625; pipes2[16].width = 473; pipes2[16].height = 14;
	pipes2[17].x = 511; pipes2[17].y = 525; pipes2[17].width = 453; pipes2[17].height = 15;
	pipes2[18].x = 591; pipes2[18].y = 358; pipes2[18].width = 492; pipes2[18].height = 15;
	pipes2[19].x = 635; pipes2[19].y = 668; pipes2[19].width = 361; pipes2[19].height = 15;
	pipes2[20].x = 1022; pipes2[20].y = 621; pipes2[20].width = 239; pipes2[20].height = 15;
	pipes2[21].x = 23;  pipes2[21].y = 202; pipes2[21].width = 453; pipes2[21].height = 15;
	pipes2[22].x = 21;  pipes2[22].y = 22;  pipes2[22].width = 1238; pipes2[22].height = 15;
	pipes2[23].x = 580; pipes2[23].y = 172; pipes2[23].width = 287; pipes2[23].height = 15;
	pipes2[24].x = 967; pipes2[24].y = 180; pipes2[24].width = 123;  pipes2[24].height = 15;
	pipes2[25].x = 1099; pipes2[25].y = 265; pipes2[25].width = 64;  pipes2[25].height = 15;
	pipes2[26].x = 147; pipes2[26].y = 313; pipes2[26].width = 190;  pipes2[26].height = 15;
	pipes2[27].x = 151; pipes2[27].y = 414; pipes2[27].width = 419;  pipes2[27].height = 15;
	pipes2[28].x = 25;  pipes2[28].y = 504; pipes2[28].width = 181;  pipes2[28].height = 15;
	pipes2[29].x = 444; pipes2[29].y = 310; pipes2[29].width = 125;  pipes2[29].height = 15;
	pipes2[30].x = 1176; pipes2[30].y = 320; pipes2[30].width = 97;  pipes2[30].height = 15;
	pipes2[31].x = 1099; pipes2[31].y = 464; pipes2[31].width = 164; pipes2[31].height = 15;
}

bool isTouchingPipe_lvl2(int nextX, int nextY, int width, int height){
	for (int i = 0; i < totalpipes_lvl2; i++){
		pipe2 p = pipes2[i];
		bool xOverlap = (nextX + width > p.x) && (nextX < p.x + p.width);
		bool yOverlap = (nextY + height > p.y) && (nextY < p.y + p.height);
		if (xOverlap && yOverlap) return true;
	}
	return false;
}

/* spikes L2 */
typedef struct { int x, y, w, h; } Spike_lvl2;
Spike_lvl2 spikes2[64];
int totalspikes_lvl2 = 9;

void initSpikes_lvl2() {
	spikes2[0].x = 729;  spikes2[0].y = 541; spikes2[0].w = 35;  spikes2[0].h = 10;
	spikes2[1].x = 864;  spikes2[1].y = 534; spikes2[1].w = 35;  spikes2[1].h = 20;
	spikes2[2].x = 862;  spikes2[2].y = 372; spikes2[2].w = 35;  spikes2[2].h = 18;
	spikes2[3].x = 524;  spikes2[3].y = 329; spikes2[3].w = 42;  spikes2[3].h = 12;
	spikes2[4].x = 187;  spikes2[4].y = 329; spikes2[4].w = 35;  spikes2[4].h = 11;
	spikes2[5].x = 191;  spikes2[5].y = 214; spikes2[5].w = 35;  spikes2[5].h = 13;
	spikes2[6].x = 330;  spikes2[6].y = 37;  spikes2[6].w = 35;  spikes2[6].h = 15;
	spikes2[7].x = 592;  spikes2[7].y = 48;  spikes2[7].w = 35;  spikes2[7].h = 12;
	spikes2[8].x = 1208; spikes2[8].y = 34;  spikes2[8].w = 35;  spikes2[8].h = 12;
}

bool isTouchingSpike_lvl2(int nextX, int nextY, int w, int h) {
	for (int i = 0; i < totalspikes_lvl2; i++) {
		Spike_lvl2 s = spikes2[i];
		bool xOverlap = ((nextX + w) > s.x) && (nextX < (s.x + s.w));
		bool yOverlap = ((nextY + h) > s.y) && (nextY < (s.y + s.h));
		if (xOverlap && yOverlap) return true;
	}
	return false;
}

/* coins L2 */
typedef struct { int x, y, w, h; int active; } L2Coin;
L2Coin l2_coins[16]; int l2_totalCoins = 0;

static void l2_initCoins(void){
	int i = 0;
	l2_coins[i].x = 549; l2_coins[i].y = 654; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 814; l2_coins[i].y = 58; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 823; l2_coins[i].y = 381; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 617; l2_coins[i].y = 381; l2_coins[i].w = 40; l2_coins[i].h = 50; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 36;  l2_coins[i].y = 540; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 50;  l2_coins[i].y = 331; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 273; l2_coins[i].y = 211; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 506; l2_coins[i].y = 340; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 58;  l2_coins[i].y = 94;  l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 383; l2_coins[i].y = 372; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 631; l2_coins[i].y = 300; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 1033; l2_coins[i].y = 116; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 1209; l2_coins[i].y = 412; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 753; l2_coins[i].y = 142; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 1122; l2_coins[i].y = 521; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;
	l2_coins[i].x = 667; l2_coins[i].y = 552; l2_coins[i].w = 40; l2_coins[i].h = 40; l2_coins[i].active = 1; i++;

	l2_totalCoins = i;
}
static void l2_checkCoinCollection(void){
	for (int i = 0; i < l2_totalCoins; i++){
		if (!l2_coins[i].active) continue;

		bool xo = ((l2_catX + l2_catW) > l2_coins[i].x) && (l2_catX < (l2_coins[i].x + l2_coins[i].w));
		bool yo = ((l2_catY + l2_catH) > l2_coins[i].y) && (l2_catY < (l2_coins[i].y + l2_coins[i].h));
		if (xo && yo){
			l2_coins[i].active = 0;

			/* add to the SAME global score */
			scoreCount += 1;
			saveScoreToFile();  /* persist running total */

			/* optional: keep your pickup SFX consistent with L1 */
			//PlaySound("coin.wav", NULL, SND_ASYNC);
			//PlaySound("bgmfinal.wav", NULL, SND_LOOP | SND_ASYNC);
			mciSendString("play \"coin.wav\"", NULL, 0, NULL);
		}
	}
}


/* ---------- L2 helpers ---------- */
static inline bool l2_isSupported(int x, int y, int w, int h) { return isTouchingPipe_lvl2(x, y - 1, w, h); }
static inline void l2_loseLifeAndRespawn(void) {
	//lives--;
	//isJumping2 = false; jumpMode = 0; climbedUp = 0;
	//airDashDir = 0; airDashRemaining = 0; usedAirDashThisJump = false;
	//jumpDirection2 = 0;
	//l2_catX = 560; l2_catY = 693; direction2 = 0;
	//if (lives <= 0) { lives = 3; scoreCount = 0; l2_initCoins(); currentScreen = level2Screen; }
	/* ---------- UPDATE THIS FUNCTION ---------- */


	lives--;


	isJumping2 = false;
	jumpMode = 0;
	climbedUp = 0;
	airDashDir = 0;
	airDashRemaining = 0;
	usedAirDashThisJump = false;
	jumpDirection2 = 0;

	if (lives <= 0) {
		// Level 2
		openLoseScreen(2);
		return;
	}


	l2_catX = 560;
	l2_catY = 693;
	direction2 = 0;

}




static inline void l2_checkSpikeAndHandle(void) { if (isTouchingSpike_lvl2(l2_catX, l2_catY, CAT_W, CAT_H)) l2_loseLifeAndRespawn(); }

/* ---------- DOGS (moving targets) ---------- */
int l2_imageRight[L2_TOTAL_FRAMES];
int l2_imageLeft[L2_TOTAL_FRAMES];
int l2_currentFrame = 0;

/* dogs base positions & motion (6 dogs) */
int l2_basePosX1 = 238, l2_basePosY1 = 99;  int l2_offsetX1 = 0; int l2_dirX1 = 1; int l2_maxOffsetX1 = 20; int l2_speedX1 = 2; int l2_alive1 = 1;
int l2_basePosX2 = 1180, l2_basePosY2 = 92;  int l2_offsetX2 = 0; int l2_dirX2 = 1; int l2_maxOffsetX2 = 16; int l2_speedX2 = 2; int l2_alive2 = 1;
int l2_basePosX3 = 755, l2_basePosY3 = 195; int l2_offsetX3 = 0; int l2_dirX3 = 1; int l2_maxOffsetX3 = 24; int l2_speedX3 = 2; int l2_alive3 = 1;
int l2_basePosX4 = 400, l2_basePosY4 = 423; int l2_offsetX4 = 0; int l2_dirX4 = 1; int l2_maxOffsetX4 = 14; int l2_speedX4 = 2; int l2_alive4 = 1;
int l2_basePosX6 = 1192, l2_basePosY6 = 497; int l2_offsetX6 = 0; int l2_dirX6 = 1; int l2_maxOffsetX6 = 12; int l2_speedX6 = 2; int l2_alive6 = 1;
//int l2_basePosX7 = 175, l2_basePosY7 = 524; int l2_offsetX7 = 0; int l2_dirX7 = 1; int l2_maxOffsetX7 = 15; int l2_speedX7 = 2; int l2_alive7 = 1;

/* ---------- drawing helpers ---------- */
static void l2_drawOne(int baseX, int baseY, int offsetX, int dirX){
	int img = (dirX >= 0) ? l2_imageLeft[l2_currentFrame] : l2_imageRight[l2_currentFrame];
	iShowImage(baseX + offsetX, baseY, L2_SPRITE_W, L2_SPRITE_H, img);
}

/* ---------- update dogs offset/frames ---------- */
void l2_updateRats(void){
	if (currentScreen != level2Screen) return;

	l2_currentFrame = (l2_currentFrame + 1) % L2_TOTAL_FRAMES;

	if (l2_alive1){
		l2_offsetX1 += l2_dirX1 * l2_speedX1;
		if (l2_offsetX1 >= l2_maxOffsetX1 * L2_RANGE_MULT){ l2_offsetX1 = l2_maxOffsetX1 * L2_RANGE_MULT; l2_dirX1 = -1; }
		if (l2_offsetX1 <= 0){ l2_offsetX1 = 0; l2_dirX1 = 1; }
	}
	if (l2_alive2){
		l2_offsetX2 += l2_dirX2 * l2_speedX2;
		if (l2_offsetX2 >= l2_maxOffsetX2 * L2_RANGE_MULT){ l2_offsetX2 = l2_maxOffsetX2 * L2_RANGE_MULT; l2_dirX2 = -1; }
		if (l2_offsetX2 <= 0){ l2_offsetX2 = 0; l2_dirX2 = 1; }
	}
	if (l2_alive3){
		l2_offsetX3 += l2_dirX3 * l2_speedX3;
		if (l2_offsetX3 >= l2_maxOffsetX3 * L2_RANGE_MULT){ l2_offsetX3 = l2_maxOffsetX3 * L2_RANGE_MULT; l2_dirX3 = -1; }
		if (l2_offsetX3 <= 0){ l2_offsetX3 = 0; l2_dirX3 = 1; }
	}
	if (l2_alive4){
		l2_offsetX4 += l2_dirX4 * l2_speedX4;
		if (l2_offsetX4 >= l2_maxOffsetX4 * L2_RANGE_MULT){ l2_offsetX4 = l2_maxOffsetX4 * L2_RANGE_MULT; l2_dirX4 = -1; }
		if (l2_offsetX4 <= 0){ l2_offsetX4 = 0; l2_dirX4 = 1; }
	}
	if (l2_alive6){
		l2_offsetX6 += l2_dirX6 * l2_speedX6;
		if (l2_offsetX6 >= l2_maxOffsetX6 * L2_RANGE_MULT){ l2_offsetX6 = l2_maxOffsetX6 * L2_RANGE_MULT; l2_dirX6 = -1; }
		if (l2_offsetX6 <= 0){ l2_offsetX6 = 0; l2_dirX6 = 1; }
	}
	//if (l2_alive7){
	//l2_offsetX7 += l2_dirX7 * l2_speedX7;
	//if (l2_offsetX7 >= l2_maxOffsetX7 * L2_RANGE_MULT){ l2_offsetX7 = l2_maxOffsetX7 * L2_RANGE_MULT; l2_dirX7 = -1; }
	//if (l2_offsetX7 <= 0){ l2_offsetX7 = 0; l2_dirX7 = 1; }
	//}
}

/* ---------- L2 hint text ---------- */
static inline void l2_drawHint(void) {
	if (!l2_canShoot && l2_fishbone.active) {
		iSetColor(255, 255, 0);
		iText(750, 633, "COLLECT THE FISHBONE AND PRESS C TO SHOOT", GLUT_BITMAP_TIMES_ROMAN_10);
	}
	else if (!l2_canShoot && !l2_fishbone.active) {
		iSetColor(255, 255, 0);
		iText(750, 633, "PICKED! NOW PRESS C TO SHOOT", GLUT_BITMAP_TIMES_ROMAN_10);
	}
}

/* ---------- projectile spawn on 'C' ---------- */
void l2_handleChar(unsigned char key){
	if (key == 'c' || key == 'C'){
		if (!l2_canShoot) return;
		if (l2_bone.active)  return;

		int faceRight = (jumpDirection2 == 1) || (direction2 == 0);

		l2_bone.active = 1;
		l2_bone.travel = 0;
		l2_bone.w = 32; l2_bone.h = 16;

		if (faceRight){
			l2_bone.x = l2_catX + CAT_DRAW_W - 6;
			l2_bone.vx = +L2_BONE_SPEED;
		}
		else {
			l2_bone.x = l2_catX - l2_bone.w + 6;
			l2_bone.vx = -L2_BONE_SPEED;
		}
		l2_bone.y = l2_catY + (CAT_DRAW_H / 2) - (l2_bone.h / 2);
	}
}

/* ---------- L2 player physics, pickup, projectile & collisions ---------- */
static inline int rectOverlap(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh){
	return ((ax + aw)>bx) && (ax<(bx + bw)) && ((ay + ah)>by) && (ay<(by + bh));
}

void l2_updateAnimation(void){

	if (currentScreen != level2Screen) return;
	/* physics and interactions only; walking animation is controlled by FrameChange1 latch */

	if (isJumping2 && jumpMode == 1){
		int step = VERT_STEP;
		if (climbedUp + step > VERT_TOTAL) step = VERT_TOTAL - climbedUp;

		int movedUp = 0;
		for (int d = 1; d <= step; d++){
			if (!isTouchingPipe_lvl2(l2_catX, l2_catY + d, CAT_W, CAT_H)) movedUp = d;
			else break;
		}
		if (movedUp > 0){ l2_catY += movedUp; climbedUp += movedUp; }

		if (airDashRemaining > 0){
			int dir = airDashDir;
			int stepDash = AIR_DASH_STEP;
			if (stepDash > airDashRemaining) stepDash = airDashRemaining;
			int movedH = 0;
			for (int d = 1; d <= stepDash; d++){
				int nx = l2_catX + dir;
				if (!isTouchingPipe_lvl2(nx, l2_catY, CAT_W, CAT_H)){ l2_catX = nx; movedH++; }
				else { airDashRemaining = 0; break; }
			}
			airDashRemaining -= movedH;
			if (movedH > 0){ direction2 = (dir > 0) ? 0 : 1; jumpDirection2 = (dir > 0) ? 1 : -1; }
		}

		if (climbedUp >= VERT_TOTAL || movedUp == 0){
			isJumping2 = false; jumpMode = 0; climbedUp = 0;
			airDashDir = 0; airDashRemaining = 0; usedAirDashThisJump = false;
		}
	}

	if (!isJumping2){
		if (!l2_isSupported(l2_catX, l2_catY, CAT_W, CAT_H)){
			int fall = GRAVITY_STEP;
			for (int d = 1; d <= GRAVITY_STEP; d++){
				int tryY = l2_catY - d;
				if (isTouchingPipe_lvl2(l2_catX, tryY, CAT_W, CAT_H)){ fall = d - 1; break; }
			}
			if (fall > 0) l2_catY -= fall;
		}
	}

	if (l2_fishbone.active){
		bool xo = ((l2_catX + l2_catW) > l2_fishbone.x) && (l2_catX < (l2_fishbone.x + l2_fishbone.w));
		bool yo = ((l2_catY + l2_catH) > l2_fishbone.y) && (l2_catY < (l2_fishbone.y + l2_fishbone.h));
		if (xo && yo){
			l2_fishbone.active = 0;
			l2_canShoot = 1;
		}
	}

	if (l2_bone.active){
		int nextX = l2_bone.x + l2_bone.vx;

		if (isTouchingPipe_lvl2(nextX, l2_bone.y, l2_bone.w, l2_bone.h)){
			l2_bone.active = 0;
		}
		else {
			l2_bone.x = nextX;
			l2_bone.travel += (l2_bone.vx>0 ? l2_bone.vx : -l2_bone.vx);
			if (l2_bone.travel >= L2_BONE_MAX_TRAVEL ||
				l2_bone.x < -100 || l2_bone.x > screenWidth + 100){
				l2_bone.active = 0;
			}
		}

		if (l2_bone.active){
			if (l2_alive1 && rectOverlap(l2_bone.x, l2_bone.y, l2_bone.w, l2_bone.h,
				l2_basePosX1 + l2_offsetX1, l2_basePosY1, L2_SPRITE_W, L2_SPRITE_H)) {
				l2_alive1 = 0; l2_bone.active = 0;
				mciSendString("play \"bonk.wav\"", NULL, 0, NULL);
			}
			else if (l2_alive2 && rectOverlap(l2_bone.x, l2_bone.y, l2_bone.w, l2_bone.h,
				l2_basePosX2 + l2_offsetX2, l2_basePosY2, L2_SPRITE_W, L2_SPRITE_H)) {
				l2_alive2 = 0; l2_bone.active = 0;
				mciSendString("play \"bonk.wav\"", NULL, 0, NULL);
			}
			else if (l2_alive3 && rectOverlap(l2_bone.x, l2_bone.y, l2_bone.w, l2_bone.h,
				l2_basePosX3 + l2_offsetX3, l2_basePosY3, L2_SPRITE_W, L2_SPRITE_H)) {
				l2_alive3 = 0; l2_bone.active = 0;
				mciSendString("play \"bonk.wav\"", NULL, 0, NULL);
			}
			else if (l2_alive4 && rectOverlap(l2_bone.x, l2_bone.y, l2_bone.w, l2_bone.h,
				l2_basePosX4 + l2_offsetX4, l2_basePosY4, L2_SPRITE_W, L2_SPRITE_H)) {
				l2_alive4 = 0; l2_bone.active = 0;
				mciSendString("play \"bonk.wav\"", NULL, 0, NULL);
			}
			else if (l2_alive6 && rectOverlap(l2_bone.x, l2_bone.y, l2_bone.w, l2_bone.h,
				l2_basePosX6 + l2_offsetX6, l2_basePosY6, L2_SPRITE_W, L2_SPRITE_H)) {
				l2_alive6 = 0; l2_bone.active = 0;
				mciSendString("play \"bonk.wav\"", NULL, 0, NULL);
			}
			//else if (l2_alive7 && rectOverlap(l2_bone.x, l2_bone.y, l2_bone.w, l2_bone.h,
			//	l2_basePosX7 + l2_offsetX7, l2_basePosY7, L2_SPRITE_W, L2_SPRITE_H)) {
			//l2_alive7 = 0; l2_bone.active = 0;
			//mciSendString("play \"bonk.wav\"", NULL, 0, NULL);
			//}
		}
	}

	l2_checkCoinCollection();
	l2_checkSpikeAndHandle();

	/* === NEW: cat vs dog collision (L2) === */
	if (l2_alive1 && rectOverlap(l2_catX, l2_catY, CAT_W, CAT_H,
		l2_basePosX1 + l2_offsetX1, l2_basePosY1, L2_SPRITE_W, L2_SPRITE_H)) {
		l2_loseLifeAndRespawn(); return;
	}
	if (l2_alive2 && rectOverlap(l2_catX, l2_catY, CAT_W, CAT_H,
		l2_basePosX2 + l2_offsetX2, l2_basePosY2, L2_SPRITE_W, L2_SPRITE_H)) {
		l2_loseLifeAndRespawn(); return;
	}
	if (l2_alive3 && rectOverlap(l2_catX, l2_catY, CAT_W, CAT_H,
		l2_basePosX3 + l2_offsetX3, l2_basePosY3, L2_SPRITE_W, L2_SPRITE_H)) {
		l2_loseLifeAndRespawn(); return;
	}
	if (l2_alive4 && rectOverlap(l2_catX, l2_catY, CAT_W, CAT_H,
		l2_basePosX4 + l2_offsetX4, l2_basePosY4, L2_SPRITE_W, L2_SPRITE_H)) {
		l2_loseLifeAndRespawn(); return;
	}
	if (l2_alive6 && rectOverlap(l2_catX, l2_catY, CAT_W, CAT_H,
		l2_basePosX6 + l2_offsetX6, l2_basePosY6, L2_SPRITE_W, L2_SPRITE_H)) {
		l2_loseLifeAndRespawn(); return;
	}
	//if (l2_alive7 && rectOverlap(l2_catX, l2_catY, CAT_W, CAT_H,
	//	l2_basePosX7 + l2_offsetX7, l2_basePosY7, L2_SPRITE_W, L2_SPRITE_H)) {
	//l2_loseLifeAndRespawn(); return;
	//}

	l2_checkEndReached();

}

void l2_handleSpecialKey(unsigned char key){
	/* Level 2 controls: mark walking ONLY when the move succeeds */
	if (!isJumping2){
		if (key == GLUT_KEY_RIGHT){
			int nx = l2_catX + WALK_STEP;
			if (!isTouchingPipe_lvl2(nx, l2_catY, CAT_W, CAT_H)) {
				l2_catX = nx;
				isWalking = true;   /* <-- walk tick only on key press that actually moved */
			}
			direction2 = 0; jumpDirection2 = 1;
			l2_checkCoinCollection(); l2_checkSpikeAndHandle();
			l2_checkEndReached();
		}
		else if (key == GLUT_KEY_LEFT){
			int nx = l2_catX - WALK_STEP;
			if (!isTouchingPipe_lvl2(nx, l2_catY, CAT_W, CAT_H)) {
				l2_catX = nx;
				isWalking = true;   /* <-- walk tick only on key press that actually moved */
			}
			direction2 = 1; jumpDirection2 = -1;
			l2_checkCoinCollection(); l2_checkSpikeAndHandle();
			l2_checkEndReached();
		}
		else if (key == GLUT_KEY_UP){
			startVerticalJump();
		}
	}
	else {
		if (jumpMode == 1 && !usedAirDashThisJump){
			if (key == GLUT_KEY_RIGHT){
				airDashDir = 1;  airDashRemaining = AIR_DASH_DIST; usedAirDashThisJump = true;
				direction2 = 0;  jumpDirection2 = 1;
			}
			else if (key == GLUT_KEY_LEFT){
				airDashDir = -1; airDashRemaining = AIR_DASH_DIST; usedAirDashThisJump = true;
				direction2 = 1;  jumpDirection2 = -1;
			}
		}
	}
}

void l2_draw(void){
	iClear();
	iShowImage(0, 0, 1280, 720, level2bg);

	if (l2_alive1) l2_drawOne(l2_basePosX1, l2_basePosY1, l2_offsetX1, l2_dirX1);
	if (l2_alive2) l2_drawOne(l2_basePosX2, l2_basePosY2, l2_offsetX2, l2_dirX2);
	if (l2_alive3) l2_drawOne(l2_basePosX3, l2_basePosY3, l2_offsetX3, l2_dirX3);
	if (l2_alive4) l2_drawOne(l2_basePosX4, l2_basePosY4, l2_offsetX4, l2_dirX4);
	if (l2_alive6) l2_drawOne(l2_basePosX6, l2_basePosY6, l2_offsetX6, l2_dirX6);
	//if (l2_alive7) l2_drawOne(l2_basePosX7, l2_basePosY7, l2_offsetX7, l2_dirX7);

	for (int i = 0; i<l2_totalCoins; i++)
	if (l2_coins[i].active)
		iShowImage(l2_coins[i].x, l2_coins[i].y, l2_coins[i].w, l2_coins[i].h, coinImg);

	if (l2_fishbone.active){
		iShowImage(l2_fishbone.x, l2_fishbone.y, l2_fishbone.w, l2_fishbone.h, fishboneImg);
	}

	if (l2_bone.active){
		iShowImage(l2_bone.x, l2_bone.y, l2_bone.w, l2_bone.h, fishboneImg);
	}

	if (isJumping2){
		int faceRight = (jumpDirection2 == 1) || (direction2 == 0);
		imgToShow = faceRight ? rightJumpImg : leftJumpImg;
	}
	else {
		int frame = isWalking ? currentFrame2 : 0;  /* idle unless a keypress moved */
		if (direction2 == 1) imgToShow = leftWalk[frame];
		else                 imgToShow = rightWalk[frame];
	}

	iShowImage(l2_catX, l2_catY, CAT_DRAW_W, CAT_DRAW_H, imgToShow);

	l2_drawHint();
	drawHUD();
}
void level2Screen(){ l2_draw(); }
//endpoint check
static inline void l2_checkEndReached() {
	bool xOverlap = ((l2_catX + CAT_W) > endGoal2.x) && (l2_catX < (endGoal2.x + endGoal2.w));
	bool yOverlap = ((l2_catY + CAT_H) > endGoal2.y) && (l2_catY < (endGoal2.y + endGoal2.h));
	if (xOverlap && yOverlap){
		saveScoreToFile();   /* keep file up to date */
		openWinScreen(2);    /* will show the L2 win bg */
	}
}

/* ======= About Us Screen (drawing) ======= */
void aboutusScreen(void) {
	iClear();
	iShowImage(0, 0, 1280, 720, aboutusPage);

	if (ishover){
		iShowImage(15, 60, 300, 130, about_back_h);
	}
	else{
		iShowImage(15, 60, 300, 130, about_backbutton);
	}

	int img = (abtDog_dirX >= 0) ? abtDog_imagesLeft[abtDog_currentFrame] : abtDog_imagesRight[abtDog_currentFrame];
	iShowImage(abtDog_baseX + abtDog_offsetX, abtDog_baseY, abtDog_W, abtDog_H, img);
}

void l2_reset(){
	l2_catX = 560; l2_catY = 693;
	for (int i = 0; i<l2_totalCoins; i++) l2_coins[i].active = 1;

	l2_fishbone.x = 807; l2_fishbone.y = 564;
	l2_fishbone.w = 40;  l2_fishbone.h = 40;
	l2_fishbone.active = 1;
	l2_canShoot = 0;

	//l2_alive1 = l2_alive2 = l2_alive3 = l2_alive4 = l2_alive6 = l2_alive7 = 1;

	l2_bone.active = 0; l2_bone.travel = 0; l2_bone.vx = 0;
}

void l2_init(){
	initpipe_lvl2();
	initSpikes_lvl2();
	l2_initCoins();

	/* load dog sprites */
	l2_imageRight[0] = iLoadImage("newdog11.png");
	l2_imageRight[1] = iLoadImage("newdog22.png");
	l2_imageRight[2] = iLoadImage("newdog33.png");
	l2_imageRight[3] = iLoadImage("newdog44.png");
	l2_imageRight[4] = iLoadImage("newdog55.png");
	l2_imageRight[5] = iLoadImage("newdog66.png");

	l2_imageLeft[0] = iLoadImage("newdog1.png");
	l2_imageLeft[1] = iLoadImage("newdog2.png");
	l2_imageLeft[2] = iLoadImage("newdog3.png");
	l2_imageLeft[3] = iLoadImage("newdog4.png");
	l2_imageLeft[4] = iLoadImage("newdog5.png");
	l2_imageLeft[5] = iLoadImage("newdog6.png");

	l2_catImg = iLoadImage("cat_left_0.png");

	fishboneImg = iLoadImage("fishbone.png");
	l2_fishbone.x = 807; l2_fishbone.y = 564;
	l2_fishbone.w = 40;  l2_fishbone.h = 40;
	l2_fishbone.active = 1;
	l2_canShoot = 0;

	l2_bone.active = 0; l2_bone.travel = 0; l2_bone.vx = 0; l2_bone.w = 32; l2_bone.h = 16;

	iSetTimer(60, l2_updateAnimation);
	iSetTimer(80, l2_updateRats);
}

void level3Screen() {
	iClear();
	iSetColor(255, 255, 255);
	iText(550, 360, "LEVEL 3 SCREEN", GLUT_BITMAP_TIMES_ROMAN_24);
	drawHUD();
}

void winTransition(void){
	if (transitionLevel == 1) iShowImage(0, 0, 1280, 720, winBgL1);
	else                      iShowImage(0, 0, 1280, 720, winBgL2);

	/* --- Show score on the win page --- */
	iSetColor(255, 255, 0);
	char buf[128];


	sprintf(buf, "SCORE: %d", scoreCount);
	iText(617, 340, buf, GLUT_BITMAP_TIMES_ROMAN_24);
}

void loseTransition(void){
	if (transitionLevel == 1) iShowImage(0, 0, 1280, 720, loseBgL1);
	else                      iShowImage(0, 0, 1280, 720, loseBgL2);

	/* (No score text here unless you want it; easy to add if needed) */
}

void openWinScreen(int level){
	mciSendString("play \"level win.wav\"", NULL, 0, NULL);
	transitionLevel = level;
	currentScreen = winTransition;
}
void openLoseScreen(int level){
	mciSendString("play \"level lost.wav\"", NULL, 0, NULL);
	transitionLevel = level;
	currentScreen = loseTransition;
}

/* -------music on/off-------- */
void playMusic(){
	PlaySound("bgmfinal.wav", NULL, SND_LOOP | SND_ASYNC);
	mciSendString("play bgm repeat", NULL, 0, NULL);
	musicOn = true;
}
void pauseMusic(){
	//PlaySound(NULL, 0, 0);
	mciSendString("pause bgm", NULL, 0, NULL);
	musicOn = false;
}
void scorepage() {
	iShowImage(0, 0, screenWidth, screenheight, Scorepage);

	iSetColor(255, 255, 0);
	char buf[160];

	/* live (in-memory) score */
	sprintf(buf, "CURRENT SCORE: %d", scoreCount);
	iText(508, 425, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	/* Saved data */
	int savedLast = loadLastScoreFromFile();
	int savedHigh = loadHighScoreFromFile();

	sprintf(buf, "HIGH SCORE: %d", savedHigh);
	iText(508, 380, buf, GLUT_BITMAP_TIMES_ROMAN_24);


}

void helppage() {
	iShowImage(0, 0, screenWidth, screenheight, Helppage);
}
/* =============================== MAIN =============================== */
int main() {
	iInitialize(screenWidth, screenheight, "Project Title");

	bg = iLoadImage("bufferpage.png");
	homepage = iLoadImage("homepage.png");
	mainmenu = iLoadImage("mainmenu.png");
	Scorepage = iLoadImage("scorepage.png");
	Helppage = iLoadImage("helppage.png");

	start = iLoadImage("start.png"); start_h = iLoadImage("start_h.png");
	score = iLoadImage("score.png"); score_h = iLoadImage("score_h.png");
	help = iLoadImage("help.png"); help_h = iLoadImage("help_h.png");
	aboutus = iLoadImage("aboutus.png");  aboutus_h = iLoadImage("aboutus_h.png");
	Exit = iLoadImage("Exit.png"); Exit_h = iLoadImage("Exit_h.png");

	aboutusPage = iLoadImage("aboutusfinal.png");
	about_backbutton = iLoadImage("back.png");
	about_back_h = iLoadImage("back_h.png");

	map = iLoadImage("map.png");

	level1 = iLoadImage("level1cbg_resized.png");
	level2bg = iLoadImage("level2.png");

	initpipe();
	initSpikes();
	initCoins();
	loadFrameslvl1();


	winBgL1 = iLoadImage("LEVEL1_COMPLETED.png");
	winBgL2 = iLoadImage("LEVEL2_COMPLETED.png");
	loseBgL1 = iLoadImage("LEVEL1_LOST.png");
	loseBgL2 = iLoadImage("LEVEL2_LOST.png");



	/* Endpoints */
	initEndPoint();   /* L1 existing */
	initEndPoint2();  /* L2 new */

	coinImg = iLoadImage("coin.png");

	/* Level 2 assets/timers */
	l2_init();

	/* About Us Dog frames (reuse the same files as L2 dog) */
	abtDog_imagesRight[0] = iLoadImage("newdog11.png");
	abtDog_imagesRight[1] = iLoadImage("newdog22.png");
	abtDog_imagesRight[2] = iLoadImage("newdog33.png");
	abtDog_imagesRight[3] = iLoadImage("newdog44.png");
	abtDog_imagesRight[4] = iLoadImage("newdog55.png");
	abtDog_imagesRight[5] = iLoadImage("newdog66.png");

	abtDog_imagesLeft[0] = iLoadImage("newdog1.png");
	abtDog_imagesLeft[1] = iLoadImage("newdog2.png");
	abtDog_imagesLeft[2] = iLoadImage("newdog3.png");
	abtDog_imagesLeft[3] = iLoadImage("newdog4.png");
	abtDog_imagesLeft[4] = iLoadImage("newdog5.png");
	abtDog_imagesLeft[5] = iLoadImage("newdog6.png");

	iSetTimer(80, abtDog_update);

	iSetTimer(40, mainTimer);
	iSetTimer(80, updateStoryTyping);
	iSetTimer(60, FrameChange1);

	mciSendString("open \"bgmfinal.wav\" type waveaudio alias bgm", NULL, 0, NULL);

	playMusic();
	iStart();
	return 0;
}