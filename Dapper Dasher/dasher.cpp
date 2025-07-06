#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool IsOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData UpdateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    // window dimensions
    const int windowWidth{512};
    const int windowHeight{380};

    // target FPS
    const int targetFPS{60};

    // initialize window with constant size
    InitWindow(windowWidth, windowHeight, "Dapper Dasher!");

    /* rectangle dimensions depreciated
    const int recWidth{50};
    const int recHeight{80};*/

    // accelaration due to gravity = (pixels/second)/second
    const int gravity{1000};

    // Scarfy texture
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    // Nebula texture
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    // Background textures
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float bgX{0};
    float mgX{0};
    float fgX{0};

    // scarfy anim data
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = (windowWidth/2 - scarfyData.rec.width/2);
    scarfyData.pos.y = (windowHeight - scarfyData.rec.height);
    scarfyData.frame = 0;
    scarfyData.updateTime = (1.0/12.0);
    scarfyData.runningTime = 0.0;

    const int sizeOfNebulae{6};
    const int nebulaOffset{300};
    AnimData nebulae[sizeOfNebulae]{};

    for (int i = 0; i < sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x = windowWidth + (i * nebulaOffset);
        nebulae[i].pos.y = windowHeight - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = (1.0/16.0);
    }

    float finishLine{nebulae[sizeOfNebulae -1].pos.x};

    // nebula X velocity (pixels/second), use for all nebula hazards so they don't pass each other
    int nebVel{-200};

    /* initialize scarfyRec - Depreciated initialization values - 7/6/25
    scarfyRec.width = scarfy.width/6;
    scarfyRec.height = scarfy.height;
    scarfyRec.x = 0;
    scarfyRec.y = 0;
    //int posY{windowHeight - recHeight}; depreciated

    // initialize scarfyPos
    scarfyPos.x = windowWidth/2 - scarfyRec.width/2;
    scarfyPos.y = windowHeight - scarfyRec.height;*/

    // position variables
    bool isInAir{false}; // is the rectangle in the air
    int velocity{0};

    // the constant jump height of the rectangle, pixels per second
    const int jumpHeight{-600};

    // collision check variable
    bool collision{false};

    // force the framerate to be constant
    SetTargetFPS(targetFPS);

    while (!WindowShouldClose())
    {
        // delta time
        const float dT{GetFrameTime()};

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // scroll the background textures
        bgX -= 20 * dT;
        mgX -= 40 * dT;
        fgX -= 80 * dT;

        if (bgX <= -background.width*2)
        {
            bgX = 0.0;
        }
        if (mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }
        if (fgX <= -foreground.width*2)
        {
            fgX = 0.0;
        }
        
        Vector2 background1Pos{bgX,0.0};
        DrawTextureEx(background, background1Pos, 0.0, 2.0, WHITE);
        Vector2 background2Pos{bgX + background.width*2, 0.0};
        DrawTextureEx(background, background2Pos, 0.0, 2.0, WHITE);

        Vector2 mg1Pos{mgX,0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        Vector2 mg2Pos{mgX + midground.width*2, 0.0};
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        Vector2 fg1Pos{fgX,0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        Vector2 fg2Pos{fgX + foreground.width*2, 0.0};
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);
        
        // perform ground check
        if (IsOnGround(scarfyData, windowHeight))
        {
            //rectangle is on the ground            
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // rectangle is in the air
            velocity += gravity * dT;
            isInAir = true;
        }

        // jump with space key only if the rectangle is on the ground
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpHeight;
        }

        for (int i = 0; i < sizeOfNebulae; i++)
        {
            // update nebula position
            nebulae[i].pos.x += nebVel * dT;
        }

        finishLine += nebVel * dT;
        
        for (int i = 0; i < sizeOfNebulae; i++)
        {
            nebulae[i] = UpdateAnimData(nebulae[i], dT, 7);
        }

        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        // updating scarfy animation frame
        if (!isInAir)
        {
            scarfyData = UpdateAnimData(scarfyData, dT, 5);
        }

        
        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };

            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
            
        }

        if (collision)
        {
            // lose the game
            DrawText("Game Over!", windowWidth/3, windowHeight/2, 40, RED);
        }
        else if (scarfyData.pos.x >= finishLine)
        {
            // win the game
            DrawText("You Win!", windowWidth/3, windowHeight/2, 40, RED);
        }
        
        else
        {
            for (int i = 0; i < sizeOfNebulae; i++)
            {
                // draw nebula
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);            
            }
    
            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }
        // end drawing
        EndDrawing();
    }

    // properly close down the window
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}