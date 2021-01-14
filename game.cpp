#include "game.hpp"
#include "assets.hpp"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120

#define SPRITE_SIZE 8
#define BORDER 2

#define MAX_X_VELOCITY 0.95

#define PADDLE_SPEED 55
#define BALL_SPEED 70

#define MIN_JOYSTICK 0.2

#define DEFAULT_WIDTH 8
#define DEFAULT_HEALTH 3

#define LEVEL_COUNT 5
#define LEVEL_WIDTH 10
#define LEVEL_HEIGHT 8

#define BLOCK_VALUE 5

using namespace blit;

struct SaveData {
    int highscore;
};

struct Paddle {
    float xPosition, yPosition;

    int width;

    int health;
    int score;

    int combo; // used for combos
};

struct Block {
    int xPosition, yPosition;

    int row, column;

    int health;

    bool noValue;
};

struct Ball {
    float xPosition, yPosition;
    float xVelocity, yVelocity;

    bool held;
};

void render_blocks();
void render_block(Block);
void render_player();
void render_hud();
void render_ball();
void start_game();
void start_level(int);
void reset_ball();
void load_level(int levelLayout[LEVEL_HEIGHT][LEVEL_WIDTH]);
Block generate_block(int, int, int);
void handle_block_collisions();
int blocks_remaining();

int state = 0;
float dt;
uint32_t lastTime = 0;


SaveData saveData;


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
        }
    },
    {
        {
            0, 0, 0, 1, 1, 1, 1, 0, 0, 0
        },
        {
            0, 0, 1, 1, 2, 2, 1, 1, 0, 0
        },
        {
            0, 1, 1, 2, 3, 3, 2, 1, 1, 0
        },
        {
            0, 0, 1, 1, 2, 2, 1, 1, 0, 0
        },
        {
           -2,-2,-2, 1, 1, 1, 1,-2,-2,-2
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    },
    {
        {
            1, 2, 3, 4, 5, 5, 4, 3, 2, 1
        },
        {
            1, 1, 2, 3, 4, 4, 3, 2, 1, 1
        },
        {
            0, 1, 1, 2, 3, 3, 2, 1, 1, 0
        },
        {
            0, 0, 1, 1, 2, 2, 1, 1, 0, 0
        },
        {
            0, 0, 0, 1, 1, 1, 1, 0, 0, 0
        },
        {
            0, 0, 0, 0, 1, 1, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    },
    {
        {
            1, 2, 3, 2, 3, 3, 2, 3, 2, 1
        },
        {
            1, 1, 2, 3, 4, 4, 3, 2, 1, 1
        },
        {
            0, 1, 1, 2, 6, 6, 2, 1, 1, 0
        },
        {
            0, 0, 2,-1,-1,-1,-1, 2, 0, 0
        },
        {
            2, 0, 0, 2, 6, 6, 2, 0, 0, 2
        },
        {
           -1,-1,-2,-2, 2, 2,-2,-2,-1,-1
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    },
    {
        {
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6
        },
        {
            6, 6, 6,-1, 6, 6,-1, 6, 6, 6
        },
        {
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6
        },
        {
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6
        },
        {
            6, 6,-1, 6, 6, 6, 6,-1, 6, 6
        },
        {
            6, 6, 6,-1,-1,-1,-1, 6, 6, 6
        },
        {
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6
        },
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    }
};

float min(float a, float b) {
    return a < b ? a : b;
}

float max(float a, float b) {
    return a > b ? a : b;
}

float clamp(float x, float mi, float ma) {
    return min(max(x, mi), ma);
}

void render_blocks() {
    for (int i = 0; i < blocks.size(); i++) {
        render_block(blocks.at(i));
    }
}

void render_block(Block block) {
    int index = block.health - 1;
    if (block.noValue) {
        index = 10 - block.health;
    }
    if (block.health == -1) {
        index = 6;
    }
    if (block.health != 0) {
        screen.sprite(index * 2, Point(block.xPosition, block.yPosition));
        screen.sprite(index * 2 + 1, Point(block.xPosition + SPRITE_SIZE, block.yPosition));
    }
}

void render_player() {
    int left = player.xPosition - player.width;

    screen.blit(screen.sprites, Rect(4, 16, 1, 4), Point(left, player.yPosition));

    for (int i = 0; i < player.width - 1; i++) {
        screen.blit(screen.sprites, Rect(5, 16, 2, 4), Point(left + 1 + i * 2, player.yPosition));
    }

    screen.blit(screen.sprites, Rect(7, 16, 1, 4), Point(left + player.width * 2 - 1, player.yPosition));
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

    // <levelnumber>
    screen.blit(screen.sprites, Rect(4 * (levelNumber + 1), 24, 4, 8), Point(SCREEN_WIDTH / 2 + 28, BORDER));

    int multiplier = 1 + (player.combo / 2);

    if (multiplier > 1) {
        // multiplier
        screen.text("x" + std::to_string(multiplier), minimal_font, Point(BORDER * 4, BORDER * 8), true, TextAlign::center_center);
    }
}

void render_ball() {
    screen.blit(screen.sprites, Rect(0, 16, 4, 4), Point(ball.xPosition, ball.yPosition));
}

void start_game() {
    player.health = DEFAULT_HEALTH;
    player.score = 0;

    start_level(4);
}

void start_level(int levelNum) {
    levelNumber = levelNum;
    player.xPosition = SCREEN_WIDTH / 2;
    player.width = DEFAULT_WIDTH;

    load_level(levelLayouts[levelNumber]);

    reset_ball();
}

void reset_ball() {
    ball.xPosition = player.xPosition - SPRITE_SIZE / 4;
    ball.yPosition = player.yPosition - SPRITE_SIZE / 2;

    ball.xVelocity = 0;
    ball.yVelocity = 0;

    ball.held = true;

    player.combo = 0;
}

void load_level(int levelLayout[LEVEL_HEIGHT][LEVEL_WIDTH]) {
    blocks.clear();

    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            if (levelLayout[y][x] != 0) {
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

    if (health == -2) {
        block.health = 3;
        block.noValue = true;
    }
    else {
        block.noValue = false;
    }

    block.row = y;
    block.column = x;

    return block;
}

void handle_block_collisions() {
    // check this row and the next
    int row = (ball.yPosition - SPRITE_SIZE * 2) / (float)SPRITE_SIZE;

    // check this column and the next
    int column = (ball.xPosition - SPRITE_SIZE * 0.5) / (float)(SPRITE_SIZE * 2);

    //printf("%d,%d\n",row, column);

    for (int i = 0; i < blocks.size(); i++) {
        if (blocks.at(i).health != 0) {
            if (blocks.at(i).row == row || blocks.at(i).row == row + 1) {
                if (blocks.at(i).column == column || blocks.at(i).column == column + 1) {
                    // probably collided with block, now check the side the ball collided with
                    if (ball.xPosition + SPRITE_SIZE / 2 > blocks.at(i).xPosition && ball.xPosition < blocks.at(i).xPosition + SPRITE_SIZE * 2) {
                        if (ball.yPosition + SPRITE_SIZE / 2 > blocks.at(i).yPosition && ball.yPosition < blocks.at(i).yPosition + SPRITE_SIZE) {
                            // definitely collided, take 1 hp off the block
                            if (blocks.at(i).health > 0) {
                                blocks.at(i).health -= 1;
                            }

                            // NOTE: collision detection is bad currently. Not sure whether to use first or second block
                            
                            //bool continueChecking = true;
                            //float x = ball.xPosition+1, y = ball.yPosition+1;

                            //while (continueChecking) {
                            //    continueChecking = false;
                            //    //printf("%f %f\n", x, y);
                            //    // now we need to check which side the ball collided with in order handle rebounds
                            //    if (ball.yVelocity < 0 && (int)y == blocks.at(i).yPosition + SPRITE_SIZE) {
                            //        // hit on bottom
                            //        ball.yVelocity = std::abs(ball.yVelocity);
                            //    }
                            //    else if (ball.yVelocity > 0 && (int)y + SPRITE_SIZE / 2 == blocks.at(i).yPosition) {
                            //        // hit on top
                            //        ball.yVelocity = -std::abs(ball.yVelocity);
                            //    }
                            //    else if (ball.xVelocity > 0 && (int)x + SPRITE_SIZE / 2 == blocks.at(i).xPosition) {
                            //        // hit on left
                            //        ball.xVelocity = -std::abs(ball.xVelocity);
                            //    }
                            //    else if (ball.xVelocity < 0 && (int)x == blocks.at(i).xPosition + SPRITE_SIZE * 2) {
                            //        // hit on right
                            //        ball.xVelocity = std::abs(ball.xVelocity);
                            //    }
                            //    else {
                            //        continueChecking = true;
                            //        x -= ball.xVelocity;
                            //        y -= ball.yVelocity;
                            //    }
                            //}

                            // now we need to check which side the ball collided with in order handle rebounds
                            if (ball.yVelocity < 0 && ball.yPosition > blocks.at(i).yPosition + SPRITE_SIZE / 2) {
                                // hit on bottom
                                ball.yVelocity = std::abs(ball.yVelocity);
                                ball.yPosition = blocks.at(i).yPosition + SPRITE_SIZE;
                            }
                            else if (ball.yVelocity > 0 && ball.yPosition + SPRITE_SIZE / 2 < blocks.at(i).yPosition + SPRITE_SIZE / 2) {
                                // hit on top
                                ball.yVelocity = -std::abs(ball.yVelocity);
                                ball.yPosition = blocks.at(i).yPosition - SPRITE_SIZE / 2;
                            }
                            else if (ball.xVelocity > 0 && ball.xPosition + SPRITE_SIZE / 2 < blocks.at(i).xPosition + SPRITE_SIZE) {
                                // hit on left
                                ball.xVelocity = -std::abs(ball.xVelocity);
                                ball.xPosition = blocks.at(i).xPosition - SPRITE_SIZE / 2;
                            }
                            else if (ball.xVelocity < 0 && ball.xPosition > blocks.at(i).xPosition + SPRITE_SIZE) {
                                // hit on right
                                ball.xVelocity = std::abs(ball.xVelocity);
                                ball.xPosition = blocks.at(i).xPosition + SPRITE_SIZE * 2;
                            }

                            if (!blocks.at(i).noValue && blocks.at(i).health >= 0) {
                                // need to add points, block wasn't a wall

                                // calculate multiplier, increase score by adjusted value
                                player.score += BLOCK_VALUE * (1 + (int)(player.combo / 2));

                                // increase combo after adjusting score
                                player.combo += 1;
                            }

                            //break; // only allow one block to be destroyed at a time?
                        }
                    }
                }
            }
        }
    }
}

int blocks_remaining() {
    int total = 0;
    for (int i = 0; i < blocks.size(); i++) {
        if (blocks.at(i).health > 0 && !blocks.at(i).noValue) {
            total++;
        }
    }
    return total;
}

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init() {
    set_screen_mode(ScreenMode::lores);
    screen.sprites = Surface::load(asset_sprites);

    player.yPosition = SCREEN_HEIGHT - BORDER * 2;

    // Attempt to load the first save slot.
    if (read_save(saveData)) {
        // Loaded sucessfully!
        highscore = saveData.highscore;
    }
    else {
        // No save file or it failed to load, set up some defaults.
        saveData.highscore = 0;
    }
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
        screen.text("ArkaBlit", minimal_font, Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 1 / 3), true, TextAlign::center_center); // change to custom icon

        screen.text("Press A to Start", minimal_font, Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2 / 3), true, TextAlign::center_center);
    }
    else if (state == 1) {
        render_blocks();

        render_player();

        render_ball();

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
        if ((buttons & Button::DPAD_LEFT) || joystick.x < -MIN_JOYSTICK) {
            player.xPosition -= PADDLE_SPEED * dt;
        }
        if ((buttons & Button::DPAD_RIGHT) || joystick.x > MIN_JOYSTICK) {
            player.xPosition += PADDLE_SPEED * dt;
        }

        player.xPosition = clamp(player.xPosition, player.width, SCREEN_WIDTH - player.width);

        if (ball.xPosition < 0 || ball.xPosition + SPRITE_SIZE / 2 > SCREEN_WIDTH) {
            ball.xVelocity = -ball.xVelocity;
        }
        if (ball.yPosition < SPRITE_SIZE * 1.5) {
            ball.yPosition = SPRITE_SIZE * 1.5;
            ball.yVelocity = -ball.yVelocity;
        }
        if (ball.yPosition > SCREEN_HEIGHT + SPRITE_SIZE / 2) {
            player.health -= 1;
            reset_ball();
        }

        ball.xPosition += ball.xVelocity * BALL_SPEED * dt;
        ball.yPosition += ball.yVelocity * BALL_SPEED * dt;

        if (ball.held) {
            reset_ball();

            if (buttons.pressed & Button::A) {
                ball.held = false;

                // offset xVelocity by a random amount
                ball.xVelocity = ((rand() % 100) / 100.0) - 0.5;

                ball.yVelocity = -std::sqrt(1 - (ball.xVelocity * ball.xVelocity));
            }
        }

        //handle ball collisions

        // Paddle collision
        if (ball.xPosition > player.xPosition - player.width - SPRITE_SIZE / 4 && ball.xPosition + SPRITE_SIZE / 2 < player.xPosition + player.width + SPRITE_SIZE / 4 && ball.yVelocity > 0) {
            if (ball.yPosition + SPRITE_SIZE / 2 > player.yPosition && ball.yPosition < player.yPosition) {
                //ball.xVelocity = clamp(ball.xVelocity - ((player.xPosition - (ball.xPosition + SPRITE_SIZE / 4)) / (float)(player.width + SPRITE_SIZE / 2)), -MAX_X_VELOCITY, MAX_X_VELOCITY);
                // new version below ----v----- deflects the ball less
                //ball.xVelocity = clamp(ball.xVelocity - ((player.xPosition - (ball.xPosition + SPRITE_SIZE / 4)) / (float)(player.width * 2)), -MAX_X_VELOCITY, MAX_X_VELOCITY);

                ball.xVelocity = clamp(ball.xVelocity - ((player.xPosition - (ball.xPosition + SPRITE_SIZE / 4)) / (float)(player.width + SPRITE_SIZE)), -MAX_X_VELOCITY, MAX_X_VELOCITY);

                ball.yVelocity = -std::sqrt(1 - (ball.xVelocity * ball.xVelocity));

                // reset player combo
                player.combo = 0;
            }
        }

        handle_block_collisions();

        // level admin stuff

        if (blocks_remaining() == 0) {
            // next level
            if (LEVEL_COUNT > levelNumber) {
                start_level(levelNumber + 1);
            }
            else {
                start_level(0); //loop back round
            }
        }

        if (player.health == 0) {
            // player died
            highscore = max(highscore, player.score);
            saveData.highscore = highscore;
            write_save(saveData); // write highscore
            state = 0;
        }
    }
}