#include "raylib.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
//Vector2 mouse = GetMousePosition(); für die maus kordinate 
//Color meineFarbe = {255, 0, 0, 255}; // Rot funktion um farbe mit r+g+b+a(transparenz) zu erstellen

std::string f2(float value)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return stream.str();
}

Sound click = LoadSound("Minimalist11.ogg");

int losses = 2;

class Entity{};
class StatShop;
class UIElements
{
    public:
    Rectangle mouse;

    void mausUpdate()
    {
        Vector2 mouseL = GetMousePosition();
        float x = mouseL.x;
        float y = mouseL.y;
        mouse = {x,y,1,1};
    }

    void drawBoxWithText(int x, int y, int w, int h,int yText, Color boxColor, const std::string& text, int textSize, Color textColor)
    {
    DrawRectangle(x, y, w, h, boxColor);
    int textWidth = MeasureText(text.c_str(), textSize);
    int textX = x + (w - textWidth) / 2;
    DrawText(text.c_str(), textX , yText, textSize, textColor);
    }
     void drawBoxWithTextRec(Rectangle r,int yText, Color boxColor, const std::string& text, int textSize, Color textColor)
    {
    float x = r.x;
    float y = r.y;
    float w = r.width;
    float h = r.height;
    DrawRectangle(x, y, w, h, boxColor);
    int textWidth = MeasureText(text.c_str(), textSize);
    int textX = x + (w - textWidth) / 2;
    DrawText(text.c_str(), textX , yText, textSize, textColor);
    }


    bool isClicked(Rectangle r)
    {
        if(CheckCollisionRecs(r,mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){PlaySound(click);;return true;}else{return false;}

    }

};

class Player : public Entity
{
public:
bool alive = true;
float x, y;
Vector2 midlepoint;
float size = 40;
float basehp = 10000;
float baseHpRegen= 5;
float hpMulti = 1;
float armor = 0;
float hpRegenMulti = 1;
float currentHp = basehp*hpMulti;
UIElements ui;
Rectangle hitBox;
 
void resetPlayer(int windowX, int windowY)
    {
        alive = true;
        x = windowX / 2 - size / 2;
        y = windowY / 2 - size / 2;

        basehp = 10000;
        baseHpRegen = 5;
        hpMulti = 1;
        armor = 0;
        hpRegenMulti = 1;
        currentHp = basehp * hpMulti;

        midlepoint = {x + size / 2, y + size / 2};
        hitBox = {x, y, size, size};
    }

void draw()
    {
        DrawRectangleRec(hitBox, BLACK);
        ui.drawBoxWithText(400,420,100,30,430,RED,std::to_string(currentHp).c_str(),10,BLACK);
    }
    void updateHitBox()
    {
   if(alive)
    {
    midlepoint.x = x+size/2;
    midlepoint.y = y+size/2;
    hitBox = {x, y, size, size};
    }
    }


bool gameOver()
{
    return currentHp <=0;
}


};
class Enemy : public Entity
{
public:
    float basespeed = 0.5;
    float speedMulti = 1;   
    float health = 10;
    float x,y;
    float size = 20;
    float dmg = 10;
    float dmgMulti = 1;
    float armor = 0;
    float healthmulti = 1;
    float atackcooldown = 0.5f;
    float currenhealth;
    float cSpeed;
    bool alive = false;
    Rectangle hitBox;
    Vector2 mitte;

    void updateHitBox()
    {
     if(alive){hitBox = {x,y,size,size};}
    }
    void overlapedX()
    {
        x =mitte.x-size/2;
    }
    void overlapedY()
    {
        y =mitte.y-size/2;
    }

    void onHitPlayer(Player & player)
    {
        if(alive)
        {
            atackcooldown -= GetFrameTime();
            if(atackcooldown <= 0)
            {
            if(CheckCollisionRecs(hitBox,player.hitBox))
            {
            float damage = (dmg * dmgMulti)-player.armor;
            if(damage < 0){damage = 0;}
            player.currentHp -= damage;
            atackcooldown = 0.5f;
            }
            }
        }
    }

   void move(Player & player)
{
    if(alive)
    {
        if(!CheckCollisionRecs(hitBox, player.hitBox))
        {
            float enemyMiddleX = x + size / 2;
            float enemyMiddleY = y + size / 2;

            float directionX = player.midlepoint.x - enemyMiddleX;
            float directionY = player.midlepoint.y - enemyMiddleY;

            float length = sqrt(directionX * directionX + directionY * directionY);

            if(length != 0)
            {
                directionX /= length;
                directionY /= length;

                x += directionX * cSpeed;
                y += directionY * cSpeed;
            }
        }
    }
}

    void draw()
    {
        if(alive)
        {
             DrawRectangleRec(hitBox, RED);
        }
    }

    void spawn()
    {
        std::vector<Vector2> spawns =
    {
   // OBEN (y = 0)
    {160, 0},
    {320, 0},
    {480, 0},
    {640, 0},

    // UNTEN (y = 450)
    {160, 450},
    {320, 450},
    {480, 450},
    {640, 450},

    // LINKS (x = 0)
    {0, 90},
    {0, 180},
    {0, 270},
    {0, 360},

    // RECHTS (x = 800)
    {800, 90},
    {800, 180},
    {800, 270},
    {800, 360}
    };

    if(!alive)
    {
    int index = rand() % 16;
    Vector2 spawnPos = spawns[index];
    x = spawnPos.x;
    y = spawnPos.y;
    alive = true;
    }

    }

    void checkAlive()
    {
        if(currenhealth <= 0)
        {
        alive = false;
        
        }
    }

};
class Projectile : public Entity
{
public:
float x, y;
Vector2 target;
float size = 10;
float basespeed = 2;
float speedMulti = 1;
float baseDmg = 10;
float dmgMulti = 1;
float cDmg;
Rectangle hitBox;
Vector2 mouse;
bool hasAimed = false;
bool alive = false;
Vector2 direction;
UIElements ui;

void create(Player player)
{
    if(!alive)
    {
    alive = true;
    x = player.x + player.size/2 -size/2;
    y = player.y + player.size/2 -size/2;
    }
}
void draw()
    {
        if(alive){DrawRectangle(x,y,size,size, BLACK);}
    }
    void updateHitBox()
    {
        if(alive)
        {
            hitBox = {x, y, size, size};
        }
    }
    void getAim()
    {
        if(alive)
        {
        if(!hasAimed)
        {
            cDmg = baseDmg*dmgMulti;
            target.x = mouse.x;
            target.y = mouse.y;
            
            direction.x = target.x - x;
            direction.y = target.y - y;
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);

            if(length != 0)
            {
            direction.x /= length;
            direction.y /= length;
            }

            hasAimed =true;
        }
        }
    }
    void move()
    {
    {
    if(alive)
    {

        x += direction.x * basespeed * speedMulti;
        y += direction.y * basespeed * speedMulti;
    }
    }
    }

    void destoyOnBorder(int borderX, int borderY)
    {
        if (x <= 0 || y <= 0 || x >= borderX || y >= borderY)
    {
        alive = false;
        hasAimed = false;
    }
    }

    void enemyKontakt(Enemy & enemy)
    {
        if(CheckCollisionRecs(enemy.hitBox,hitBox))
        {
            enemy.currenhealth -= cDmg;
            alive = false;
        }
    }
};
class WaveManager
{
    public:
    int wave = 0;
    float dificulty = 1;
    int enemyAmount;
    float spawnCooldown = 0.5f;
    float spawnAmount = 1;
    float basehealth = 10;
    float healthmulti = 1;
    float basedmg = 10;
    float dmgMulti = 1;
    float size = 20;
    float basespeed = 0.5;
    float speedMulti = 1;   
    float cHealth = basehealth*healthmulti;
    float cDmg = basedmg*dmgMulti;
    float cSpeed = basespeed*speedMulti;
    bool waveDefeat = true;
    bool bossWave = false;
    int bossCounter = 1;
    float bosshp;
    bool addgold = false;
    UIElements ui;
    std::vector<Enemy> enemies;

void resetProgress()
    {
        wave = 0;
        dificulty = 1;
        enemyAmount = 0;
        spawnCooldown = 0.5f;
        spawnAmount = 1;

        basehealth = 10;
        healthmulti = 1;
        basedmg = 10;
        dmgMulti = 1;
        size = 20;
        basespeed = 0.5f;
        speedMulti = 1;

        cHealth = basehealth * healthmulti;
        cDmg = basedmg * dmgMulti;
        cSpeed = basespeed * speedMulti;

        waveDefeat = true;
        bossWave = false;
        bossCounter = 10;
        bosshp = 0;
        addgold = false;

        enemies.clear();
    }




    void createWave()
    {
        if(waveDefeat == true)
        {
        wave++;
        bossCounter++;
        if(bossCounter >=10){bossWave =true;}
        enemies.clear();
        srand(time(NULL));
        enemyAmount = wave + 3;
         if(bossWave)
        {
        Enemy boss;
        boss.currenhealth = cHealth*20;
        boss.health = cHealth*20;
        boss.dmg = cDmg*5;
        boss.cSpeed = cSpeed/2;
        boss.size = boss.size*3;
        boss.spawn();
        enemies.push_back(boss);
        bosshp = boss.currenhealth;
        bossCounter = 0;
        }
        for(int i=0; i < enemyAmount; i++)
        {
        Enemy enemy;
        enemy.currenhealth = cHealth;
        enemy.dmg = cDmg;
        enemy.cSpeed = cSpeed;
        enemy.spawn();
        enemies.push_back(enemy); 
        waveDefeat = false;
        }
        }
    }

    void checkEnemyOverlap()
    {
        for(int i = 0; i < enemies.size(); i++)
        {
        for(int j = i + 1; j < enemies.size(); j++)
        {
            if(i == j){continue;}
            bool overlaped = false;
            enemies[i].mitte.x = enemies[i].x + enemies[i].size/2;  //warum nutze ich nicht einfach ki?
            enemies[j].mitte.x = enemies[j].x + enemies[j].size/2;  //was ist dieses stück von code was ich grade bitte fabrizier 
            enemies[i].mitte.y = enemies[i].y + enemies[i].size/2;  //das ist unlesbar ich werde sobalt es klappt es nie wieder anfassen 
            enemies[j].mitte.y = enemies[j].y + enemies[j].size/2;
            Vector2 a; a.x = enemies[i].x+enemies[i].size/2; a.y = enemies[i].y+enemies[i].size/2; //ich habe hier mittelpunkte gemacht wollte aber kein langen variablen nammen deswegen a und b
            Vector2 b; b.x = enemies[j].x+enemies[j].size/2; b.y = enemies[j].y+enemies[j].size/2;
            if((int)a.x == (int)b.x && (int)a.y == (int)b.y){enemies[i].mitte.x += enemies[i].size*2;enemies[i].mitte.y += enemies[i].size*2;overlaped = true;}
            if(a.x < b.x && CheckCollisionRecs(enemies[i].hitBox,enemies[j].hitBox)){enemies[i].mitte.x =a.x - enemies[i].cSpeed;overlaped = true;}
            if(a.x > b.x && CheckCollisionRecs(enemies[i].hitBox,enemies[j].hitBox)){enemies[i].mitte.x =a.x + enemies[i].cSpeed;overlaped = true;}
            //ich update alle standorte damit nicht doppel vershoben wird geht wahrscheinlich effizienter
            if(overlaped){enemies[i].overlapedX();enemies[i].updateHitBox();enemies[j].updateHitBox();}
            overlaped = false;
            a.x = enemies[i].mitte.x; a.y = enemies[i].mitte.y;
            b.x = enemies[j].mitte.x; b.y = enemies[j].mitte.y;
            if(a.y < b.y && CheckCollisionRecs(enemies[i].hitBox,enemies[j].hitBox)){enemies[i].mitte.y =a.y - enemies[i].cSpeed;overlaped = true;}
            if(a.y > b.y && CheckCollisionRecs(enemies[i].hitBox,enemies[j].hitBox)){enemies[i].mitte.y =a.y + enemies[i].cSpeed;overlaped = true;}
            if(overlaped){enemies[i].overlapedY();enemies[i].updateHitBox();enemies[j].updateHitBox();}//ich sitze hier shon fast 3 stunden drann ich habe in 3 stunden den wavemanger und atackmanger geschrieben diese funktion raubt mir meine geistliche gesündheit so stark das ich soger mit kommentaren anfange  
        }
        }
    }

    void spawnWave(Player & player)
    {   
        for(int i = enemies.size() - 1; i >= 0; i--)
        {
        enemies[i].move(player);
        enemies[i].updateHitBox();
        enemies[i].onHitPlayer(player);
        enemies[i].checkAlive();
        if(!enemies[i].alive)
            {
            enemies.erase(enemies.begin() + i);
            addgold = true;
            }
        }
    }

    void drawWave()
    {
        for(int i = 0; i < enemies.size(); i++){enemies[i].draw();}
        if(enemies[0].health > cHealth){bosshp = enemies[0].currenhealth;}else{bossWave =false;}
        std::string waveText = std::to_string(wave);
        DrawText(waveText.c_str(), 440, 50, 30, BLACK);
        DrawText("Wave", 340, 50, 30, BLACK);
        if(bossWave){ui.drawBoxWithText(320,90,140,30,95,GREEN,f2(bosshp).c_str(),20,BLACK);}
    }

    void waveEnded()
    {
        if(enemies.size() == 0){waveDefeat = true;if(bossWave){bossWave=false;};}
    }

    void apllyDificulty()
    {
    if(waveDefeat==true)
    {
    dmgMulti = dmgMulti + dificulty*wave/10;
    healthmulti = healthmulti + dificulty*wave/10;
    if(speedMulti <= 6){speedMulti = speedMulti + wave/100;}
    cHealth = basehealth*healthmulti;
    cDmg = basedmg*dmgMulti;
    cSpeed = basespeed*speedMulti;
    }
    }

    void drawStats()
    {
        DrawRectangle(660,450,100,100,GRAY);
        DrawText(f2(cHealth).c_str(),710,480,10,BLACK);DrawText("EnemyHP :",630,480,11,BLACK);
        DrawText(f2(cDmg).c_str(),710,500,10,BLACK);DrawText("EnemyDMG :",630,500,11,BLACK);
        DrawText(f2(cSpeed).c_str(),710,520,10,BLACK);DrawText("EnemySpeed :",630,520,11,BLACK);
    }

};
class AtackManager
{
 public:
    float baseAtackColdown = 0.5;
    float coldownMulti = 2;
    float currentColdown = baseAtackColdown*coldownMulti;
    float currentColdownREF = baseAtackColdown*coldownMulti;
    float baseDmg = 10;
    float dmgMulti = 1;
    std::vector<Projectile> projectiles;



    void resetAtack()
    {
        baseAtackColdown = 0.5f;
        coldownMulti = 2.0f;
        baseDmg = 10;
        dmgMulti = 1;

        currentColdown = baseAtackColdown * coldownMulti;
        currentColdownREF = baseAtackColdown * coldownMulti;

        projectiles.clear();
    }

    void createProjectile(Player & player)
    {
       if(currentColdown <= 0)
       {
        Projectile p;
        p.baseDmg = baseDmg;
        p.dmgMulti = dmgMulti;
        p.mouse = GetMousePosition();
        p.create(player);
        p.getAim();
        projectiles.push_back(p);
        currentColdown = baseAtackColdown*coldownMulti;
        currentColdownREF = baseAtackColdown*coldownMulti;
       }
    }

    void updateProjectile(WaveManager & wave)
    {
        for(int i = projectiles.size() - 1; i >= 0; i--)
        {
            projectiles[i].move();
            projectiles[i].updateHitBox();
            for(int j = 0; j < wave.enemies.size(); j++)
            {
            projectiles[i].enemyKontakt(wave.enemies[j]);
            }
            projectiles[i].destoyOnBorder(800,450);
            if(!projectiles[i].alive)
            {
            projectiles.erase(projectiles.begin() + i);
            }
        }
    }

    void drawProjectiles()
    {
        for(int i = projectiles.size() - 1; i >= 0; i--)
        {
            projectiles[i].draw();
        }
    }




    void cd()
    {
    if(currentColdown > 0 ){currentColdown -= GetFrameTime();}
    }


};

class skillTree : UIElements
{
    public:
    int scrap = 0;
    int scrapGain = 1;





};  

class Items
{
    public:
    std::string name;
    std::string priceSTR;
    int price;
    std::string rarity;
    float rawDmgUp = 0;
    float rawHpUp = 0;
    float rawArmorUp = 0;
    float rawRateUp = 0;
    float rawShotSpeedUp = 0;
    float dmgPercentUp = 0;
    float hpPercentUp = 0;
    float ratePercentUp = 0;


};

class MainMenu : UIElements
{
    public:
    bool gameStarted = false;
    float buttonWidth = 200;
    float buttonHeight = 70;
    float buttonsX = (GetScreenWidth()/2)-buttonWidth/2;
    int buttonOffsetY = 130;
    float buttonsY =  200;                                  //(GetScreenHeight()/2)+buttonOffsetY;
    Rectangle playButton;
    Rectangle skillTreeButton;

    void menuButtons()
    {
        buttonsX = (GetScreenWidth()/2)-buttonWidth/2;
        buttonsY = (GetScreenHeight()/2)-buttonOffsetY;
        playButton = {buttonsX,buttonsY,buttonWidth,buttonHeight};mausUpdate();
        if(losses >=1){skillTreeButton = {buttonsX,buttonsY+100,buttonWidth,buttonHeight};mausUpdate();}
        if(isClicked(playButton)){gameStarted = true;}
    }


    void drawMenu()
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Defend A Cube",playButton.x-120,playButton.y-100,60,BLACK);
        drawBoxWithTextRec(playButton,playButton.y+22,LIGHTGRAY,"PLAY",30,BLACK);
        if(losses >=1){drawBoxWithTextRec(skillTreeButton,skillTreeButton.y+22,LIGHTGRAY,"SKILL-TREE",30,BLACK);}
        EndDrawing();
    }

};

class UI : UIElements
{
    public:


    void drawUI()
    {
        DrawRectangle(0,0,40,550,DARKGRAY);
        DrawRectangle(0,0,800,40,DARKGRAY);
        DrawRectangle(760,0,40,550,DARKGRAY);
        drawBoxWithText(40,40,20,20,50,RED,"X",10,BLACK); //close game button zum testen meiner click funktion 
    }
    void exitButton(MainMenu & menu)
    {
        Rectangle exit ={40,40,20,20};
        mausUpdate();if(isClicked(exit)){menu.gameStarted = false;}
    }


};

class  ShopNotUsed : UIElements //sollte der shop werden ich lasse es im programm falls ich wieder lusst auf sowas wie items kriege maybe late game shop oder so
{
    public:
    int gold = 0;
    bool isShopOpen = false;
    std::vector<Items> items;



    void createNewItemDefense(std::string nameN, std::string rarityN, float hpUp, float percenHp, float armorUp, float priceN)
    {   
        Items n;

        n.name = nameN;
        n.rarity = rarityN;
        n.rawHpUp = hpUp;
        n.hpPercentUp = percenHp;
        n.rawArmorUp = armorUp;
        n.price = priceN;
        items.push_back(n);
    }
    void createNewItemAtack(std::string nameN, std::string rarityN, float dmgUp, float percendmg, float rateUp, float priceN)
    {   
        Items n;

        n.name = nameN;
        n.rarity = rarityN;
        n.rawDmgUp = dmgUp;
        n.dmgPercentUp = percendmg;
        n.rawRateUp = rateUp;
        n.price = priceN;
        items.push_back(n);
    }

    
    void createShop(int X,int Y,int sizeX,int sizeY)
    {
        if(isShopOpen)
        {
         for(int i = 0;int d= 0;i<items.size())
         {
            DrawRectangle(X+d,Y,sizeX,sizeY,BLACK);
            d+=20;
         }
        }
    }

};

class StatShop : UIElements
{
    public:
    int gold = 1;
    int goldgain=1;
    int baseDmg =1;
    int baseHp =1;
    int baseArmor =1;
    int baseReload =1;
    int startGold = 10;
    int startGoldGain=1;
    int startBaseDmg =1;
    int startBaseHp =1;
    int startBaseArmor =1;
    int startBaseReload =1;
    int shopMulti = 1;
    float cArmor;
    float cDmg;
    float cHP;
    float cRate;
    Rectangle dmgUpgrade;
    Rectangle hpUpgrade;
    Rectangle armorUpgrade;
    Rectangle rateUpgrade;
    Rectangle oneX;
    Rectangle fiveX;
    Rectangle tenX;

    void resetShop()
    {
        gold = startGold;
        goldgain = startGoldGain;
        baseDmg = startBaseDmg;
        baseHp = startBaseHp;
        baseArmor = startBaseArmor;
        baseReload = startBaseReload;

        cArmor = 0;
        cDmg = 0;
        cHP = 0;
        cRate = 0;

        dmgUpgrade = {0,0,0,0};
        hpUpgrade = {0,0,0,0};
        armorUpgrade = {0,0,0,0};
        rateUpgrade = {0,0,0,0};
    }


    void updateGold(WaveManager & wave)
    {
        if(wave.addgold)
        {
            gold +=goldgain;
            wave.addgold = false;
        }
    }

    void shopButtons(AtackManager & atack,Player & player)
    {
     mausUpdate();
     oneX = {180,455,15,20};
     if(isClicked(oneX)){shopMulti = 1;}
     fiveX = {210,455,15,20};
     if(isClicked(fiveX)){shopMulti = 5;}
     tenX = {240,455,15,20};
     if(isClicked(tenX)){shopMulti = 10;}
     dmgUpgrade = {60,480,80,50};
     if(isClicked(dmgUpgrade)&& gold >= 1*shopMulti){atack.baseDmg += 1*shopMulti;gold-=1*shopMulti;}
     hpUpgrade = {180,480,80,50};
     if(isClicked(hpUpgrade)&& gold >= 1*shopMulti){player.basehp += 1*shopMulti;gold-=1*shopMulti;}
     armorUpgrade = {320,480,80,50};
     if(isClicked(armorUpgrade)&& gold >= 1*shopMulti){player.armor += 1*shopMulti;gold-=1*shopMulti;}
     rateUpgrade = {440,480,80,50};
     if(isClicked(rateUpgrade)&& gold >= 1*shopMulti){atack.coldownMulti -= 0.1*shopMulti;gold-=1*shopMulti;}
    cArmor = player.armor;
    cDmg = atack.baseDmg;
    cHP = player.basehp;
    cRate = atack.currentColdownREF;
    }

    void drawshop()
    {
    drawBoxWithText(0,450,800,100,460,GRAY,"SHOP",20,BLACK);
    drawBoxWithTextRec(oneX,oneX.y+3,DARKGRAY,"1X",12,BLACK);
    drawBoxWithTextRec(fiveX,fiveX.y+3,DARKGRAY,"5X",12,BLACK);
    drawBoxWithTextRec(tenX,tenX.y+3,DARKGRAY,"10X",12,BLACK);
    DrawText("gold",60,455,15,BLACK);DrawText(std::to_string(gold).c_str(),100,455,15,BLACK);
    drawBoxWithText(dmgUpgrade.x,dmgUpgrade.y,dmgUpgrade.width,dmgUpgrade.height,dmgUpgrade.y+15,DARKGRAY,"DMG+",20,BLACK);
    drawBoxWithText(hpUpgrade.x,hpUpgrade.y,hpUpgrade.width,hpUpgrade.height,hpUpgrade.y+15,DARKGRAY,"HP+",20,BLACK);
    drawBoxWithText(armorUpgrade.x,armorUpgrade.y,armorUpgrade.width,armorUpgrade.height,armorUpgrade.y+15,DARKGRAY,"Armor+",20,BLACK);
    drawBoxWithText(rateUpgrade.x,rateUpgrade.y,rateUpgrade.width,rateUpgrade.height,rateUpgrade.y+15,DARKGRAY,"Rate+",20,BLACK);
    DrawText(f2(cDmg).c_str(),dmgUpgrade.x,dmgUpgrade.y+40,10,BLACK);
    DrawText(f2(cHP).c_str(),hpUpgrade.x,hpUpgrade.y+40,10,BLACK);
    DrawText(f2(cArmor).c_str(),armorUpgrade.x,armorUpgrade.y+40,10,BLACK);
    DrawText(f2(cRate).c_str(),rateUpgrade.x,rateUpgrade.y+40,10,BLACK);
    }

};


int main()
{
srand(time(NULL));
MainMenu menu;
Player player;
Projectile Projectile; 
Enemy enemy;
WaveManager wave;
AtackManager atack;
StatShop shop;
UI ui;
UIElements uiE;
int windowX = 800;
int windowY = 550;
player.x = windowX / 2 - player.size / 2;
player.y = windowY / 2 - player.size / 2;
InitWindow(windowX,windowY,"Game");
InitAudioDevice();
click = LoadSound("Minimalist11.ogg");
Music music = LoadMusicStream("Gymnopédie No.1.ogg");
PlayMusicStream(music);
SetTargetFPS(60);
while(!WindowShouldClose())
{
UpdateMusicStream(music);
if(!menu.gameStarted)
{menu.menuButtons();menu.drawMenu();player.resetPlayer(windowX,windowY);atack.resetAtack();wave.resetProgress();shop.resetShop();}else{
shop.shopButtons(atack,player);
ui.exitButton(menu);
uiE.mausUpdate();
wave.apllyDificulty();
wave.createWave();
if(player.gameOver()){menu.gameStarted = false;}
atack.cd();
atack.createProjectile(player);
atack.updateProjectile(wave);
player.updateHitBox();
wave.spawnWave(player);
shop.updateGold(wave);
wave.checkEnemyOverlap();
wave.waveEnded();
BeginDrawing();
ClearBackground(RAYWHITE);
wave.drawWave();
player.draw();
atack.drawProjectiles();
shop.drawshop();
ui.drawUI();
wave.drawStats();
EndDrawing();
}}
CloseWindow();
}


