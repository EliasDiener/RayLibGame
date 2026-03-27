#include "raylib.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
//Vector2 mouse = GetMousePosition(); für die maus kordinate 

class Entity{};

class UI
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

    void drawBoxWithText(int x, int y, int w, int h, Color boxColor, const std::string& text, int textSize, Color textColor)
    {
    DrawRectangle(x, y, w, h, boxColor);
    int textWidth = MeasureText(text.c_str(), textSize);
    int textX = x + (w - textWidth) / 2;
    DrawText(text.c_str(), textX , y + 10, textSize, textColor);
    }


    bool isClicked(Rectangle r)
    {
        if(CheckCollisionRecs(r,mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){return true;}

    }

};

class Player : public Entity
{
public:
bool alive = true;
float x, y;
Vector2 midlepoint;
float size = 40;
float basehp = 100;
float baseHpRegen= 5;
float hpMulti = 1;
float armor = 0;
float hpRegenMulti = 1;
float currentHp = basehp*hpMulti;
UI ui;
Rectangle hitBox;
void draw()
    {
        DrawRectangleRec(hitBox, BLACK);
        ui.drawBoxWithText(400,420,100,30,RED,std::to_string(currentHp).c_str(),10,BLACK);
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

    void updateHitBox()
    {
     if(alive){hitBox = {x,y,size,size};}
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
        if(!CheckCollisionRecs(hitBox,player.hitBox))
        {
        if(player.midlepoint.x > x){x+= cSpeed;}
        if(player.midlepoint.x < x){x-= cSpeed;}
        if(player.midlepoint.y > y){y+= cSpeed;}
        if(player.midlepoint.y < y){y-= cSpeed;}
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
Rectangle hitBox;
Vector2 mouse;
bool hasAimed = false;
bool alive = false;
Vector2 direction;

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
            enemy.currenhealth -= baseDmg;
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
        DrawRectangle(700,0,100,50,GRAY);
        DrawText(std::to_string(cHealth).c_str(),720,0,20,BLACK);
    }

};
class AtackManager
{
 public:
    float baseAtackColdown = 1;
    float coldownMulti = 1;
    float currentColdown = baseAtackColdown*coldownMulti;
    std::vector<Projectile> projectiles;

    void createProjectile(Player & player)
    {
       if(currentColdown <= 0)
       {
        Projectile p;
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

class  Shop : UI
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



int main()
{
srand(time(NULL));
Player player;
Projectile Projectile; 
Enemy enemy;
WaveManager wave;
AtackManager atack;
int windowX = 800;
int windowY = 450;
player.x = windowX / 2 - player.size / 2;
player.y = windowY / 2 - player.size / 2;
InitWindow(windowX,windowY,"Game");
SetTargetFPS(60);
while(!WindowShouldClose())
{
wave.apllyDificulty();
wave.createWave();
if(player.gameOver()){break;}
atack.cd();
atack.createProjectile(player);
atack.updateProjectile(wave);
player.updateHitBox();
wave.spawnWave(player);
wave.waveEnded();
BeginDrawing();
ClearBackground(RAYWHITE);
wave.drawWave();
player.draw();
atack.drawProjectiles();
wave.drawStats();
EndDrawing();
}
CloseWindow();
}


