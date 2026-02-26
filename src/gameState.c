#include "gameState.h"
#include <stdio.h>
#include "fizzlefade.h"

#ifdef THREADED_FLOOR_RENDERING
#include <pthread.h>
#endif

void GameState_init(GameState *gameState, SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Surface *Loading_Surf;

    Loading_Surf = SDL_LoadBMP("../assets/walls.bmp");
    gameState->wallsTexture = SDL_CreateTextureFromSurface(renderer, Loading_Surf);
    SDL_FreeSurface(Loading_Surf);

    Loading_Surf = SDL_LoadBMP("../assets/bar.bmp");
    SDL_SetColorKey(Loading_Surf, SDL_TRUE, SDL_MapRGB(Loading_Surf->format, 255, 0, 255));
    gameState->barTexture = SDL_CreateTextureFromSurface(renderer, Loading_Surf);
    SDL_FreeSurface(Loading_Surf);

    Loading_Surf = SDL_LoadBMP("../assets/enemy.bmp");
    SDL_SetColorKey(Loading_Surf, SDL_TRUE, SDL_MapRGB(Loading_Surf->format, 255, 0, 255));
    gameState->soldierTexture = SDL_CreateTextureFromSurface(renderer, Loading_Surf);
    SDL_FreeSurface(Loading_Surf);

    Loading_Surf = SDL_LoadBMP("../assets/gun.bmp");
    SDL_SetColorKey(Loading_Surf, SDL_TRUE, SDL_MapRGB(Loading_Surf->format, 255, 0, 255));
    gameState->gunTexture = SDL_CreateTextureFromSurface(renderer, Loading_Surf);
    SDL_FreeSurface(Loading_Surf);

    Loading_Surf = SDL_LoadBMP("../assets/screen.bmp");
    SDL_SetColorKey(Loading_Surf, SDL_TRUE, SDL_MapRGB(Loading_Surf->format, 255, 0, 255));
    gameState->frameTexture = SDL_CreateTextureFromSurface(renderer, Loading_Surf);
    SDL_FreeSurface(Loading_Surf);

    gameState->wallsSurface = SDL_LoadBMP("../assets/walls.bmp");
    gameState->soldierSurface = SDL_LoadBMP("../assets/soldier.bmp");

    gameState->youDiedText = TextureXAspectRation_create(renderer, font, "YOU DIED", (SDL_Color){237, 0, 3});
    gameState->font = font;

    ///////
    Player_init(&(gameState->player));
    KeysArray_init(&(gameState->keys));
    EntityArray_init(&(gameState->enemies), 128, sizeof(Enemy));

    for (int i = 0; i < 1; i++){
        Enemy e = Enemy_create(Vector2f_create(2 + i * 2.f, 2));
        EntityArray_add(&(gameState->enemies), &e);
    }

    EntityArray_init(&(gameState->enemyProjectiles), 32, sizeof(Projectile));

    Map_init(&gameState->map);

    gameState->rays = calloc(SCREEN_WIDTH, sizeof(Ray));
    if (gameState->rays == NULL)
        exit(1);

    gameState->screenSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    SDL_SetSurfaceRLE(gameState->screenSurface, 1);

    // VIGNETE
    SDL_Surface *vignete_surf = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    memset(vignete_surf->pixels, 255, SCREEN_HEIGHT * SCREEN_WIDTH * vignete_surf->format->BytesPerPixel);
    SDL_SetColorKey(vignete_surf, SDL_TRUE, SDL_MapRGB(vignete_surf->format, 255, 255, 255));
    gameState->vigneteTexture = SDL_CreateTextureFromSurface(renderer, vignete_surf);
    // for(int i = 0;)
    SDL_FreeSurface(vignete_surf);
    //

    gameState->deathScreenSurface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    memset(gameState->deathScreenSurface->pixels, 255, SCREEN_HEIGHT * SCREEN_WIDTH * gameState->deathScreenSurface->format->BytesPerPixel);
    SDL_SetColorKey(gameState->deathScreenSurface, SDL_TRUE, SDL_MapRGB(gameState->deathScreenSurface->format, 255, 255, 255));
    SDL_SetSurfaceRLE(gameState->deathScreenSurface, 1);

    gameState->rndval = 1;
    gameState->isDeathAnimFinished = false;

    gameState->isQuitRequested = false;
    gameState->isPaused = false;
    gameState->selectedOptionPaused = IntInRange_create(0, 0, 2 - 1);
}

void GameState_free(GameState *gameState)
{
    Map_free(&gameState->map);
    free(gameState->rays);
    EntityArray_free(&(gameState->enemyProjectiles));
    EntityArray_free(&gameState->enemies);
    TextureXAspectRation_free(&(gameState->youDiedText));

    SDL_FreeSurface(gameState->screenSurface);
    SDL_DestroyTexture(gameState->wallsTexture);
    SDL_DestroyTexture(gameState->soldierTexture);
    SDL_DestroyTexture(gameState->gunTexture);
    SDL_DestroyTexture(gameState->frameTexture);
    SDL_DestroyTexture(gameState->vigneteTexture);
    SDL_DestroyTexture(gameState->barTexture);
    SDL_FreeSurface(gameState->wallsSurface);
    SDL_FreeSurface(gameState->soldierSurface);
}

void GameState_event(GameState *gameState, SDL_Event event)
{
    read_keyboard(&event, &gameState->keys);
    if (gameState->isPaused)
    {

        if (getKeyData(&(gameState->keys), SDLK_RETURN).isJustPressed)
        {
            switch (gameState->selectedOptionPaused.value)
            {
            case 0:
                gameState->isPaused = false;
                break;
            case 1:
                gameState->isQuitRequested = true;
                break;
            }
        }
        else if (getKeyData(&(gameState->keys), SDLK_UP).isJustPressed)
        {
            IntInRange_decrement(&(gameState->selectedOptionPaused));
        }
        else if (getKeyData(&(gameState->keys), SDLK_DOWN).isJustPressed)
        {
            IntInRange_increment(&(gameState->selectedOptionPaused));
        }
    }
    else
    {
        if (getKeyData(&(gameState->keys), SDLK_ESCAPE).isJustPressed)
        {
            if (!gameState->player.isDead)
                gameState->isPaused = true;
        }
    }
}

void GameState_updateProjectiles(EntityArray *projectiles, float delta)
{
    for (int i = 0; i < projectiles->size; i++)
    {
        Projectile *p = (Projectile*) EntityArray_get(projectiles, i); 
        p->pos.x = p->pos.x + p->dir.x * delta * 3;
        p->pos.y = p->pos.y + p->dir.y * delta * 3;
        p->circle.p.x = p->pos.x;
        p->circle.p.y = p->pos.y;
        p->lifetime += delta;
        if (p->lifetime >= 10.0f)
        {
            EntityArray_remove(projectiles, i);
            i--;
        }
    }
}

void GameState_draw(GameState *gameState, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 64, 0, 25, 255);
    SDL_RenderClear(renderer);

    //****************************************************GAME_SCREEN************************************
    SDL_Rect SrcR;
    SDL_Rect DestR;

    ScalingData scalingInfo = ScalingData_create(renderer);
    
    //TODO use settings, read from args
    const bool drawFloor = true;
    const bool drawThreaded = true;

    if(!drawFloor){
        SDL_Rect topRect = {0,0,SCREEN_WIDTH, SCREEN_HEIGHT/2};
        SDL_Rect bottomRect = {0,SCREEN_HEIGHT/2,SCREEN_WIDTH, SCREEN_HEIGHT/2};
        SDL_FillRect(gameState->screenSurface, &bottomRect, SDL_MapRGB(gameState->screenSurface->format, 30, 30, 30));
        SDL_FillRect(gameState->screenSurface, &topRect, SDL_MapRGB(gameState->screenSurface->format, 120, 120, 120));
    }

    if (!gameState->isPaused)
    {

        if (!gameState->player.isDead)
        {
            SDL_LockSurface(gameState->screenSurface);

            

///
            if(drawThreaded){
                pthread_t threads[THREAD_COUNT];
                DrawFloorThreadData floor_thread_data[THREAD_COUNT];
                for (int i = 0; i < THREAD_COUNT; i++)
                {
                    floor_thread_data[i].player = &(gameState->player);
                    floor_thread_data[i].screenSurface = gameState->screenSurface;
                    floor_thread_data[i].wallsSurface = gameState->wallsSurface;
                    floor_thread_data[i].startY = i * (SCREEN_HEIGHT / THREAD_COUNT);
                    floor_thread_data[i].endY = (i + 1) * (SCREEN_HEIGHT / THREAD_COUNT);
                }
                for (int i = 0; i < THREAD_COUNT; i++)
                    pthread_create(&threads[i], NULL, drawFloor_threaded, (void *)&floor_thread_data[i]);
                for (int i = 0; i < THREAD_COUNT; i++)
                    pthread_join(threads[i], NULL);

                DrawWallsThreadData walls_thread_data[THREAD_COUNT];
                for (int i = 0; i < THREAD_COUNT; i++)
                {
                    walls_thread_data[i].player = &(gameState->player);
                    walls_thread_data[i].screenSurface = gameState->screenSurface;
                    walls_thread_data[i].wallsSurface = gameState->wallsSurface;
                    walls_thread_data[i].rays = gameState->rays;
                    walls_thread_data[i].zBuffer = gameState->depthBuffer;
                    walls_thread_data[i].startX = i * (SCREEN_WIDTH / THREAD_COUNT);
                    walls_thread_data[i].endX = (i + 1) * (SCREEN_WIDTH / THREAD_COUNT);
                }

                for (int i = 0; i < THREAD_COUNT; i++)
                    pthread_create(&threads[i], NULL, drawWalls_threaded, (void *)&walls_thread_data[i]);
                for (int i = 0; i < THREAD_COUNT; i++)
                    pthread_join(threads[i], NULL);
            }else{
                if(drawFloor)
                    drawFloorNCeiling(gameState->screenSurface, &gameState->player, gameState->wallsSurface, 0, SCREEN_HEIGHT);
                drawWalls(gameState->screenSurface, &gameState->player, gameState->rays, gameState->depthBuffer, gameState->wallsSurface, 0, SCREEN_WIDTH);
            }
            ///

            SDL_UnlockSurface(gameState->screenSurface);
        } //! player is dead

        SrcR.x = 0;
        SrcR.y = 0;
        SrcR.w = SCREEN_WIDTH;
        SrcR.h = SCREEN_HEIGHT;
        DestR.x = GAME_SCREEN_OFFSET_X * scalingInfo.scalingFactor + scalingInfo.offsetX;
        DestR.y = GAME_SCREEN_OFFSET_Y * scalingInfo.scalingFactor + scalingInfo.offsetY;
        DestR.w = SCREEN_WIDTH * scalingInfo.scalingFactor;
        DestR.h = SCREEN_HEIGHT * scalingInfo.scalingFactor;

        SDL_Texture *tTempTexture;

        tTempTexture = SDL_CreateTextureFromSurface(renderer, gameState->screenSurface);
        SDL_RenderCopy(renderer, tTempTexture, &SrcR, &DestR);
        SDL_DestroyTexture(tTempTexture);

        SpriteArray sprites;
        SpriteArray_init(&sprites, gameState->enemies.size + gameState->enemyProjectiles.size); // + projectiles->size...

        for (int i = 0; i < gameState->enemies.size; i++)
        {
            Sprite s = Enemy_getSprite((Enemy*)EntityArray_get(&(gameState->enemies), i));
            s.distanceToPlayerSquared = distanceSquared(s.pos.x, s.pos.y, gameState->player.pos.x, gameState->player.pos.y);
            SpriteArray_add(&sprites, s);
        }

        for (int i = 0; i < gameState->enemyProjectiles.size; i++)
        {
            Sprite s = Projectile_getSprite((Projectile*)EntityArray_get(&(gameState->enemyProjectiles), i));
            s.distanceToPlayerSquared = distanceSquared(s.pos.x, s.pos.y, gameState->player.pos.x, gameState->player.pos.y);
            SpriteArray_add(&sprites, s);
        }

        drawSprites(renderer, &gameState->player, &sprites, gameState->depthBuffer, gameState->soldierTexture, &scalingInfo);

        SpriteArray_free(&sprites);

        drawMap(renderer, &gameState->player, &(gameState->enemies), gameState->rays, SCREEN_WIDTH, &gameState->map);

        drawWeapon(renderer, &gameState->player, gameState->gunTexture, &scalingInfo);

        if (gameState->player.hitAnimOpacity >= 0) // player hit anim
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200 * gameState->player.hitAnimOpacity); // hit anim
            SDL_RenderFillRect(renderer, &DestR);                                               // hit anim
        }
    }

    if (gameState->player.isDead) // death animation and deah screen
    {
        SDL_Texture *tTempTexture;
        tTempTexture = SDL_CreateTextureFromSurface(renderer, gameState->deathScreenSurface);
        SDL_RenderCopy(renderer, tTempTexture, &SrcR, &DestR);
        SDL_DestroyTexture(tTempTexture);

        if (gameState->isDeathAnimFinished)
        {
            int textWidth = (SCREEN_WIDTH * scalingInfo.scalingFactor) / 3;
            int textHeight = (int)((float)textWidth / gameState->youDiedText.aspectRatio);

            DestR.x = GAME_SCREEN_OFFSET_X * scalingInfo.scalingFactor + scalingInfo.offsetX + SCREEN_WIDTH * scalingInfo.scalingFactor * 0.5f - textWidth / 2;
            DestR.y = GAME_SCREEN_OFFSET_Y * scalingInfo.scalingFactor + scalingInfo.offsetY + SCREEN_HEIGHT * scalingInfo.scalingFactor * 0.5f - textHeight / 2;
            DestR.w = textWidth;
            DestR.h = textHeight;
            SDL_RenderCopy(renderer, gameState->youDiedText.texture, NULL, &DestR);
        }
    }

    //**********************FRAME******************
    SrcR.x = 0;
    SrcR.y = 0;
    SrcR.w = FRAME_WIDTH;
    SrcR.h = FRAME_HEIGHT;
    DestR.x = scalingInfo.offsetX;
    DestR.y = scalingInfo.offsetY;
    DestR.w = FRAME_WIDTH * scalingInfo.scalingFactor;
    DestR.h = FRAME_HEIGHT * scalingInfo.scalingFactor;
    SDL_RenderCopy(renderer, gameState->frameTexture, &SrcR, &DestR);

    if (gameState->isPaused)
        drawPausedBar(renderer, gameState->font, &scalingInfo, gameState->selectedOptionPaused.value);
    else
        drawBar(renderer, &(gameState->player), gameState->barTexture, &scalingInfo);

    //*************************************RECTANGLES TO HIDE SPRITES OUT OF BOUNDS
    DestR.x = 0;
    DestR.y = 0;
    DestR.w = scalingInfo.rendererWidth;
    DestR.h = scalingInfo.offsetY + 1;
    
    SDL_SetRenderDrawColor(renderer, 64, 0, 25, 255);

    SDL_RenderFillRect(renderer, &DestR);//top
    DestR.y = scalingInfo.offsetY + (FRAME_HEIGHT * scalingInfo.scalingFactor);
    
    SDL_RenderFillRect(renderer, &DestR);//bottom

    DestR.w = scalingInfo.offsetX;
    DestR.y = scalingInfo.offsetY;
    DestR.h = FRAME_HEIGHT * scalingInfo.scalingFactor;
    SDL_RenderFillRect(renderer, &DestR);//left
}

void GameState_update(GameState *gameState, float delta)
{
    if (gameState->isPaused)
    {
        KeysArray_update(&(gameState->keys)); // call after done using keys in the iteration
        return;
    }

    if (!gameState->player.isDead)
    {
        Player_update(&gameState->player, &gameState->keys, delta);

        resCollPlayerXMap(&gameState->player, &gameState->map);

        GameState_updateEnemies(&(gameState->enemies), &(gameState->enemyProjectiles), &(gameState->player), delta);

        GameState_playerAttackEnemies(&(gameState->enemies), &(gameState->player), &(gameState->map));

        GameState_updateProjectiles(&(gameState->enemyProjectiles), delta);

        resCollEnemyArrayXMap(&(gameState->enemies), &gameState->map);

        resCollProjectileArrayXMap(&(gameState->enemyProjectiles), &(gameState->map));

        resCollPlayerXProjectileArray(&(gameState->player), &(gameState->enemyProjectiles));

        castRays(&gameState->player, gameState->rays, &gameState->map);
    }
    else
    {
        if (!gameState->isDeathAnimFinished)
        {
            fizzlefade(gameState, delta); 
        }
        else
        { // death anim finished
            if (gameState->keys.isAnyKeyPressed)
            {
                gameState->isQuitRequested = true;
                return;
            }
        }
    }

    KeysArray_update(&(gameState->keys)); // call after done using keys in the iteration
}

void GameState_updateEnemies(EntityArray *enemies, EntityArray *enemy_projectiles, Player *player, float delta)
{
    for (int i = 0; i < enemies->size; i++)
    {
        Enemy *e = (Enemy*)EntityArray_get(enemies, i);

        // MOVE ENEMY
        Vector2f dirToPlayer = Vector2f_sub(player->pos, e->pos);
        dirToPlayer = Vector2f_norm(dirToPlayer);
        const float speed = 0.75f;
        e->pos = Vector2f_add(e->pos, Vector2f_multScalar(dirToPlayer, delta * speed));
        e->circle.p.x = e->pos.x;
        e->circle.p.y = e->pos.y;
        //

        // SHOOT ENEMY
        e->attackTimeCounter += delta;
        if (e->attackTimeCounter >= ENEMY_SHOOT_DELAY)
        {
            Projectile p = Projectile_create(e->pos, Vector2f_norm(Vector2f_sub(player->pos, e->pos)), 0.1f);
            EntityArray_add(enemy_projectiles, &p);
            e->attackTimeCounter = 0.0f;
        } //

        // ANIMATE ENEMY
        if (e->isHitAnimPlaying)
        {
            e->hitAnimTimeCounter += delta;
            if (e->hitAnimTimeCounter >= ENEMY_HIT_ANIM_TIME)
                e->isHitAnimPlaying = false;
        }
        //
    }
}

void GameState_playerAttackEnemies(EntityArray *enemies, Player *player, Map *map)
{
    if (player->isJustAttacked)
    {
        switch (player->selectedWeapon)
        {
        case WEAPON_HANDGUN:
            GameState_playerAttackEnemiesRaycast(enemies, player, map, 420.0f);
            break;
        case WEAPON_KNIFE:
            GameState_playerAttackEnemiesRaycast(enemies, player, map, KNIFE_REACH);
            break;
        case WEAPON_LAUNCHER:
            // instantiate projectile
            break;
        }
    }
}

void GameState_playerAttackEnemiesRaycast(EntityArray *enemies, Player *player, Map *map, float maxDistance)
{
    Ray ray = castOneRay(player->pos, player->dir, map);

    for (int i = 0; i < enemies->size; i++)
    {
        Enemy *e = (Enemy*)EntityArray_get(enemies, i);
        float dist = rayPointDistSquared(player->pos, player->dir, e->pos);
        if (dist <= e->width * 0.5f * e->width * 0.5f)
        {
            float squareDistToEnemy = distanceSquared(e->pos.x, e->pos.y, player->pos.x, player->pos.y);
            if (ray.isHit)
            {
                if (ray.correctedDistance * ray.correctedDistance < squareDistToEnemy)
                    continue; // wall hit is closer than enemy
            }
            if (squareDistToEnemy > maxDistance * maxDistance)
                continue; // enemy is too far, out of weapons reach

            Vector2f vToEnemy = Vector2f_sub(e->pos, player->pos);

            float dot = Vector2f_dot(vToEnemy, player->dir);
            if (dot < 0.0f)
                continue; // enemy is behind player

            Enemy_takeDamage(e);
            // printf("hit");
        }
    }
}
