#include "game.hpp"
#include "assets.hpp"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120

#define SPRITE_SIZE 8
#define BORDER 2

#define PADDLE_SPEED 100

#define DEFAULT_WIDTH 10
#define DEFAULT_HEALTH 3

#define LEVEL_COUNT 1
#define LEVEL_WIDTH 10
#define LEVEL_HEIGHT 8

using namespace blit;

struct Paddle {
    float xPosition;

    int width;

    int health;
    int score;
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
void load_level(int levelLayout[LEVEL_HEIGHT][LEVEL_WIDTH]);
Block generate_block(int, int, int);

int state = 0;
float dt;
uint32_t lastTime = 0;


int highscore = 0;

int levelNumber;

Paddle player;
Ball ball;
std::vector<Block> blocks;

Surface* background = Surface::load(asset_background);

int levelLayouts[LEVEL_COUNT][LEVEL_HEIGHT][LEVEL_WIDTH] = {
    {
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        },
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        },
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        },
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
    }
};

void render_blocks() {
    for (int i = 0; i < blocks.size(); i++) {
        render_block(blocks.at(i));
    }
}

void render_block(Block block) {
    screen.sprite((block.health - 1) * 2, Point(block.xPosition, block.yPosition));
    screen.sprite((block.health - 1) * 2 + 1, Point(block.xPosition + SPRITE_SIZE, block.yPosition));
}

void render_player() {
    int left = player.xPosition - player.width; // fix offset?
    int yPosition = SCREEN_HEIGHT - BORDER * 2;

    screen.blit(screen.sprites, Rect(4, 16, 1, 4), Point(left, yPosition));

    for (int i = 0; i < player.width - 1; i++) {
        screen.blit(screen.sprites, Rect(5, 16, 2, 4), Point(left + 1 + i * 2, yPosition));
    }

    screen.blit(screen.sprites, Rect(7, 16, 1, 4), Point(left + player.width * 2 - 1, yPosition));
}

void render_hud() {
    // SCORE
    screen.blit(screen.sprites, Rect(48, 24, 24, 8), Point(BORDER, BORDER));

    // :
    screen.blit(screen.sprites, Rect(72, 24, 2, 8), Point(BORDER + SPRITE_SIZE * 3 + 1, BORDER));

    // <score>
    screen.blit(screen.sprites, Rect(4 * (int)((player.score % 100000) / 10000), 24, 4, 8), Point(BORDER + SPRITE_SIZE * 3 + 4, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((player.score % 10000) / 1000), 24, 4, 8), Point(BORDER + SPRITE_SIZE * 3 + 9, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((player.score % 1000) / 100), 24, 4, 8), Point(BORDER + SPRITE_SIZE * 3 + 14, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((player.score % 100) / 10), 24, 4, 8), Point(BORDER + SPRITE_SIZE * 3 + 19, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((player.score % 10)), 24, 4, 8), Point(BORDER + SPRITE_SIZE * 3 + 24, BORDER));


    // HI
    screen.blit(screen.sprites, Rect(40, 24, 8, 8), Point(SCREEN_WIDTH - BORDER - 28 - SPRITE_SIZE, BORDER));

    // :
    screen.blit(screen.sprites, Rect(72, 24, 2, 8), Point(SCREEN_WIDTH - BORDER - 27, BORDER));

    // <highscore>
    screen.blit(screen.sprites, Rect(4 * (int)((highscore % 100000) / 10000), 24, 4, 8), Point(SCREEN_WIDTH - BORDER - 24, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((highscore % 10000) / 1000), 24, 4, 8), Point(SCREEN_WIDTH - BORDER - 19, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((highscore % 1000) / 100), 24, 4, 8), Point(SCREEN_WIDTH - BORDER - 14, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((highscore % 100) / 10), 24, 4, 8), Point(SCREEN_WIDTH - BORDER - 9, BORDER));
    screen.blit(screen.sprites, Rect(4 * (int)((highscore % 10)), 24, 4, 8), Point(SCREEN_WIDTH - BORDER - 4, BORDER));

    // Health
    for (int i = 0; i < player.health; i++) {
        screen.sprite(47, Point(SCREEN_WIDTH / 2 + (SPRITE_SIZE * (i - 2.5)), BORDER));
        //screen.blit(screen.sprites, Rect(112, 16, 16, 4), Point(SCREEN_WIDTH / 2 + (SPRITE_SIZE * (i - 1.5)), BORDER));
    }

    // Level
    screen.blit(screen.sprites, Rect(80, 24, 16, 8), Point(SCREEN_WIDTH / 2 + 10, BORDER));

    // :
    screen.blit(screen.sprites, Rect(72, 24, 2, 8), Point(SCREEN_WIDTH / 2 + 25, BORDER));

    // <levelnumber
    screen.blit(screen.sprites, Rect(4 * (levelNumber + 1), 24, 4, 8), Point(SCREEN_WIDTH / 2 + 28, BORDER));
}

void start_game() {
    player.health = DEFAULT_HEALTH;
    player.score = 0;

    start_level(0);
}

void start_level(int levelNumber) {
    player.xPosition = SCREEN_WIDTH / 2;
    player.width = DEFAULT_WIDTH;

    load_level(levelLayouts[levelNumber]);
}

void load_level(int levelLayout[LEVEL_HEIGHT][LEVEL_WIDTH]) {
    blocks.clear();

    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            if (levelLayout[y][x] > 0) {
                blocks.push_back(generate_block(levelLayout[y][x], x, y));
            }
        }
    }
}

Block generate_block(int health, int x, int y) {
    Block block;
    block.health = health;
    block.xPosition = x * SPRITE_SIZE * 2;
    block.yPosition = (y + 1.5) * SPRITE_SIZE;
    return block;
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
    else if (state == 1) {
        if (buttons.pressed & Button::DPAD_LEFT) {

        }
        else if (buttons.pressed & Button::DPAD_RIGHT) {

        }
    }
}