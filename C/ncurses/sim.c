#include <ncurses.h>
#include <unistd.h>
#include <string.h>

#define DELAY 40000

typedef struct {
    int x;
    int y;
} vec2;

typedef struct {
    vec2 ball;
    vec2 vel;
    vec2 windowmax;
    const char* text;
    size_t textlen;
} floater;

floater newFloater(vec2 ball, vec2 vel, vec2 windowmax, const char* text){
    floater newfloater = {ball, vel, windowmax, text, strlen(text)};
    return newfloater;
}

void updateFloater(floater* f) {
    vec2 next;
    next.x = f->ball.x + f->vel.x;
    next.y = f->ball.y + f->vel.y; 
    if (next.x + f->textlen > f->windowmax.x || next.x < 0) {
        f->vel.x *= -1;
    }
    else if (next.y > f->windowmax.y || next.y < 0) {
        f->vel.y *= -1;
    }

    f->ball.x += f->vel.x;
    f->ball.y += f->vel.y;
}

void drawFloater(floater f) {
    mvprintw(f.ball.y, f.ball.x, f.text);
}

int main(int argc, char *argv[]) {
    vec2 ball = {0,0};
    vec2 windowmax;
    vec2 vel = {2,1};
    vec2 next;

    initscr();
    noecho();
    curs_set(FALSE);

    // Global var `stdscr` is created by the call to `initscr()`
    getmaxyx(stdscr, windowmax.y, windowmax.x);
    floater f = newFloater(ball, vel, windowmax, argv[1]);

    while(1) {
        clear();
        updateFloater(&f);
        drawFloater(f);
        refresh();
        usleep(DELAY);
    }

    endwin();
}
