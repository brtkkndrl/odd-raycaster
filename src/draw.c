#include "draw.h"
#include "drawRaycast.h"

void *drawFloor_threaded(void *vargp)
{
    DrawFloorThreadData *data = (DrawFloorThreadData *)vargp;
    drawFloorNCeiling(data->screenSurface, data->player, data->wallsSurface, data->startY, data->endY);
    return NULL;
}

void *drawWalls_threaded(void *vargp)
{
    DrawWallsThreadData *data = (DrawWallsThreadData *)vargp;
    drawWalls(data->screenSurface, data->player, data->rays, data->zBuffer, data->wallsSurface, data->startX, data->endX);
    return NULL;
}

void drawBar(SDL_Renderer *renderer, Player *player, SDL_Texture *bar_texture, ScalingData *scalingInfo)
{
    SDL_Rect SrcR;
    SDL_Rect DestR;
    SrcR.x = 0;
    SrcR.y = 32 + 1;
    SrcR.w = 32;
    SrcR.h = 32;
    DestR.x = scalingInfo->offsetX + (HEARTS_SLOT_X)*scalingInfo->scalingFactor;
    DestR.y = scalingInfo->offsetY + (HEARTS_SLOT_Y)*scalingInfo->scalingFactor;
    DestR.w = (32) * scalingInfo->scalingFactor;
    DestR.h = (32) * scalingInfo->scalingFactor;

    // HEARTS
    for (int i = 0; i < player->health / 2; i++)
    {
        DestR.x = scalingInfo->offsetX + (HEARTS_SLOT_X + (32) * i) * scalingInfo->scalingFactor;
        SDL_RenderCopy(renderer, bar_texture, &SrcR, &DestR);
    }

    if (player->health % 2 == 1)
    {
        SrcR.x = 32 + 1;
        DestR.x = scalingInfo->offsetX + (HEARTS_SLOT_X + (32) * (player->health / 2)) * scalingInfo->scalingFactor;
        SDL_RenderCopy(renderer, bar_texture, &SrcR, &DestR);
    }

    // HEAD
    SrcR.x = ((PLAYER_MAX_HEALTH - player->health) / 2) * (32 + 1);
    SrcR.y = 0;
    DestR.x = scalingInfo->offsetX + (HEAD_SLOT_X)*scalingInfo->scalingFactor;
    DestR.y = scalingInfo->offsetY + (HEAD_SLOT_Y)*scalingInfo->scalingFactor;
    SDL_RenderCopy(renderer, bar_texture, &SrcR, &DestR);

    // weapon
    SrcR.x = (64 + 1) * player->selectedWeapon;
    SrcR.w = 64;
    SrcR.y = (32 + 1) * 2;
    DestR.x = scalingInfo->offsetX + (WEAPON_SLOT_X)*scalingInfo->scalingFactor;
    DestR.y = scalingInfo->offsetY + (WEAPON_SLOT_Y)*scalingInfo->scalingFactor;
    DestR.w = 64 * scalingInfo->scalingFactor;
    SDL_RenderCopy(renderer, bar_texture, &SrcR, &DestR);
}

void drawText(SDL_Renderer *renderer, SDL_Texture *texture, int sx, int sy, char *text)
{
    SDL_Rect SrcR;
    SDL_Rect DestR;

    for (int i = 0; text[i] != '\0'; i++)
    {
        int n = text[i] - 'A';
        SrcR.x = n * 3 + n;
        SrcR.y = 0;
        SrcR.w = 3;
        SrcR.h = 5;
        DestR.x = sx + i * 12;
        DestR.y = sy;
        DestR.w = 9;
        DestR.h = 15;
        SDL_RenderCopy(renderer, texture, &SrcR, &DestR);
    }
}


void drawMap(SDL_Renderer *renderer, Player *player, EntityArray *enemies, Ray *rays, int iRayCount, Map *map)
{
    //TODO render to a map texture
    const int BLOCK_SIZE = 15;
    const int FRAME_SIZE = 1;
    const SDL_Color WALL_COLOR = {0, 0, 255};
    const SDL_Color FRAME_COLOR = {255, 255, 0};

    const int texWidth = map->width * BLOCK_SIZE + FRAME_SIZE * 2;
    const int texHeight = map->height * BLOCK_SIZE + FRAME_SIZE * 2;

    SDL_Texture* mapTex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 10, 10);

    SDL_SetRenderTarget(renderer, mapTex);//Render to texture

    SDL_Rect squareRect;
    squareRect.x = 0;
    squareRect.y = 0;
    squareRect.w = map->width * BLOCK_SIZE + FRAME_SIZE * 2;
    squareRect.h = map->height * BLOCK_SIZE + FRAME_SIZE * 2;

    SDL_SetRenderDrawColor(renderer, FRAME_COLOR.r, FRAME_COLOR.g, FRAME_COLOR.b, 255);
    SDL_RenderFillRect(renderer, NULL);

    SDL_SetRenderTarget(renderer, NULL);//Render to screen


    SDL_Rect texDstRect = {
        30, 30, texWidth, texHeight
    };
    SDL_RenderCopy(renderer, mapTex, NULL, &texDstRect);

    SDL_DestroyTexture(mapTex);

    return;

    squareRect.x = FRAME_SIZE;
    squareRect.y = FRAME_SIZE;
    squareRect.w = map->width * BLOCK_SIZE;
    squareRect.h = map->height * BLOCK_SIZE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &squareRect);

    squareRect.w = BLOCK_SIZE;
    squareRect.h = BLOCK_SIZE;

    SDL_SetRenderDrawColor(renderer, WALL_COLOR.r, WALL_COLOR.g, WALL_COLOR.b, 255);

    for (int x = 0; x < map->width; x++)
    {
        for (int y = 0; y < map->height; y++)
        {
            if (map->arr[y][x].blockType == '#')
                continue;
            if (map->arr[y][x].blockType == 'E')
            {
                squareRect.x = FRAME_SIZE + x * BLOCK_SIZE;
                squareRect.y = FRAME_SIZE + y * BLOCK_SIZE;
                SDL_RenderFillRect(renderer, &squareRect);
            }
            else
            {
                SDL_Vertex verts[3];
                if (map->arr[y][x].blockType == 'A')
                {
                    verts[0].position = (SDL_FPoint){x, y + 1};
                    verts[1].position = (SDL_FPoint){x + 1, y};
                    verts[2].position = (SDL_FPoint){x + 1, y + 1};
                }
                else if (map->arr[y][x].blockType == 'B')
                {
                    verts[0].position = (SDL_FPoint){x, y};
                    verts[1].position = (SDL_FPoint){x + 1, y + 1};
                    verts[2].position = (SDL_FPoint){x, y + 1};
                }
                else if (map->arr[y][x].blockType == 'C')
                {
                    verts[0].position = (SDL_FPoint){x, y};
                    verts[1].position = (SDL_FPoint){x + 1, y};
                    verts[2].position = (SDL_FPoint){x + 1, y + 1};
                }
                else if (map->arr[y][x].blockType == 'D')
                {
                    verts[0].position = (SDL_FPoint){x, y};
                    verts[1].position = (SDL_FPoint){x + 1, y};
                    verts[2].position = (SDL_FPoint){x, y + 1};
                }
                verts[0].color = (SDL_Color){0, 0, 255, 255};
                verts[1].color = (SDL_Color){0, 0, 255, 255};
                verts[2].color = (SDL_Color){0, 0, 255, 255};
                verts[0].position.x = verts[0].position.x * BLOCK_SIZE + FRAME_SIZE;
                verts[0].position.y = verts[0].position.y * BLOCK_SIZE + FRAME_SIZE;
                verts[1].position.x = verts[1].position.x * BLOCK_SIZE + FRAME_SIZE;
                verts[1].position.y = verts[1].position.y * BLOCK_SIZE + FRAME_SIZE;
                verts[2].position.x = verts[2].position.x * BLOCK_SIZE + FRAME_SIZE;
                verts[2].position.y = verts[2].position.y * BLOCK_SIZE + FRAME_SIZE;
                SDL_RenderGeometry(renderer, NULL, verts, 3, NULL, 0);
            }
        }
    }

    SDL_Vertex vert[iRayCount + 1];
    int indicies[(iRayCount - 1) * 3];
    unsigned iIndiciesIndex = 0;

    vert[0].position.x = player->pos.x * BLOCK_SIZE + FRAME_SIZE;
    vert[0].position.y = player->pos.y * BLOCK_SIZE + FRAME_SIZE;
    vert[0].color.r = 255;
    vert[0].color.g = 0;
    vert[0].color.b = 255;
    vert[0].color.a = 255;

    for (int i = 1; i < iRayCount; i++)
    {
        indicies[iIndiciesIndex++] = 0;
        indicies[iIndiciesIndex++] = i;
        indicies[iIndiciesIndex++] = i + 1;
    }

    for (int i = 1; i <= iRayCount; i++)
    {
        vert[i].position.x = rays[i - 1].hitPos.x * BLOCK_SIZE + FRAME_SIZE;
        vert[i].position.y = rays[i - 1].hitPos.y * BLOCK_SIZE + FRAME_SIZE;
        vert[i].color.r = 255;
        vert[i].color.g = 0;
        vert[i].color.b = 255;
        vert[i].color.a = 255;
    }

    SDL_RenderGeometry(renderer, NULL, vert, iRayCount + 1, indicies, (iRayCount - 1) * 3);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    Vector2f A = Vector2f_add(player->pos, player->dir);
    SDL_RenderDrawLine(renderer,
                    player->pos.x * BLOCK_SIZE + FRAME_SIZE,
                    player->pos.y * BLOCK_SIZE + FRAME_SIZE,
                    A.x * BLOCK_SIZE + FRAME_SIZE,
                    A.y * BLOCK_SIZE + FRAME_SIZE);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    Vector2f B = Vector2f_add(A, player->plane);
    Vector2f C = Vector2f_sub(A, player->plane);
    SDL_RenderDrawLine(renderer,
                    B.x * BLOCK_SIZE + FRAME_SIZE,
                    B.y * BLOCK_SIZE + FRAME_SIZE,
                    C.x * BLOCK_SIZE + FRAME_SIZE,
                    C.y * BLOCK_SIZE + FRAME_SIZE);

    // draw entities
    for (int i = 0; i < enemies->size; i++)
    {
        Enemy* e = (Enemy*)EntityArray_get(enemies, i);
        squareRect.x = e->pos.x * BLOCK_SIZE;
        squareRect.y = e->pos.y * BLOCK_SIZE;
        squareRect.w = BLOCK_SIZE / 2;
        squareRect.h = BLOCK_SIZE / 2;
        // printf("%f %f\n", enemies->enemies[i].pos.x, enemies->enemies[i].pos.y);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &squareRect);
    }
}


void drawWeapon(SDL_Renderer *renderer, Player *player, SDL_Texture *gun_texture, ScalingData *scalingInfo)
{
    SDL_Rect SrcR;
    SDL_Rect DestR;

    SrcR.x = player->gunFrame * 64 + player->gunFrame;
    SrcR.y = player->selectedWeapon * (64 + 1);
    SrcR.w = 64;
    SrcR.h = 64;
    DestR.x = scalingInfo->offsetX + (SCREEN_WIDTH / 2 - 128 / 2 + GAME_SCREEN_OFFSET_X) * scalingInfo->scalingFactor;
    DestR.y = scalingInfo->offsetY + (SCREEN_HEIGHT - 128 + GAME_SCREEN_OFFSET_Y) * scalingInfo->scalingFactor;
    DestR.w = 128 * scalingInfo->scalingFactor;
    DestR.h = 128 * scalingInfo->scalingFactor;
    SDL_RenderCopy(renderer, gun_texture, &SrcR, &DestR);
}

ScalingData ScalingData_create(SDL_Renderer *renderer)
{
    int rendererWidth, rendererHeight;
    SDL_GetRendererOutputSize(renderer, &rendererWidth, &rendererHeight);
    ScalingData scalingInfo;
    float scalingFactorX = (float)rendererWidth / (FRAME_WIDTH);
    float scalingFactorY = (float)rendererHeight / (FRAME_HEIGHT);
    scalingInfo.scalingFactor = scalingFactorX < scalingFactorY ? (int)floor(scalingFactorX) : (int)floor(scalingFactorY);
    scalingInfo.offsetX = (rendererWidth - FRAME_WIDTH * scalingInfo.scalingFactor) / 2;
    scalingInfo.offsetY = (rendererHeight - FRAME_HEIGHT * scalingInfo.scalingFactor) / 2;
    scalingInfo.rendererHeight = rendererHeight;
    scalingInfo.rendererWidth = rendererWidth;
    return scalingInfo;
}

void TextureXAspectRation_free(TextureXAspectRatio *tt)
{
    SDL_DestroyTexture(tt->texture);
}

void drawPausedBar(SDL_Renderer *renderer, TTF_Font *font, ScalingData *scalingInfo, int selectedOption)
{
    SDL_Rect rect;
    // draw bottom background
    rect.x = scalingInfo->offsetX + BAR_SCREEN_OFFSET_X * scalingInfo->scalingFactor;
    rect.y = scalingInfo->offsetY + BAR_SCREEN_OFFSET_Y * scalingInfo->scalingFactor;
    rect.w = BAR_SCREEN_WIDTH * scalingInfo->scalingFactor;
    rect.h = BAR_SCREEN_HEIGHT * scalingInfo->scalingFactor;
    SDL_SetRenderDrawColor(renderer, 0, 25, 64, 255);
    SDL_RenderFillRect(renderer, &rect);
    // draw bottom text
    TextureXAspectRatio textPaused = TextureXAspectRation_create(renderer, font, "--PAUSED--", (SDL_Color){255, 225, 191});
    rect.w = textPaused.aspectRatio * rect.h;
    rect.x = scalingInfo->offsetX + BAR_SCREEN_OFFSET_X * scalingInfo->scalingFactor + BAR_SCREEN_WIDTH * scalingInfo->scalingFactor / 2 - rect.w / 2;
    SDL_RenderCopy(renderer, textPaused.texture, NULL, &rect);
    TextureXAspectRation_free(&textPaused);

    // draw top background
    rect.x = scalingInfo->offsetX + GAME_SCREEN_OFFSET_X * scalingInfo->scalingFactor;
    rect.y = scalingInfo->offsetY + GAME_SCREEN_OFFSET_Y * scalingInfo->scalingFactor;
    rect.w = SCREEN_WIDTH * scalingInfo->scalingFactor;
    rect.h = SCREEN_HEIGHT * scalingInfo->scalingFactor;
    SDL_RenderFillRect(renderer, &rect);

// draw top text
#define OPTIONS_COUNT 2
#define TEXT_MARGIN_RATIO 2
    const static char *options[OPTIONS_COUNT] = {"RESUME", "MENU"};
    const static char *optionsSelected[OPTIONS_COUNT] = {">RESUME<", ">MENU<"};

    const int textHeight = SCREEN_HEIGHT / 4;

    for (int i = 0; i < OPTIONS_COUNT; i++)
    {
        const char *text = i == selectedOption ? optionsSelected[i] : options[i];

        TextureXAspectRatio textTexture = TextureXAspectRation_create(renderer, font, text, (SDL_Color){255, 225, 191});

        int textWidth = textHeight * textTexture.aspectRatio;

        rect.x = scalingInfo->rendererWidth / 2 - textWidth / 2;
        rect.y = scalingInfo->rendererHeight / 2 + i * textHeight * TEXT_MARGIN_RATIO - (OPTIONS_COUNT * textHeight * TEXT_MARGIN_RATIO) / 2;
        rect.w = textWidth;
        rect.h = textHeight;

        SDL_RenderCopy(renderer, textTexture.texture, NULL, &rect);
        TextureXAspectRation_free(&textTexture);
    }
#undef OPTIONS_COUNT
#undef TEXT_MARGIN_RATIO
}

TextureXAspectRatio TextureXAspectRation_create(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color)
{
    TextureXAspectRatio tt;
    SDL_Surface *surf = TTF_RenderText_Solid(font, text, color);
    tt.texture = SDL_CreateTextureFromSurface(renderer, surf);
    tt.aspectRatio = surf->w / surf->h;
    SDL_FreeSurface(surf);
    return tt;
}