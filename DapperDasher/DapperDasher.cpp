#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isInAir(AnimData data, int windowHeight) {
    return !(data.pos.y >= windowHeight - data.rec.height);
}

AnimData updateAnimData(AnimData data, float deltaTime, int MaxFrame) {
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime) {
        data.runningTime = 0.0;
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > MaxFrame) {
            data.frame = 0;
        }
    }
    return data;
}

int main() {
    // Window dimensions
    int windowDimensions[2] = { 1366, 768 };

    // Velocity and gravity constants
    int velocity = 0;
    const int gravity = 1'000;
    const int jumpVel = -600;

    // Initialize the window
    InitWindow(windowDimensions[0], windowDimensions[1], "Tom Clancy's Rainbow Six Siege");
    SetTargetFPS(60);

    // Load textures
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    // Load music
    Music gameMusic = LoadMusicStream("textures/ter.mp3");
    SetMusicVolume(gameMusic , 50);
  
        PlayMusicStream(gameMusic);
    

    // Scarfy variables
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width / 6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0] / 2 - scarfyData.rec.width / 2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0 / 12.0;
    scarfyData.runningTime = 0.0;

    const int sizeofNebuale{ 10 };
    AnimData nebulae[sizeofNebuale]{};
    for (size_t i = 0; i < sizeofNebuale; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.y = windowDimensions[1] - nebulae[i].rec.height;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0 / 8.0;
        nebulae[i].pos.x = windowDimensions[0] + i * 300;
    }

    float finishLine{ nebulae[sizeofNebuale - 1].pos.x };

    int nebVel{ -200 };
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    bool collision{};

    // Main game loop
    while (!WindowShouldClose()) {
        // Delta time (time since last frame)
        const float dT = GetFrameTime();

        // Update music stream
        UpdateMusicStream(gameMusic);

        // Begin drawing
        BeginDrawing();
        ClearBackground(WHITE);

        bgX -= 30 * dT;

        // Reset bgX to create a seamless loop
        if (bgX <= -background.width * 6.0) {
            bgX = 0.0;
        }

        // Draw the background
        Vector2 bg1Pos{ bgX, 0.0 };
        DrawTextureEx(background, bg1Pos, 0.0, 6.0, WHITE);
        Vector2 bg2Pos{ bgX + background.width * 6, 0.0 };
        DrawTextureEx(background, bg2Pos, 0.0, 6.0, WHITE);

        // Draw the midground
        mgX -= 50 * dT;
        if (mgX <= -midground.width * 5.8) {
            mgX = 0.0;
        }

        Vector2 mg1Pos{ mgX, 0.0 };
        DrawTextureEx(midground, mg1Pos, 0.0, 5.8, WHITE);
        Vector2 mg2Pos{ mgX + midground.width * 5.8, 0.0 };
        DrawTextureEx(midground, mg2Pos, 0.0, 5.8, WHITE);

        // Draw the foreground
        fgX -= 70 * dT;
        if (fgX <= -foreground.width * 4.0) {
            fgX = 0.0;
        }
        Vector2 fg1Pos{ fgX, 0.0 };
        DrawTextureEx(foreground, fg1Pos, 0.0, 4.0, WHITE);
        Vector2 fg2Pos{ fgX + foreground.width * 4, 0.0 };
        DrawTextureEx(foreground, fg2Pos, 0.0, 4.0, WHITE);

        // Check if Scarfy is in the air
        bool scarfyInAir = isInAir(scarfyData, windowDimensions[1]);

        // Update Scarfy animation frame
        if (!scarfyInAir) {
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        // Update nebula animation frames
        for (int i = 0; i < sizeofNebuale; i++) {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }

        for (AnimData nebula : nebulae) {
            float pad{ 50 };
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if (CheckCollisionRecs(nebRec, scarfyRec)) {
                collision = true;
            }
        }

        if (collision)
        {
            // Lose the game
            const char* message = "Nice try, loser!";
            int textWidth = MeasureText(message, 50);
            DrawText(message, windowDimensions[0] / 2 - textWidth / 2, windowDimensions[1] / 2 - 25, 50, WHITE);
        }
        else if (scarfyData.pos.x >= finishLine) {
            const char* message = "Well, you won.";
            int textWidth = MeasureText(message, 50);
            DrawText(message, windowDimensions[0] / 2 - textWidth / 2, windowDimensions[1] / 2 - 25, 50, WHITE);
        }
        else
        {
            // Draw nebula textures
            for (int i = 0; i < sizeofNebuale; i++) {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }

            // Draw the current frame of Scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        // Apply gravity if Scarfy is in the air
        if (scarfyInAir) {
            velocity += gravity * dT;
        }
        else {
            velocity = 0; // Reset velocity when on the ground
        }

        // Handle jump input
        if (IsKeyPressed(KEY_SPACE) && !scarfyInAir) {
            velocity += jumpVel; // Apply jump velocity
        }

        // Update positions
        for (int i = 0; i < sizeofNebuale; i++) {
            nebulae[i].pos.x += nebVel * dT;
        }

        finishLine += nebVel * dT;

        scarfyData.pos.y += velocity * dT;

        
        EndDrawing();
    }

   
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadMusicStream(gameMusic); 
    CloseWindow();

    return 0;
}
