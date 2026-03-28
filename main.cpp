#include "raylib.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
//Vector2 mouse = GetMousePosition(); für die maus kordinate 

class Entity{};

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
Rectangle hitBox;
void draw()
    {
        DrawRectangleRec(hitBox, BLACK);
        DrawText(std::to_string(currentHp).c_str(), x, y, 20, GREEN);
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
        if(player.midlepoint.x > x){x+= basespeed*speedMulti;}
        if(player.midlepoint.x < x){x-= basespeed*speedMulti;}
        if(player.midlepoint.y > y){y+= basespeed*speedMulti;}
        if(player.midlepoint.y < y){y-= basespeed*speedMulti;}
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
    currenhealth = health*healthmulti;
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
    int wave = 1;
    float dificulty = 1;
    int enemyAmount;
    float spawnCooldown = 2;
    bool waveDefeat = false;
    bool specialWave = false;
    std::vector<Enemy> enemies;

    void setWaveLocation()
    {
        enemies.clear();
        srand(time(NULL));
        enemyAmount = wave + 3;
        for(int i=0; i < enemyAmount; i++)
        {
        Enemy enemy;
        enemy.spawn();
        enemies.push_back(enemy); 
        }
    }

    void spawnWave(Player & player)
    {   
        for(int i = 0; i < enemies.size(); i++)
        {
        enemies[i].move(player);
        enemies[i].updateHitBox();
        enemies[i].onHitPlayer(player);
        }
    }

    void drawWave()
    {
        for(int i = 0; i < enemies.size(); i++){enemies[i].draw();}
    }

    void apllyDificulty(Enemy & enemy)
    {
    enemy.dmgMulti = enemy.dmgMulti + dificulty*wave/10;
    enemy.healthmulti = enemy.healthmulti + dificulty*wave/10;
    if(enemy.speedMulti <= 6){enemy.speedMulti = enemy.speedMulti + wave/100;}
    }

    void drawStats(Enemy & enemy)
    {
        DrawRectangle(700,0,100,50,GRAY);
        DrawText(std::to_string(enemy.health).c_str(),720,0,20,BLACK);
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
        Projectile p;
        p.create(player);
        p.getAim();
    }






    void cd()
    {
    if(currentColdown > 0 ){currentColdown -= GetFrameTime();}
    }

    void hit(WaveManager & wave, Projectile & Projectile)
    {
        for(int i = 0; i < wave.enemies.size(); i++)
    {
    Projectile.enemyKontakt(wave.enemies[i]);
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
wave.setWaveLocation(); 
InitWindow(windowX,windowY,"Game");
SetTargetFPS(60);
while(!WindowShouldClose())
{
if(player.gameOver()){break;}
player.updateHitBox();
atack.hit(wave,Projectile);
wave.spawnWave(player);
BeginDrawing();
ClearBackground(RAYWHITE);
wave.drawWave();
player.draw();
Projectile.draw();
EndDrawing();
}
CloseWindow();
}


