#include "game.hpp"
#include "assets.hpp"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120

#define SPRITE_SIZE 8

#define PADDLE_SPEED 100

#define DEFAULT_WIDTH 6

using namespace blit;

struct Paddle {
    float xPosition;

    int width;

    int health;
};

struct Block {
    int xPosition, yPosition;

    int health;
};

struct Ball {
    float xPosition, yPosition;
    float xVelocity, yVelocity;
};

void render_blocks();
void render_block(Block);
void render_player();
void render_hud();
void start_game();
void start_level(int);
void load_level(int[]);

int state = 0;
float dt;
uint32_t lastTime = 0;

Paddle player;
Ball ball;
std::vector<Block> blocks;

Surface* background = Surface::load(asset_background);

int levelLayouts[1][1] = { 
    {
        0
    }
};

void render_blocks() {
    for (int i = 0; i < blocks.size(); i++) {
        render_block(blocks.at(i));
    }
}

void render_block(Block block) {

}

void render_player() {
    int left = player.xPosition - player.width - 1; // fix offset?
    int yPosition = SCREEN_HEIGHT - SPRITE_SIZE;

    screen.blit(screen.sprites, Rect(4, 16, 1, 4), Point(left, yPosition));

    for (int i = 0; i < player.width; i++) {
        screen.blit(screen.sprites, Rect(5, 16, 2, 4), Point(left + 1 + i * 2, yPosition));
    }

    screen.blit(screen.sprites, Rect(7, 16, 1, 4), Point(left + 1 + player.width * 2, yPosition));
}

void render_hud() {

}

void start_game() {
    start_level(0);
}

void start_level(int levelNumber) {
    player.xPosition = 6;//SCREEN_WIDTH / 2;
    player.width = DEFAULT_WIDTH;

    load_level(levelLayouts[levelNumber]);
}

void load_level(int levelLayout[]) {
    // assign to blocks
}

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
    set_screen_mode(ScreenMode::lores);
    screen.sprites = SpriteSheet::load(asset_sprites);
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time) {

    // clear the screen -- screen is a reference to the frame buffer and can be used to draw all things with the 32blit
    screen.clear();

    // draw some text at the top of the screen
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(255, 255, 255);

    screen.blit(background, Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), Point(0, 0), false);

    if (state == 0) {
        screen.text("Jumpy Squirrel", minimal_font, Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 1 / 3), true, TextAlign::center_center); // change to custom icon

        screen.text("Press A to Start", minimal_font, Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 3), true, TextAlign::center_center);
    }
    else if (state == 1) {
        render_blocks();

        render_player();

        render_hud();
    }

    screen.pen = Pen(0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time) {
    dt = (time - lastTime) / 1000.0;
    lastTime = time;

    if (state == 0) {
        if (buttons.pressed & Button::A) {
            state = 1;
            start_game();
        }
    }
}