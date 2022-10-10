#include "GL/freeglut.h"
#include <stdlib.h>
#include <random>
#include <string>
#include <math.h>
#include <time.h>
#include <vector>
//#include <winuser.h>

#define WIDTH 640
float Pi = 3.1416f;
using namespace std;
random_device rd;
mt19937 mt(rd());
uniform_real_distribution<double> dist(-10.0, 10.0);

enum State {
	MENU,
	ACTIVE,
	END
};
enum Skill {
	NONE,
	BIGGER,
	SPEEDUP,
	INCREASE,
	DIAMOND
};
struct Drop {
	int score = 0;
	float x = dist(mt);
	float y = 10;
	float r = 0.5;
	float v = 0.6 + dist(mt) / 200;
	int red;
	int green;
	int blue;
	Skill skill = NONE;
	Drop(int score, float v, int red, int green, int blue) :
		score(score), v(v), red(red), green(green), blue(blue) {}
	Drop(int red, int green, int blue, Skill skill) :
		red(red), green(green), blue(blue), skill(skill) {}
};

struct Player {
	float x = 0;
	float y = -8.5;
	float r = 1.5;
	float v = 0.8;
};
State state = MENU;
int menu = 1;
clock_t time_begin;
clock_t time_pre;
clock_t time_pre_ac;
clock_t time_pre_as;
int remain;
Player player;
vector<struct Drop> drops;
int score = 0;

double aspect_ratio = 0;
void reshape(int w, int h)
{
	aspect_ratio = (double)w / (double)h;
	glViewport(0, 0, w, h);
}

void checkPress() {
	switch (state) {
	case MENU:
		break;
	case ACTIVE:
		if (GetAsyncKeyState(VK_LEFT) < 0) {
			player.x > -10 ? player.x -= player.v : player.x = -10;
		}
		else if (GetAsyncKeyState(VK_RIGHT) < 0) {
			player.x < 10 ? player.x += player.v : player.x = 10;
		}
		break;
	case END:
		break;
	default:
		exit(-1);
		break;
	}
}
void processNormalKeys(unsigned char key, int x, int y) {
	if (key == 27) { //esc
		exit(0);
	}
	switch (state) {
	case MENU:
		if (key == 32) {
			if (menu == 1) {
				state = ACTIVE;
				drops.clear();
				score = 0;
				time_begin = clock();
				time_pre = time_begin;
				time_pre_ac = time_begin;
				time_pre_as = time_begin - 5000;
			}
			else
				exit(0);
		}
		break;
	case ACTIVE:
		break;
	case END:
		if (key == 32) {
			state = MENU;
		}
		break;
	default:
		exit(-1);
		break;
	}		
}
void processSpecialKeys(int key, int x, int y) {
	switch (state) {
	case MENU:
		if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN)
			menu *= -1;
		break;
	case ACTIVE:
		/*if (key == GLUT_KEY_LEFT) {
			player.x > -10 ? player.x -= player.v : player.x = -10;
		}
		else if (key == GLUT_KEY_RIGHT) {
			player.x < 10 ? player.x += player.v : player.x = 10;
		}*/
		break;
	case END:
		break;
	default:
		break;
	}
}

void drawText(string text, float x, float y, float size) {
	glColor3ub(255, 255, 255);
	float unit = 104.76f*2;
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size / unit, size / unit, size / unit);
	for (auto it = text.begin(); it != text.end(); it++) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *it);
	}
	glPopMatrix();
}
void drawArrow(float x, float y, float size) {
	glColor3ub(255, 255, 255);
	size = size / 24;
	glBegin(GL_LINE_STRIP);
		glVertex3f(x, y, 0.0f);
		glVertex3f(x + size/2, y - size/2, 0.0f);
		glVertex3f(x, y - size, 0.0f);
	glEnd();
}

void drawCircle(float x, float y, float r) {
	int triangleAmount = 50;
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, 0.0f); //center
		for (int i = 0; i <= triangleAmount; i++) {
			glVertex2f(
				x + (r * cos(i * 2 * Pi / triangleAmount)),
				y + (r * sin(i * 2 * Pi / triangleAmount))
			);
		}
	glEnd();
}
void drawSquare(float x, float y, float r) {
	int scale = 1.5;
	glBegin(GL_POLYGON);
		glVertex2f(x, y + r / scale);//up
		glVertex2f(x + r / scale, y);
		glVertex2f(x, y - r / scale);
		glVertex2f(x - r / scale, y);
	glEnd();
}
void drawDiamond(float x, float y, float r) {
	int scale = 1.5;
	glBegin(GL_POLYGON);
		glVertex2f(x - r / (2 * scale), y + r / (2*scale));
		glVertex2f(x + r / (2 * scale), y +  r / (2 * scale));//up
		glVertex2f(x + r / scale, y);
		glVertex2f(x, y - r / scale);
		glVertex2f(x - r / scale, y);
	glEnd();
}
void drawPlayer() {
	glColor3ub(253, 129, 214);
	drawCircle(player.x, player.y, player.r);
}
void randomCoin() {
	Drop gold(50, 0.6 + dist(mt) / 200, 246, 227, 49);
	Drop silver(10, 0.4 + dist(mt) / 200, 207, 207, 216);
	Drop cooper(5, 0.3 + dist(mt) / 200, 167, 84, 17);
	float ran = dist(mt);
	if (ran > 0)
		drops.push_back(cooper);
	else if (ran > -7)
		drops.push_back(silver);
	else
		drops.push_back(gold);
}
void addCoin() {
	clock_t time_now = clock();
	if (time_now - time_pre_ac > 500) {
		randomCoin();
		time_pre_ac = time_now;
	}
}
void randomSkill() {
	Drop bigger(160, 247, 156, BIGGER);
	Drop speedup(160, 247, 156, SPEEDUP);
	Drop increase(160, 247, 156, INCREASE);
	Drop diamond(156, 236, 247, DIAMOND);
	diamond.score = 1000;
	float ran = dist(mt);
	if (ran > 5)
		drops.push_back(bigger);
	else if (ran > 0)
		drops.push_back(speedup);
	else if (ran > -5)
		drops.push_back(increase);
	else
		drops.push_back(diamond);
}
void addSkill() {
	clock_t time_now = clock();
	if (time_now - time_pre_as > 10000) {
		randomSkill();
		time_pre_as = time_now;
	}
}

clock_t skill_begin;
Skill skill_on = NONE;
bool execSkill(Skill skill) {
	if (skill == NONE || skill == DIAMOND)
		return false;

	skill_begin = clock();
	if (skill == BIGGER) {
		player.r *= 2;
		player.y += 1.5;
		skill_on = BIGGER;
	}
	else if (skill == SPEEDUP) {
		player.v *= 2;
		skill_on = SPEEDUP;
	}
	else if (skill == INCREASE) {
		skill_on = INCREASE;
		return true;
	}
	return false;
}
void removeSkill() {
	clock_t time_now = clock();
	if (time_now - skill_begin > 5000) {
		skill_begin = time_now;
		if (skill_on == BIGGER) {
			player.r /= 2;
			player.y -= 1.5;
		}
		else if (skill_on == SPEEDUP) {
			player.v /= 2;
		}
		skill_on = NONE;
	}
	else {
		string sk = "NONE";
		if (skill_on == BIGGER)
			sk = "BIGGER";
		else if (skill_on == SPEEDUP)
			sk = "SPEEDUP";
		else
			sk = "NONE";

		drawText("skill: "+sk, -12, 6, 2);
	}
}
bool dropStuff(Drop& drop) {
	int bottom = -11;
	if (drop.y > bottom) {
		drop.y -= drop.v;
		return false;
	}
	else {
		drop.y = bottom;
		return true;
	}
}
void updateStuff() {
	clock_t time_now = clock();
	if (time_now - time_pre > 50) {
		checkPress();
		for (auto it = drops.begin(); it != drops.end();) {
			if (dropStuff(*it)) {
				it = drops.erase(it);
			}
			else {
				it++;
			}
		}
		time_pre = time_now;
	}
}

void detectCollision() {
	for (auto it = drops.begin(); it != drops.end();) {
		float dist_x = abs(player.x - it->x);
		float dist_y = abs(player.y - it->y);
		float dist = hypot(dist_x, dist_y);
		if (dist < player.r + it->r) {
			score += it->score;
			if (execSkill(it->skill)) {
				drops.erase(it);
				for (int i = 0; i < 50; i++)
					randomCoin();
				it = drops.begin();
			}
			else {
				it = drops.erase(it);
			}
		}
		else {
			it++;
		}
	}
}
void drawStuff() {
	for (auto it = drops.begin(); it != drops.end();it++) {
		glColor3ub(it->red, it->green, it->blue);
		if(it->skill == NONE)
			drawCircle(it->x, it->y, it->r);
		else if (it->skill == DIAMOND)
			drawDiamond(it->x, it->y, it->r);
		else
			drawSquare(it->x, it->y, it->r);
	}
	detectCollision();
	updateStuff();
}

void drawTime() {
	int time_spent = (clock() - time_begin)/1000;
	int limit = 60;
	remain = limit - time_spent;
	if (time_spent == limit) {
		state = END;
		return;
	}
	drawText(to_string(remain), -12, 8, 2);
}
void drawScore(float x, float y, float size) {
	char buf[10];
	string tmp;
	sprintf_s(buf, "%04d", score);
	tmp = buf;
	drawText(tmp, x, y, size);
}

void display(void) {
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-10 * aspect_ratio, 10 * aspect_ratio, -10, 10, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	switch (state) {
	case MENU:
		drawText("Coin Catcher", -7.5, 2, 2.5);
		drawText("- Start", -5, -1, 1.5);
		drawText("- Quit", -5, -4, 1.5);
		drawText("Use space and arrow keys to play.", -12, -9, 1);
		if(menu == 1) {
			drawArrow(-4.55f, -0.37f, 15.0f);
		}
		else {
			drawArrow(-4.55f, -3.37f, 15.0f);
		}
		glutSwapBuffers();
		break;
	case ACTIVE:
		drawTime();
		drawScore(8, 8, 2);
		drawPlayer();
		addCoin();
		addSkill();
		drawStuff();
		removeSkill();
		glutSwapBuffers();
		break;
	case END:
		drawText("Your Score", -5, 5, 2);
		drawScore(-10, -3, 10);
		drawText("Press space to continue.", -6, -6, 1);
		glutSwapBuffers();
		break;
	default:
		exit(0);
		break;
	}
}

void init(void) {
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);

	// glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
}

int main(int argc, char** argv) {
	int mode = GLUT_DOUBLE | GLUT_RGBA;
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(mode);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Coin Catcher");
	init();
	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}