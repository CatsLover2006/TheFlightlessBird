// The Flightless Bird
// By Half-Qilin

#include <citro2d.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240

class ellipseObj {
    public:
        float x;
        float y;
        float w;
        float h;
        float movX;
        float movY;
        long tmr;
        long maxTmr;
        bool polongInSelf(float locationX, float locationY) {
            float distX = abs(this->x - locationX);
            float distY = abs(this->y - locationY);
            distX /= this->w;
            distY /= this->h;
            return (distX * distX + distY * distY < 1);
        }

        ellipseObj(float x, float y, float w, float h) {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
            this->movX = 0;
            this->movY = 0;
            this->tmr = 0;
            this->maxTmr = 0;
        }

        ellipseObj(float x, float y, float w, float h, float movX, float movY, long maxTmr) {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
            this->movX = movX;
            this->movY = movY;
            this->tmr = 0;
            this->maxTmr = maxTmr;
        }

        void moveObj () {
            this->x += this->movX;
            this->y += this->movY;
            this->tmr++;
            if (this->tmr > this->maxTmr) {
                this->tmr = 0;
                this->movX *= -1;
                this->movY *= -1;
            }
        }
};

class player {
    public:
        float x;
        float xMov = 0;
        float yMov = 0;
        float y;
        float r;

        bool isColliding(ellipseObj* ellipseObj) {
            float distX = (this->x - ellipseObj->x) / ellipseObj->w * sqrt(ellipseObj->h);
            float distY = (this->y - ellipseObj->y) / ellipseObj->h * sqrt(ellipseObj->w);
            float locationX = this->x - this->r * (distX / sqrt(distY * distY + distX * distX));
            float locationY = this->y - this->r * (distY / sqrt(distY * distY + distX * distX));
            return ellipseObj->polongInSelf(locationX, locationY);
        }

        bool isColliding(float x, float y, float r) {
            float distX = (this->x - x);
            float distY = (this->y - y);
            float tR = this->r + r;
            return distX * distX + distY * distY < tR * tR;
        }

        bool uncollide(ellipseObj* ellipseObj) {
            float distX = (this->x - ellipseObj->x) / ellipseObj->w;
            float distY = (this->y - ellipseObj->y) / ellipseObj->h;
            float mov = sqrt(distY * distY + distX * distX);
            float movX = distX / mov * 0.01;
            float movY = distY / mov * 0.01;
            while (this->isColliding(ellipseObj)) {
                this->x += movX;
                this->y += movY;
            }
            return abs(distX) < abs(distY) * 2;
        }
		
		void uncollide(ellipseObj* ellipseObj, float maintainX) {
            float distY = abs(this->y - ellipseObj->y) / (this->y - ellipseObj->y) * 0.01f;
            while (this->isColliding(ellipseObj)) {
                this->x = maintainX;
                this->y += distY;
            }
        }

        float getDistRatio(ellipseObj* ellipseObj) {
            return ((this->x - ellipseObj->x) / ellipseObj->w) / ((this->y - ellipseObj->y) / ellipseObj->h);
        }

        player(float x, float y, float r) {
            this->x = x;
            this->y = y;
            this->r = r;
        }
};

player* playerObj = new player(0, 0, 0);
float scrollX = 0;
float scrollY = 0;

class level {
    private:
        ellipseObj** startEllipseObjs;
        void copyIntoCurrent() {
            for (long j = 0; j < this->ellipses; j++) {
                this->ellipseObjs[j] = this->startEllipseObjs[j];
            }
        }
    public:
        ellipseObj** ellipseObjs;
        long ellipses = 0;
        float playerStartX, playerStartY, playerSize;
        float maxY, minX, maxX, minY;
        float endX, endY, endR;
        long exitTo = 0;

        level() {
            // nada
        }

        level(ellipseObj** ellipseObjList, long ellipses, float playerStartX, float playerStartY, float playerSize, float maxY, float minY, float minX, float maxX, float endX, float endY, float endR, long exitTo) {
            this->startEllipseObjs = ellipseObjList;
            this->ellipseObjs = new ellipseObj*[ellipses];
            this->ellipses = ellipses;
            this->playerStartX = playerStartX;
            this->playerStartY = playerStartY;
            this->playerSize = playerSize;
            this->maxY = maxY;
            this->minY = minY;
            this->maxX = maxX;
            this->minX = minX;
            this->endY = endY;
            this->endX = endX;
            this->endR = endR;
            this->exitTo = exitTo;
        }

        void startLevel() {
			playerObj->x = this->playerStartX;
			playerObj->y = this->playerStartY;
			playerObj->r = this->playerSize;
			playerObj->xMov = 0;
			playerObj->yMov = 0;
            scrollX = this->playerStartX;
            scrollY = this->playerStartY;
            this->copyIntoCurrent();
        }
};

int main(int argc, char * argv[]) {
    // Init libs
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_BOTTOM, NULL);

    // Create screens
    C3D_RenderTarget * top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    // Create colors
    u32 clrClear = C2D_Color32(66, 176, 245, 0xFF);
    u32 clrGround = C2D_Color32(246, 250, 235, 0xFF);
    u32 clrCloud = C2D_Color32(246, 250, 235, 80);
    u32 clrPlayer = C2D_Color32(224, 4, 195, 0xFF);
    u32 clrEnd = C2D_Color32(224, 4, 195, 0xFF);

    // Game Vars
    level* levels[1];
    long timeToJump = 0;
    long levelIndex = 0;

    // Level Generation
    ellipseObj* level1Ellipses[8] = {
        new ellipseObj(440.0f, 980.0f, 80.0f, 40.0f),
        new ellipseObj(530.0f, 1000.0f, 80.0f, 80.0f),
        new ellipseObj(140.0f, 600.0f, 100.0f, 40.0f, 0.125f, -0.25f, 400l),
        new ellipseObj(740.0f, 900.0f, 100.0f, 30.0f),
        new ellipseObj(940.0f, 850.0f, 40.0f, 20.0f),
        new ellipseObj(740.0f, 800.0f, 100, 30.0f),
        new ellipseObj(500.0f, 720.0f, 50.0f, 25.0f),
        new ellipseObj(380.0f, 640.0f, 50.0f, 25.0f)
    };
    levels[0] = new level(level1Ellipses, 8l, 440.0f, 940.0f, 10.0f, 1000.0f, 0.0f, 0.0f, 1000.0f, 190.0f, 440.0f, 10.0f, 0l);
    levels[levelIndex]->startLevel();

    // Main loop
    while (aptMainLoop()) {
        // Debug
		printf("\x1b[1;1HThe Flightless Bird");
		printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[4;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);

        // Get Input
        hidScanInput();
        u32 kHeld = hidKeysHeld();
        if (kHeld & KEY_START)
            break; // break in order to return to hbmenu, temp

        // Physics Steps
        for (long i = 0; i < 8; i++) {
            playerObj->yMov += 0.03125;
            playerObj->xMov *= pow(0.9, 1 / 4.0f);
            if (kHeld & KEY_LEFT)
                playerObj->xMov -= 1 / 16.0f;
            if (kHeld & KEY_RIGHT)
                playerObj->xMov += 1 / 16.0f;
            playerObj->y += playerObj->yMov / 25 * playerObj->r;
            playerObj->x += playerObj->xMov / 25 * playerObj->r;
            timeToJump--;
            for (long j = 0; j < levels[levelIndex]->ellipses; j++) {
                levels[levelIndex]->ellipseObjs[j]->moveObj();
                float beforeX = playerObj->x;
                if (playerObj->isColliding(levels[levelIndex]->ellipseObjs[j])) {
                    if (playerObj->uncollide(levels[levelIndex]->ellipseObjs[j])) {
                        if (playerObj->y < levels[levelIndex]->ellipseObjs[j]->y) {
                            playerObj->yMov = abs(playerObj->getDistRatio(levels[levelIndex]->ellipseObjs[j]));
                            timeToJump = 48;
                            playerObj->x = beforeX + levels[levelIndex]->ellipseObjs[j]->movX;
							playerObj->uncollide(levels[levelIndex]->ellipseObjs[j], playerObj->x);
                            playerObj->y += levels[levelIndex]->ellipseObjs[j]->movY;
                        }
                    } else {
                        if (playerObj->x > levels[levelIndex]->ellipseObjs[j]->x) playerObj->xMov = fmax(playerObj->xMov, 0);
                        else playerObj->xMov = fmin(playerObj->xMov, 0);
                    }
                }
            }
            if (playerObj->x - playerObj->r < levels[levelIndex]->minX) {
                playerObj->x = levels[levelIndex]->minX + playerObj->r;
            }
            if (playerObj->x + playerObj->r > levels[levelIndex]->maxY) {
                playerObj->x = levels[levelIndex]->maxX - playerObj->r;
            }
            if (timeToJump > 0 && (kHeld & KEY_A)) {
                playerObj->yMov = -3.5;
                timeToJump = 0;
            }
        }
        float camDistX = (((playerObj->x - scrollX) < 0) ? -1 : 1) * sqrt(abs(playerObj->x - scrollX)) * 2;
        float camDistY = (((playerObj->y - scrollY) < 0) ? -1 : 1) * sqrt(abs(playerObj->y - scrollY)) * 2;
        float deltaX = playerObj->x - scrollX;
        float deltaY = playerObj->y - scrollY;
        scrollX += camDistX;
        scrollY += camDistY;
        if ((deltaX > 0) != ((playerObj->x - scrollX) > 0)) scrollX = playerObj->x;
        if ((deltaY > 0) != ((playerObj->y - scrollY) > 0)) scrollY = playerObj->y;
        scrollX = fmin(levels[levelIndex]->maxX - SCREEN_WIDTH / 2, fmax(levels[levelIndex]->minX + SCREEN_WIDTH / 2, scrollX));
        scrollY = fmin(levels[levelIndex]->maxY - SCREEN_HEIGHT / 2, fmax(levels[levelIndex]->minY + SCREEN_HEIGHT / 2, scrollY));

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, clrClear);
		C2D_SceneBegin(top);
        float offsetX = SCREEN_WIDTH / 2 - scrollX;
        float offsetY = SCREEN_HEIGHT / 2 - scrollY;
        for (long j = 0; j < levels[levelIndex]->ellipses; j++) {
            C2D_DrawEllipseSolid(levels[levelIndex]->ellipseObjs[j]->x + offsetX - levels[levelIndex]->ellipseObjs[j]->w, levels[levelIndex]->ellipseObjs[j]->y + offsetY - levels[levelIndex]->ellipseObjs[j]->h, 0.2f, levels[levelIndex]->ellipseObjs[j]->w * 2, levels[levelIndex]->ellipseObjs[j]->h * 2, clrGround);
        }
        C2D_DrawCircleSolid(playerObj->x + offsetX, playerObj->y + offsetY, 0.4f, playerObj->r, clrPlayer);
        C2D_DrawCircleSolid(levels[levelIndex]->endX + offsetX, levels[levelIndex]->endY + offsetY, 0.5f, levels[levelIndex]->endR, clrEnd);
        for (long j = 0; j < levels[levelIndex]->ellipses; j++) {
            C2D_DrawEllipseSolid(levels[levelIndex]->ellipseObjs[j]->x + offsetX - levels[levelIndex]->ellipseObjs[j]->w - 10, levels[levelIndex]->ellipseObjs[j]->y + offsetY - levels[levelIndex]->ellipseObjs[j]->h - 10, 0.75f, levels[levelIndex]->ellipseObjs[j]->w * 2 + 20, levels[levelIndex]->ellipseObjs[j]->h * 2 + 20, clrCloud);
        }
        if (playerObj->isColliding(levels[levelIndex]->endX, levels[levelIndex]->endY, levels[levelIndex]->endR)) {
            levelIndex = levels[levelIndex]->exitTo;
            levels[levelIndex]->startLevel();
        }
        if (playerObj->y - playerObj->r > levels[levelIndex]->maxY) levels[levelIndex]->startLevel();
        C3D_FrameEnd(0);
    }

    // Deinit libs
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}