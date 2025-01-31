#include "raylib.h"
#include <raymath.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 20
#define BALL_SIZE 16
#define NUM_BRICKS 30

struct Ball {
    Rectangle rect;
    Vector2 speed;
};

struct Paddle {
    Rectangle rect;
};

struct Brick {
    Rectangle rect;
    bool active;
};

struct PowerUpBar {
    Rectangle rect;
    float fillAmount;
};

Ball ball;
Paddle paddle;
Brick bricks[NUM_BRICKS];
PowerUpBar powerUpBar;

int lives = 1;
bool gameLost = false;
bool gameWon = false;
bool gameStarted = false;
float powerUpThreshold = 10.0f;
float powerUpProgress = 0.0f;

void ResetBall() {
    ball.rect.x = SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    ball.rect.y = SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    ball.speed = { 300, -300 };
}

void InitializeBricks() {
    int brickWidth = SCREEN_WIDTH / 10;
    int brickHeight = 20;

    for (int i = 0; i < NUM_BRICKS; i++) {
        bricks[i].rect.x = (i % 10) * brickWidth;
        bricks[i].rect.y = (i / 10) * brickHeight + 50;
        bricks[i].rect.width = brickWidth - 2;
        bricks[i].rect.height = brickHeight - 2;
        bricks[i].active = true;
    }
}

void UpdateGame() {
    if (IsKeyDown(KEY_LEFT)) paddle.rect.x -= 500 * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) paddle.rect.x += 500 * GetFrameTime();
    paddle.rect.x = Clamp(paddle.rect.x, 0, SCREEN_WIDTH - paddle.rect.width);

    Vector2 ballMovement = Vector2Scale(ball.speed, GetFrameTime());
    ball.rect.x += ballMovement.x;
    ball.rect.y += ballMovement.y;

    if (ball.rect.x <= 0 || ball.rect.x + ball.rect.width >= SCREEN_WIDTH) {
        ball.speed.x *= -1;
    }
    if (ball.rect.y <= 0) {
        ball.speed.y *= -1;
    }

    if (CheckCollisionRecs(ball.rect, paddle.rect)) {
        ball.speed.y *= -1;
    }

    for (int i = 0; i < NUM_BRICKS; i++) {
        if (bricks[i].active && CheckCollisionRecs(ball.rect, bricks[i].rect)) {
            bricks[i].active = false;
            ball.speed.y *= -1;
            powerUpProgress += 1.0f;

            if (powerUpProgress >= powerUpThreshold) {
                lives++;
                powerUpProgress = 0.0f;
            }
            break;
        }
    }

    if (ball.rect.y > SCREEN_HEIGHT) {
        lives--;
        if (lives > 0) {
            ResetBall();
        }
        else {
            gameLost = true;
        }
    }

    gameWon = true;
    for (int i = 0; i < NUM_BRICKS; i++) {
        if (bricks[i].active) {
            gameWon = false;
            break;
        }
    }

    powerUpBar.fillAmount = powerUpProgress / powerUpThreshold;
}

void DrawGame() {
    DrawRectangleRec(paddle.rect, BLUE);
    DrawCircleV({ ball.rect.x + BALL_SIZE / 2, ball.rect.y + BALL_SIZE / 2 }, BALL_SIZE / 2, ORANGE);
    DrawCircleV({ ball.rect.x + BALL_SIZE / 2, ball.rect.y + BALL_SIZE / 2 }, BALL_SIZE / 3, YELLOW);

    for (int i = 0; i < NUM_BRICKS; i++) {
        if (bricks[i].active) {
            DrawRectangleRec(bricks[i].rect, RED);
        }
    }

    DrawRectangleRec(powerUpBar.rect, GRAY);
    DrawRectangle(powerUpBar.rect.x, powerUpBar.rect.y, powerUpBar.rect.width * powerUpBar.fillAmount, powerUpBar.rect.height, GREEN);

    DrawText(TextFormat("Lives: %d", lives), 10, 10, 20, WHITE);

    if (gameWon) {
        DrawText("You Won!", SCREEN_WIDTH / 2 - MeasureText("You Won!", 30) / 2, SCREEN_HEIGHT / 2 - 30, 30, GREEN);
    }
    if (gameLost) {
        DrawText("Game Over!", SCREEN_WIDTH / 2 - MeasureText("Game Over!", 30) / 2, SCREEN_HEIGHT / 2 - 30, 30, RED);
    }
}

void DrawMenu() {
    DrawText("Starbreaker", SCREEN_WIDTH / 2 - MeasureText("Starbreaker", 40) / 2, SCREEN_HEIGHT / 4 - 50, 40, BLUE);
    DrawText("Press ENTER to Start", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to Start", 20) / 2, SCREEN_HEIGHT / 2, 20, BLACK);
    DrawText("Press ESC to Quit", SCREEN_WIDTH / 2 - MeasureText("Press ESC to Quit", 20) / 2, SCREEN_HEIGHT / 2 + 30, 20, BLACK);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Starbreaker");
    SetTargetFPS(60);

    paddle.rect = { SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 20, PADDLE_WIDTH, PADDLE_HEIGHT };
    ball.rect = { SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2, BALL_SIZE, BALL_SIZE };
    ball.speed = { 300, -300 };

    InitializeBricks();

    powerUpBar.rect = { 10, 10, SCREEN_WIDTH / 4, 20 };
    powerUpBar.fillAmount = 0.0f;

    while (!WindowShouldClose()) {
        if (!gameStarted) {
            if (IsKeyPressed(KEY_ENTER)) {
                gameStarted = true;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
                return 0;
            }
        }
        else {
            if (!gameLost && !gameWon) {
                UpdateGame();
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameStarted) {
            DrawMenu();
        }
        else {
            DrawGame();
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
