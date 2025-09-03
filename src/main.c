#include "raylib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCALE 0.75f
#define THUMBNAIL_SCALE 0.25f
#define THUMBNAIL_SPACING 10

typedef struct {
  char **files;
  int count;
} PhotoList;

typedef struct {
  Texture2D *textures;
  int count;
} ThumbnailList;

void setup_window();
bool is_photo_file(const char *filename);
PhotoList load_photo_list(const char *folder_name);
void free_photo_list(PhotoList *list);
ThumbnailList load_thumbnails(PhotoList photos);
void free_thumbnails(ThumbnailList *thumbs);
void draw_selected_photo(Texture2D tex);
void draw_selected_photo_list(ThumbnailList thumbs, int current_photo);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <folder>\n", argv[0]);
    return 1;
  }

  PhotoList photos = load_photo_list(argv[1]);
  if (photos.count == 0) {
    fprintf(stderr, "No photos found in folder: %s\n", argv[1]);
    return 1;
  }

  setup_window();

  int current_photo = 0;

  Image image = LoadImage(photos.files[current_photo]);
  Texture2D texture = LoadTextureFromImage(image);
  UnloadImage(image);

  ThumbnailList thumbs = load_thumbnails(photos);

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_RIGHT) && current_photo < photos.count - 1) {
      UnloadTexture(texture);
      current_photo++;
      image = LoadImage(photos.files[current_photo]);
      texture = LoadTextureFromImage(image);
      UnloadImage(image);
    }

    if (IsKeyPressed(KEY_LEFT) && current_photo > 0) {
      UnloadTexture(texture);
      current_photo--;
      image = LoadImage(photos.files[current_photo]);
      texture = LoadTextureFromImage(image);
      UnloadImage(image);
    }

    BeginDrawing();
    {
      ClearBackground(BLACK);
      draw_selected_photo(texture);
      draw_selected_photo_list(thumbs, current_photo);
    }
    EndDrawing();
  }

  UnloadTexture(texture);
  free_thumbnails(&thumbs);
  free_photo_list(&photos);
  CloseWindow();
  return 0;
}

void setup_window() {
  InitWindow(1920, 1080, "PicPickr");
  int current_monitor = GetCurrentMonitor();
  int refresh_rate = GetMonitorRefreshRate(current_monitor);

  if (refresh_rate <= 0) {
    refresh_rate = 60;
  }

  SetTargetFPS(refresh_rate);
}

bool is_photo_file(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return false;

  return (strcasecmp(dot, ".jpg") == 0 || strcasecmp(dot, ".jpeg") == 0 ||
          strcasecmp(dot, ".png") == 0 || strcasecmp(dot, ".raw") == 0 ||
          strcasecmp(dot, ".bmp") == 0 || strcasecmp(dot, ".tiff") == 0);
}

PhotoList load_photo_list(const char *folder_name) {
  PhotoList list = {0};
  DIR *dp = opendir(folder_name);
  struct dirent *entry;

  if (dp == NULL) {
    perror("opendir");
    return list;
  }

  while ((entry = readdir(dp))) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    if (is_photo_file(entry->d_name)) {
      char path[PATH_MAX];
      snprintf(path, sizeof(path), "%s/%s", folder_name, entry->d_name);

      list.files = realloc(list.files, sizeof(char *) * (list.count + 1));
      list.files[list.count] = strdup(path);
      list.count++;
    }
  }

  closedir(dp);
  return list;
}

void free_photo_list(PhotoList *list) {
  for (int i = 0; i < list->count; i++) {
    free(list->files[i]);
  }
  free(list->files);
  list->files = NULL;
  list->count = 0;
}

ThumbnailList load_thumbnails(PhotoList photos) {
  ThumbnailList thumbs = {0};
  thumbs.count = photos.count;
  thumbs.textures = malloc(sizeof(Texture2D) * photos.count);

  for (int i = 0; i < photos.count; i++) {
    Image img = LoadImage(photos.files[i]);
    ImageResize(&img, img.width * THUMBNAIL_SCALE,
                img.height * THUMBNAIL_SCALE);
    thumbs.textures[i] = LoadTextureFromImage(img);
    UnloadImage(img);
  }

  return thumbs;
}

void free_thumbnails(ThumbnailList *thumbs) {
  for (int i = 0; i < thumbs->count; i++) {
    UnloadTexture(thumbs->textures[i]);
  }
  free(thumbs->textures);
  thumbs->textures = NULL;
  thumbs->count = 0;
}

void draw_selected_photo(Texture2D tex) {
  DrawTextureEx(tex, (Vector2){0, GetScreenHeight() - tex.height * SCALE}, 0.0f,
                SCALE, WHITE);
}

void draw_selected_photo_list(ThumbnailList thumbs, int current_photo) {
  int pos_x = 20;
  int pos_y = 40;

  for (int i = 0; i < thumbs.count; i++) {
    Color tint = (i == current_photo) ? YELLOW : WHITE;
    DrawTexture(thumbs.textures[i], pos_x, pos_y, tint);

    pos_x += thumbs.textures[i].width + THUMBNAIL_SPACING;
  }
}
