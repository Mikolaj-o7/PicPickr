#include "raylib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char **files;
  int count;
} PhotoList;

void _app_setup();
bool _is_file_photo(const char *filename);
PhotoList _load_photo_list(const char *folder_name);
void _free_photo_list(PhotoList *list);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <folder>\n", argv[0]);
    return 1;
  }

  PhotoList photos = _load_photo_list(argv[1]);
  _app_setup();

  int currentPhoto = 0;
  Image photo = LoadImage(photos.files[currentPhoto]);
  Texture2D tex = LoadTextureFromImage(photo);
  UnloadImage(photo);

  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(BLACK);
      DrawTexture(tex, (GetScreenWidth() - tex.width) / 2,
                  (GetScreenHeight() - tex.height) / 2, WHITE);
    }
    EndDrawing();
  }

  _free_photo_list(&photos);
  return 0;
}

void _app_setup() {
  InitWindow(1920, 1080, "PicPickr");
  int currentMonitor = GetCurrentMonitor();
  int refreshRate = GetMonitorRefreshRate(currentMonitor);

  if (refreshRate <= 0) {
    refreshRate = 60;
  }

  SetTargetFPS(refreshRate);
}

bool _is_file_photo(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return false;

  if (strcasecmp(dot, ".jpg") == 0)
    return true;
  if (strcasecmp(dot, ".jpeg") == 0)
    return true;
  if (strcasecmp(dot, ".png") == 0)
    return true;
  if (strcasecmp(dot, ".raw") == 0)
    return true;
  if (strcasecmp(dot, ".bmp") == 0)
    return true;
  if (strcasecmp(dot, ".tiff") == 0)
    return true;

  return false;
}

PhotoList _load_photo_list(const char *folder_name) {
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
    if (_is_file_photo(entry->d_name)) {
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

void _free_photo_list(PhotoList *list) {
  for (int i = 0; i < list->count; i++) {
    free(list->files[i]);
  }
  free(list->files);
  list->files = NULL;
  list->count = 0;
}
