#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 160
#define HEIGHT 44

typedef struct {
  float x;
  float y;
  float z;
} Point;

Point calculate_rotation(Point point, float ax, float ay, float az) {
  float sinX = sin(ax);
  float cosX = cos(ax);
  float sinY = sin(ay);
  float cosY = cos(ay);
  float sinZ = sin(az);
  float cosZ = cos(az);

  Point result;

  result.x = point.x * (cosY * cosZ)
           + point.y * (sinX * sinY * cosZ - cosX * sinZ)
           + point.z * (cosX * sinY * cosZ + sinX * sinZ);

  result.y = point.x * (cosY * sinZ)
           + point.y * (sinX * sinY * sinZ + cosX * cosZ)
           + point.z * (cosX * sinY * sinZ - sinX * cosZ);

  result.z = point.x * (-sinY)
           + point.y * (sinX * cosY)
           + point.z * (cosX * cosY);

  return result;
}

typedef struct {
  char character;
  float distance;
} Pixel;

Pixel z_buffer[WIDTH * HEIGHT];

void render_cube(Point position, int width, char character) {
  float cube_width = width / 2.0f;

  for (float x = -cube_width; x < cube_width; x += 1) {
    for (float y = -cube_width; y < cube_width; y += 1) {
      Point point;

      point.x = x + position.x;
      point.y = y + position.y;
      point.z = position.z;

      point = calculate_rotation(point, A, B, C);

      if (point.z <= 0) {
        continue;
      }

      float ooz = 1 / point.z;

      int xp = (int)(WIDTH / 2 + K1 * ooz * point.x);
      int yp = (int)(HEIGHT / 2 + K1 * ooz * point.y);

      if (xp < 0 || xp >= WIDTH || yp < 0 || yp >= HEIGHT) {
        continue;
      }

      int index = xp + yp * WIDTH;

      if (ooz > z_buffer[index].distance) {
        z_buffer[index].distance = ooz;
        z_buffer[index].character = character;
      }
    }
  }
}

void clear_screen(char background_character) {
  memset(z_buffer, 0, WIDTH * HEIGHT * sizeof(Pixel));

  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    z_buffer[i].character = background_character;
  }
}

void render_screen() {
  printf("\x1b[H");

  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    putchar(z_buffer[i].character);

    if ((i + 1) % WIDTH == 0) {
      putchar('\n');
    }
  }
}

int main() {
  float increment_speed = 0.5f;

  A = 0.0f;
  B = 0.0f;
  C = 0.0f;

  while (1) {
    clear_screen('.');

    Point positions[] = {
        { -40, 0, -100 },
        { 0, 0, -100 },
        { 40, 0, -100 },
    };

    char characters[] = { '@', '$', '~' };

    int widths[] = { 20, 10, 5 };

    for (int i = 0; i < 3; i++) {
      render_cube(positions[i], widths[i], characters[i]);
    }

    render_screen();

    A += 0.05f;
    B += 0.05f;
    C += 0.01f;

    usleep(8000);
  }

  return 0;
}
