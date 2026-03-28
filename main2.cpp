#include "raylib.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
//Vector2 mouse = GetMousePosition(); für die maus kordinate 
//Color meineFarbe = {255, 0, 0, 255}; // Rot funktion um farbe mit r+g+b+a(transparenz) zu erstellen

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


    bool isClicked(Rectangle r)
    {
        if(CheckCollisionRecs(r,mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){return true;}else{return false;}

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
    void exitButton()
    {
        Rectangle exit ={40,40,20,20};
        mausUpdate();if(isClicked(exit)){CloseWindow();}
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
    {0, 0},          // oben links
    {400, 0},        // oben mitte
    {800, 0},        // oben rechts

    {0, 225},        // mitte links
    {800, 225},      // mitte rechts

    {0, 450},        // unten links
    {400, 450},      // unten mitte
    {800, 450}       // unten rechts
    };

    if(!alive)
    {
    int index = rand() % 8;
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
    bool specialWave = false;
    std::vector<Enemy> enemies;

    void createWave()
    {
        if(waveDefeat == true)
        {
        wave++;
        enemies.clear();
        srand(time(NULL));
        enemyAmount = wave + 3;
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
            }
        }
    }

    void drawWave()
    {
        for(int i = 0; i < enemies.size(); i++){enemies[i].draw();}
        std::string waveText = std::to_string(wave);
        DrawText(waveText.c_str(), 440, 50, 30, BLACK);
        DrawText("Wave", 340, 50, 30, BLACK);
    }

    void waveEnded()
    {
        if(enemies.size() == 0){waveDefeat = true;}
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
        DrawText(std::to_string(cHealth).c_str(),710,480,10,BLACK);DrawText("EnemyHP :",630,480,11,BLACK);
        DrawText(std::to_string(cDmg).c_str(),710,500,10,BLACK);DrawText("EnemyDMG :",630,500,11,BLACK);
        DrawText(std::to_string(cSpeed).c_str(),710,520,10,BLACK);DrawText("EnemySpeed :",630,520,11,BLACK);
    }

};
class AtackManager
{
 public:
    float baseAtackColdown = 1;
    float coldownMulti = 1;
    float currentColdown = baseAtackColdown*coldownMulti;
    float baseDmg = 10;
    float dmgMulti = 1;
    std::vector<Projectile> projectiles;

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
    int baseDmg =1;
    int baseHp =1;
    int baseArmor =1;
    int baseReload =1;
    Rectangle dmgUpgrade;
    Rectangle hpUpgrade;
    Rectangle armorUpgrade;
    Rectangle rateUpgrade;
    

    void shopButtons(AtackManager & atack,Player & player)
    {
     mausUpdate();
     dmgUpgrade = {60,480,80,50};
     if(isClicked(dmgUpgrade)&& gold >= 1){atack.baseDmg += 1;gold-=1;}
     hpUpgrade = {180,480,80,50};
     armorUpgrade = {320,480,80,50};
     rateUpgrade = {440,480,80,50};
    }

    void drawshop()
    {
    drawBoxWithText(0,450,800,100,460,GRAY,"SHOP",20,BLACK);
    DrawText("gold",60,455,15,BLACK);DrawText(std::to_string(gold).c_str(),100,455,15,BLACK);
    drawBoxWithText(dmgUpgrade.x,dmgUpgrade.y,dmgUpgrade.width,dmgUpgrade.height,dmgUpgrade.y+15,DARKGRAY,"DMG+",20,BLACK);
    drawBoxWithText(hpUpgrade.x,hpUpgrade.y,hpUpgrade.width,hpUpgrade.height,hpUpgrade.y+15,DARKGRAY,"HP+",20,BLACK);
    drawBoxWithText(armorUpgrade.x,armorUpgrade.y,armorUpgrade.width,armorUpgrade.height,armorUpgrade.y+15,DARKGRAY,"Armor+",20,BLACK);
    drawBoxWithText(rateUpgrade.x,rateUpgrade.y,rateUpgrade.width,rateUpgrade.height,rateUpgrade.y+15,DARKGRAY,"Rate+",20,BLACK);
    }

};

int main()
{
srand(time(NULL));
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
SetTargetFPS(60);
while(!WindowShouldClose())
{
shop.shopButtons(atack,player);
ui.exitButton();
uiE.mausUpdate();
wave.apllyDificulty();
wave.createWave();
if(player.gameOver()){break;}
atack.cd();
atack.createProjectile(player);
atack.updateProjectile(wave);
player.updateHitBox();
wave.spawnWave(player);
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
}
CloseWindow();
}


