// The Flightless Bird
// By Half-Qilin

float ellipsePerimiter (float x, float y) {
    float c = PI * (x + y);
    float t = pow(x-y, 2);
    float l = pow(x+y, 2);
    c *= 3 * (t * t)/(l*l*(sqrt(-3 * ((t*t)/(l*l)) +3)+10))+1;
    return c;
}

class player {
    float x;
    float xMov = 0;
    float yMov = 0;
    float y;
    float r;

    boolean isColliding(ellipseObj ellipseObj) {
        float distX = (this.x - ellipseObj.x) / ellipseObj.w * sqrt(ellipseObj.h);
        float distY = (this.y - ellipseObj.y) / ellipseObj.h * sqrt(ellipseObj.w);
        float locationX = this.x - this.r * (distX / sqrt(distY * distY + distX * distX));
        float locationY = this.y - this.r * (distY / sqrt(distY * distY + distX * distX));
        return ellipseObj.pointInSelf(locationX, locationY);
    }
    
    boolean isColliding(float x, float y, float r) {
        float distX = (this.x - x);
        float distY = (this.y - y);
        float tR = this.r + r;
        return distX * distX + distY * distY < tR * tR;
    }

    boolean uncollide(ellipseObj ellipseObj) {
        float distX = (this.x - ellipseObj.x) / ellipseObj.w;
        float distY = (this.y - ellipseObj.y) / ellipseObj.h;
        float mov = sqrt(distY * distY + distX * distX);
        float movX = distX / mov * 0.01;
        float movY = distY / mov * 0.01;
        while (this.isColliding(ellipseObj)) {
            this.x += movX;
            this.y += movY;
        }
        return Math.abs(distX) < Math.abs(distY) * 2;
    }
    
    boolean uncollide(ellipseObj ellipseObj, float xMaintain) {
        float distX = (this.x - ellipseObj.x) / ellipseObj.w;
        float distY = (this.y - ellipseObj.y) / ellipseObj.h;
        float mov = sqrt(distY * distY + distX * distX);
        float movX = distX / mov * 0.01;
        float movY = distY / mov * 0.01;
        while (this.isColliding(ellipseObj)) {
            this.x = xMaintain;
            this.y += movY;
        }
        return Math.abs(distX) < Math.abs(distY) * 2;
    }

    float getDistRatio(ellipseObj ellipseObj) {
        return ((this.x - ellipseObj.x) / ellipseObj.w) / ((this.y - ellipseObj.y) / ellipseObj.h);
    }

    public player(float x, float y, float r) {
        this.x = x;
        this.y = y;
        this.r = r;
    }
}

class visualObj {
    float[] xValues;
    float[] yValues;
    float[] zValues;
    float[] radii;
    boolean[] onBack;
    int count;
    
    public visualObj(float w, float h) {
      count = floor(sqrt(ellipsePerimiter (w, h)));
      xValues = new float[count];
      yValues = new float[count];
      zValues = new float[count];
      radii = new float[count];
      for (int j = 0; j < count; j++) {
        boolean test = true;
        while (test) {
          float r = (count-j) * TWO_PI / count; //random(0, TWO_PI)
          float len = random(0.25, 0.5);
          float temp = min(w, h);
          xValues[j] = (sin(r) * temp * len);
          yValues[j] = (cos(r) * temp * len);
          xValues[j] *= (w/temp);
          yValues[j] *= (h/temp);
          xValues[j] *= sqrt(w/temp);
          yValues[j] *= sqrt(h/temp);
          xValues[j] *= 1.01;
          yValues[j] *= 1.01;
          zValues[j] = random(-5, 5);
          radii[j] = (1 - len) * min(w, h); // (count - j)* random(min(w, h), min(w, h)/1.1)/count
          test = false;
          for (int k = 0; k < j; k++) {
            if (!test && pow(xValues[k] -xValues[j], 2) + pow(yValues[k] - yValues[j], 2) + pow(zValues[k] - zValues[j], 2) < pow((radii[k] - radii[j])/2, 2)) {
              test = true;
            }
          }
        }
      }
    }
}

class ellipseObj {
    float x;
    float y;
    float w;
    float h;
    float movX;
    float movY;
    int tmr;
    int maxTmr;
    boolean pointInSelf(float locationX, float locationY) {
        float distX = abs(this.x - locationX);
        float distY = abs(this.y - locationY);
        distX /= this.w;
        distY /= this.h;
        return (distX * distX + distY * distY < 1);
    }
    
    void moveObj () {
      x += movX;
      y += movY;
      tmr++;
      if (tmr > maxTmr) {
        tmr = 0;
        movX *= -1;
        movY *= -1;
      }
    }

    public ellipseObj(float x, float y, float w, float h) {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
        this.movX = 0;
        this.movY = 0;
        this.tmr = 0;
        this.maxTmr = 0;
    }

    public ellipseObj(float x, float y, float w, float h, float movX, float movY, int maxTmr) {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
        this.movX = movX;
        this.movY = movY;
        this.tmr = 0;
        this.maxTmr = maxTmr;
    }
}

player playerObj = null;
ellipseObj[] ellipseObjs;

class level {
    ellipseObj[] ellipseObjsInternal;
    visualObj[] visualObjs;
    boolean isSetUp;
    float playerStartX, playerStartY, playerSize;
    float maxY, minX, maxX, minY;
    float endX, endY, endR;
    int exitTo = 0;

    level(ellipseObj[] ellipseObjList, float playerStartX, float playerStartY, float playerSize, float maxY, float minY, float minX, float maxX, float endX, float endY, float endR, int exitTo) {
        this.ellipseObjsInternal = ellipseObjList;
        this.playerStartX = playerStartX;
        this.playerStartY = playerStartY;
        this.playerSize = playerSize;
        this.maxY = maxY;
        this.minY = minY;
        this.maxX = maxX;
        this.minX = minX;
        this.endY = endY;
        this.endX = endX;
        this.endR = endR;
        this.exitTo = exitTo;
        isSetUp = false;
    }

    void startLevel() {
        if (!isSetUp) {
            visualObjs = new visualObj[ellipseObjsInternal.length];
            for (int j = 0; j < ellipseObjsInternal.length; j++) {
                visualObjs[j] = new visualObj(ellipseObjsInternal[j].w, ellipseObjsInternal[j].h);
            }
        }
        isSetUp = true;
        playerObj.x = this.playerStartX;
        playerObj.y = this.playerStartY;
        playerObj.r = this.playerSize;
        playerObj.xMov = 0;
        playerObj.yMov = 0;
        ellipseObjs = this.ellipseObjsInternal;
        scrollX = this.playerStartX;
        scrollY = this.playerStartY;
    }
}


level[] levels = new level[1];
int levelIndex = 0;
float scrollX = 0;
float scrollY = 0;

boolean[] controls = {
    false,
    false,
    false
};

void keyPressed() {
    if (keyCode == 37) controls[0] = true;
    if (keyCode == 39) controls[1] = true;
    if (keyCode == 38) controls[2] = true;
}

void keyReleased() {
    if (keyCode == 37) controls[0] = false;
    if (keyCode == 39) controls[1] = false;
    if (keyCode == 38) controls[2] = false;
}

int timeToJump = 0;

void setup() {
    ellipseMode(RADIUS);
    size(500, 500, P3D);
    smooth(16);
    playerObj = new player(0,0,0);
    ellipseObj[] ellipseGenerator = {
        new ellipseObj(440, 980, 80, 40),
        new ellipseObj(530, 1000, 80, 80),
        new ellipseObj(140, 600, 100, 40, 0.125, -0.25, 400),
        new ellipseObj(740, 900, 100, 30),
        new ellipseObj(940, 850, 40, 20),
        new ellipseObj(740, 800, 100, 30),
        new ellipseObj(500, 720, 50, 25),
        new ellipseObj(380, 640, 50, 25)
    };
    levels[0] = new level(ellipseGenerator, 440, 940, 10, 1000, 0, 0, 1000, 190, 440, 10, 0);
    levels[levelIndex].startLevel();
}

void draw() {
    for (int i = 0; i < 8; i++) {
        playerObj.yMov += 0.03125;
        playerObj.xMov *= pow(0.9, 1 / 4.0f);
        if (controls[0])
            playerObj.xMov -= 1 / 16.0f;
        if (controls[1])
            playerObj.xMov += 1 / 16.0f;
        playerObj.y += playerObj.yMov / 25 * playerObj.r;
        playerObj.x += playerObj.xMov / 25 * playerObj.r;
        timeToJump--;
        for (int j = 0; j < ellipseObjs.length; j++) {
            ellipseObjs[j].moveObj();
            float beforeX = playerObj.x;
            if (playerObj.isColliding(ellipseObjs[j])) {
                if (playerObj.uncollide(ellipseObjs[j])) {
                    if (playerObj.y < ellipseObjs[j].y) {
                        playerObj.yMov = abs(playerObj.getDistRatio(ellipseObjs[j]));
                        timeToJump = 48;
                        playerObj.x = beforeX + ellipseObjs[j].movX;
                        playerObj.uncollide(ellipseObjs[j], playerObj.x);
                        playerObj.y += max(ellipseObjs[j].movY, 0);
                    }
                } else {
                    if (playerObj.x > ellipseObjs[j].x) playerObj.xMov = max(playerObj.xMov, 0);
                    else playerObj.xMov = min(playerObj.xMov, 0);
                }
            }
        }
        if (playerObj.x - playerObj.r < levels[levelIndex].minX) {
            playerObj.x = levels[levelIndex].minX + playerObj.r;
        }
        if (playerObj.x + playerObj.r > levels[levelIndex].maxY) {
            playerObj.x = levels[levelIndex].maxX - playerObj.r;
        }
        if (timeToJump > 0 && controls[2]) {
            playerObj.yMov = -3.5;
            timeToJump = 0;
        }
    }
    float camDistX = (((playerObj.x - scrollX)<0)?-1:1) * sqrt(abs(playerObj.x - scrollX)) * 2;
    float camDistY = (((playerObj.y - scrollY)<0)?-1:1) * sqrt(abs(playerObj.y - scrollY)) * 2;
    float deltaX = playerObj.x - scrollX;
    float deltaY = playerObj.y - scrollY;
    scrollX += camDistX;
    scrollY += camDistY;
    if ((deltaX > 0) != ((playerObj.x - scrollX) > 0)) scrollX = playerObj.x;
    if ((deltaY > 0) != ((playerObj.y - scrollY) > 0)) scrollY = playerObj.y;
    scrollX = min(levels[levelIndex].maxX - width/2, max(levels[levelIndex].minX + width/2, scrollX));
    scrollY = min(levels[levelIndex].maxY - height/2, max(levels[levelIndex].minY + height/2, scrollY));
    pushMatrix();
    noStroke();
    lights();
    translate(width/2-scrollX, height/2-scrollY);
    background(66, 176, 245);
    fill(246, 250, 235);
    for (int j = 0; j < ellipseObjs.length; j++) {
        pushMatrix();
        translate(ellipseObjs[j].x, ellipseObjs[j].y, 0);
        pushMatrix();
        scale(ellipseObjs[j].w-10, ellipseObjs[j].h-10, min(ellipseObjs[j].w, ellipseObjs[j].h)-10);
        sphere(1);
        popMatrix();
        for (int k = 0; k < levels[levelIndex].visualObjs[j].count; k++) {
            pushMatrix();
            translate(levels[levelIndex].visualObjs[j].xValues[k], levels[levelIndex].visualObjs[j].yValues[k], levels[levelIndex].visualObjs[j].zValues[k]);
            sphere(levels[levelIndex].visualObjs[j].radii[k]);
            popMatrix();
        }
        popMatrix();
    }
    fill(224, 4, 195);
    pushMatrix();
    translate(playerObj.x, playerObj.y, 0);
    sphere(playerObj.r);
    popMatrix();
    fill(224, 4, 195);
    pushMatrix();
    translate(levels[levelIndex].endX, levels[levelIndex].endY, 0);
    sphere(levels[levelIndex].endR);
    popMatrix();
    if (playerObj.isColliding(levels[levelIndex].endX, levels[levelIndex].endY, levels[levelIndex].endR)) {
      levelIndex = levels[levelIndex].exitTo;
      levels[levelIndex].startLevel();
    }
    if (playerObj.y - playerObj.r > levels[levelIndex].maxY) levels[levelIndex].startLevel();
    popMatrix();
}
