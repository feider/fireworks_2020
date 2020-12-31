#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <algorithm>

void main_loop();

class Globals;
class Rocket;

class Rocket;
class Particle;
class Fragment;

class RocketFactory;

std::vector<Particle*> particles;
std::vector<Rocket*> rockets;
std::vector<Fragment*> fragments;

class Globals
{
public:

    bool end;
    int iter;

    SDL_Renderer * r;
    SDL_Window * w;
    const Uint8 * k;

    int width, height;

    unsigned int ticks_begin;
    unsigned int last_ticks;
    unsigned int ticks_passed;
    double time_mult;

    std::vector<Rocket*> rockets;



    void update_ticks()
    {
        auto t = SDL_GetTicks();
        ticks_passed = t-last_ticks;
        last_ticks = t;
        time_mult = (double) ticks_passed / 1000.0;
    }

};

Globals g;

class Particle
{
public:
    int x, y;
    int red, green, blue;
    unsigned int solid;
    unsigned int decay;
    bool dead;

    Particle(int x, int y, int red, int green, int blue, unsigned int solid, unsigned int decay)
    {
        this->x = x;
        this->y = y;
        this->red=red;
        this->green=green;
        this->blue=blue;
        this->solid = solid+g.last_ticks;
        this->decay=this->solid+decay;
        this->dead=false;
    }



    void update_and_render()
    {
        unsigned int ticks = g.last_ticks;

        if (ticks < solid)
        {
            SDL_SetRenderDrawColor(g.r, red, green, blue, 0);
            SDL_RenderDrawPoint(g.r, x, y);
        }

        else if (ticks < decay)
        {
            float perc = 1.0- ((float) (ticks-solid) / (float) (decay-solid));
            int red = this->red*perc;
            int blue = this->blue*perc;
            int green = this->green*perc;
            SDL_SetRenderDrawColor(g.r, red, green, blue, 0);
            SDL_RenderDrawPoint(g.r, x, y);
        }
        else
        {
            this->dead = true;
        }

    }

};

class Rocket
{
public:
    double x, y, sx, sy, boom;
    bool dead;
    virtual void update() {}
};

class Fragment
{
public:
    double x, y, sx, sy;
    bool dead;
    virtual void update() {}
};

class StandardFragment: public Fragment
{

public:
    int red, green, blue, gone;
    StandardFragment(double x, double y, double sx, double sy, int red, int green, int blue, unsigned int gone)
    {
        this->x=x;
        this->y=y;
        this->sx=sx;
        this->sy=sy;
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->gone = gone+g.last_ticks;
        this->dead = false;
    }

    void update()
    {
        x += sx*g.time_mult;
        y += sy*g.time_mult;
        particles.push_back(new Particle(x, y, red, green, blue, 100, 1000));
        if(g.last_ticks> gone)
            this->dead = true;
    }
};

class SuperFancyFragment: public Fragment
{
public:
    int red, green, blue, gone;
    SuperFancyFragment(double x, double y, double sx, double sy, unsigned int gone)
    {
        this->x=x;
        this->y=y;
        this->sx=sx;
        this->sy=sy;
        this->gone = gone+g.last_ticks;
        this->dead = false;
    }

    void update()
    {
        x += sx*g.time_mult;
        y += sy*g.time_mult;
        particles.push_back(new Particle(x, y, 255, 255, 100, 100, 100));
        if(g.last_ticks> gone)
            this->dead = true;
    }
};

class FancyFragment: public Fragment
{
public:
    int red, green, blue, gone;
    FancyFragment(double x, double y, double sx, double sy, unsigned int gone)
    {
        this->x=x;
        this->y=y;
        this->sx=sx;
        this->sy=sy;
        this->gone = gone+g.last_ticks;
        this->dead = false;
    }

    void update()
    {
        x += sx*g.time_mult;
        y += sy*g.time_mult;
        particles.push_back(new Particle(x, y, 30, 30, 20, 100, 100));
        if(g.last_ticks> gone)
        {
            this->dead = true;

            int speed = rand()%20+50;
            int time = rand()%500+500;
            int red, green, blue, col;
            col = rand() % 3;
            for(int i = 0; i<10; i++)
            {
                int deg = rand() % 360;
                double rad = (double) deg * 3.14159 / 180.0;
                double sx = sin(rad) * (double) speed;
                double sy = cos(rad) * (double) speed;
                fragments.push_back(new SuperFancyFragment(x, y, sx, sy, time));
            }
        }
    }
};

class StandardRocket: public Rocket
{
public:
    StandardRocket(double x, double y, double sx, double sy, unsigned int boomtime)
    {
        this->x=x;
        this->y=y;
        this->sx=sx;
        this->sy=sy;
        this->boom = boomtime+g.last_ticks;
        this->dead = false;
    }

    void update()
    {
        x += sx*g.time_mult;
        y += sy*g.time_mult;
        particles.push_back(new Particle(x, y, 100, 100, 100, 1, 1000));
        if(g.last_ticks> boom)
        {
            int speed = rand()%20+50;
            int time = rand()%500+2000;
            int red, green, blue, col;
            col = rand() % 3;
            for(int i = 0; i<50; i++)
            {
                int deg = rand() % 360;
                double rad = (double) deg * 3.14159 / 180.0;
                double sx = sin(rad) * (double) speed;
                double sy = cos(rad) * (double) speed;

                switch(col)
                {
                case 0:
                    red = 255;
                    green = 0;
                    blue = 0;
                    break;
                case 1:
                    red = 0;
                    green = 255;
                    blue = 0;
                    break;
                case 2:
                    red = 0;
                    green = 0;
                    blue = 255;
                    break;
                }


                fragments.push_back(new StandardFragment(x, y, sx, sy, red, green, blue, time));
            }
            this->dead = true;
        }
    }
};

class RainbowRocket: public Rocket
{
public:
    RainbowRocket(double x, double y, double sx, double sy, unsigned int boomtime)
    {
        this->x=x;
        this->y=y;
        this->sx=sx;
        this->sy=sy;
        this->boom = boomtime+g.last_ticks;
        this->dead = false;
    }

    void update()
    {
        x += sx*g.time_mult;
        y += sy*g.time_mult;
        particles.push_back(new Particle(x, y, 100, 100, 100, 1, 1000));
        if(g.last_ticks> boom)
        {
            int speed = rand()%20+50;
            int time = rand()%500+2000;
            for(int i = 0; i<50; i++)
            {
                int deg = rand() % 360;
                double rad = (double) deg * 3.14159 / 180.0;
                double sx = sin(rad) * (double) speed;
                double sy = cos(rad) * (double) speed;
                int red, green, blue, col;
                col = rand() % 3;

                switch(col)
                {
                case 0:
                    red = 255;
                    green = 0;
                    blue = 0;
                    break;
                case 1:
                    red = 0;
                    green = 255;
                    blue = 0;
                    break;
                case 2:
                    red = 0;
                    green = 0;
                    blue = 255;
                    break;
                }


                fragments.push_back(new StandardFragment(x, y, sx, sy, red, green, blue, time));
            }
            this->dead = true;
        }
    }
};

class FancyRocket: public Rocket
{
public:
    FancyRocket(double x, double y, double sx, double sy, unsigned int boomtime)
    {
        this->x=x;
        this->y=y;
        this->sx=sx;
        this->sy=sy;
        this->boom = boomtime+g.last_ticks;
        this->dead = false;
    }

    void update()
    {
        x += sx*g.time_mult;
        y += sy*g.time_mult;
        particles.push_back(new Particle(x, y, 100, 100, 100, 1, 1000));
        if(g.last_ticks> boom)
        {
            int speed = rand()%30+50;
            for(int i = 0; i<50; i++)
            {
                int time = rand()%1000+2000;
                int deg = rand() % 360;
                double rad = (double) deg * 3.14159 / 180.0;
                double sx = sin(rad) * (double) speed;
                double sy = cos(rad) * (double) speed;


                fragments.push_back(new FancyFragment(x, y, sx, sy,  time));
            }
            this->dead = true;
        }
    }
};


class RocketFactory
{
public:
    float rps;
    float rrps;
    float frps;

    RocketFactory()
    {
        this->rps = 0.7;
        this->rrps = 0.3;
        this->frps = 0.8;
    }

    void update()
    {
        int mult = 10000;

        int tss = g.last_ticks - g.ticks_begin;

        float m = 1.3;

        if(tss < m*5000)
        {
            this->rps = 0.4;
            this->rrps = 0.0;
            this->frps = 0.0;
        }
        else if(tss < m*11000)
        {
            this->rps = 0.8;
            this->rrps = 0.0;
            this->frps = 0.0;
        }
        else if(tss < m*22000)
        {
            this->rps = 0.7;
            this->rrps = 0.2;
            this->frps = 0.0;
        }
        else if(tss < m*33000)
        {
            this->rps = 0;
            this->rrps = 0;
            this->frps = 1.5;
        }
        else if(tss < m*44000)
        {
            this->rps = 0.3;
            this->rrps = 0.0;
            this->frps = 0.9;
        }
        else
        {
            this->rps = 0.5;
            this->rrps = 0.1;
            this->frps = 0.7;
        }

        //normal rockets
        {
            int rps = mult*(this->rps * g.time_mult);
            int randval = rand() % mult;
            if (randval < rps)
            {
                rockets.push_back(new StandardRocket(rand() % g.width, g.height, rand() % 20 - 10, -(rand() % 30 + 60), 3000));
            }
        }

        //rainbow rockets
        {
            int rps = mult*(this->rrps * g.time_mult);
            int randval = rand() % mult;
            if (randval < rps)
            {
                rockets.push_back(new RainbowRocket(rand() % g.width, g.height, rand() % 20 - 10, -(rand() % 30 + 60), 3000));
            }
        }

        //fancy rockets
        {

            int rps = mult*(this->frps * g.time_mult);
            int randval = rand() % mult;
            if (randval < rps)
            {
                rockets.push_back(new FancyRocket(rand() % g.width, g.height, rand() % 20 - 10, -(rand() % 30 + 60), 3800));
            }
        }


    }
};
RocketFactory rf;




int main() {
    g.end = false;
    g.iter = 0;
    g.width = 800;
    g.height = 600;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_CreateWindowAndRenderer(g.width, g.height, 0, &g.w, &g.r);

    g.k = SDL_GetKeyboardState(0);


    // Particle * p = new Particle(10, 10, 255, 0, 0, 1000, 3000);
    // particles.push_back(p);

    // Rocket * r = new StandardRocket(400, 300, 10, -20, 5000);
    // rockets.push_back(r);
    //
    
    while(!g.k[SDL_SCANCODE_RETURN])
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
                g.end=true;
        }
    }   

    g.ticks_begin = SDL_GetTicks();
    g.update_ticks();


    while(!g.end)
        main_loop();

    SDL_Quit();
    return 0;
}


void main_loop()
{
    SDL_SetRenderDrawColor(g.r, 0, 0, 0, 0);
    SDL_RenderClear(g.r);
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        if(e.type == SDL_QUIT)
            g.end=true;
    }
    if(g.k[SDL_SCANCODE_ESCAPE] || g.k[SDL_SCANCODE_Q])
        g.end=true;



    g.update_ticks();

    rf.update();

    for( int i = 0; i < rockets.size(); i++)
    {
        Rocket * r = rockets[i];
        r->update();
        if(r->dead)
        {
            delete r;
            rockets[i] = nullptr;
        }
    }
    rockets.erase(std::remove_if(std::begin(rockets), std::end(rockets), [](Rocket * r) {
        return r == nullptr;
    }),
    std::end(rockets));

    for( int i = 0; i<particles.size(); i++)
    {
        particles[i]->update_and_render();
        if(particles[i]->dead)
        {
            delete particles[i];
            particles[i] = nullptr;
        }
    }
    particles.erase(std::remove_if(std::begin(particles), std::end(particles), [](Particle * p) {
        return p == nullptr;
    }),
    std::end(particles));

    for( int i = 0; i<fragments.size(); i++)
    {
        fragments[i]->update();
        if(fragments[i]->dead)
        {
            delete fragments[i];
            fragments[i] = nullptr;
        }
    }
    fragments.erase(std::remove_if(std::begin(fragments), std::end(fragments), [](Fragment * f) {
        return f == nullptr;
    }),
    std::end(fragments));

    SDL_RenderPresent(g.r);
    SDL_Delay(10);
}


