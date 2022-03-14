#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <math.h>

#include "bmp_header.h"

void save(bmp out, char * path) {
  FILE * bmpFile = fopen(path, "wb");

  if (bmpFile == NULL) {
    fprintf(stderr, "ERROR: Can't open file %s", path);
    return;
  }
  fwrite( & out.header, sizeof(bmp_fileheader), 1, bmpFile);
  fwrite( & out.info, sizeof(bmp_infoheader), 1, bmpFile);

  signed int width = out.info.width;
  signed int height = out.info.height;

  for (int i = 0; i < height; i++) {
    int pixelOffset = i * out.unpaddedRowsize;
    fwrite(& out.bitmap[pixelOffset], 1, out.paddedRowSize, bmpFile);
  }
  fclose(bmpFile);
}

void ReadBitmap(FILE * bmpFile, bmp * img) {
  int width = img -> info.width;
  int height = img -> info.height;
  int bytesPerPixel = img -> info.bitPix / BYTE;
  img -> paddedRowSize = (int)(MULTIPLE_OF4 * ceil((float) width / MULTIPLE_OF4FLOAT)) * bytesPerPixel;
  img -> unpaddedRowsize = width * bytesPerPixel;
  img -> bitmap = (unsigned char *) malloc(img -> unpaddedRowsize * height);
  unsigned char * currentRowPointer = img -> bitmap;
  for (int i = 0; i < height; i++) {
    fseek(bmpFile, img -> header.imageDataOffset + (i * img -> paddedRowSize), SEEK_SET);
    fread(currentRowPointer, 1, img -> unpaddedRowsize, bmpFile);
    currentRowPointer += img -> unpaddedRowsize;
  }
}

bmp edit(char * path) {
  bmp edit_bmp;

  FILE * bmpFile = fopen(path, "rb");

  if (bmpFile == NULL) {
    fprintf(stderr, "ERROR: Can't open file %s", path);
    return edit_bmp;
  }

  fread( & edit_bmp.header, sizeof(bmp_fileheader), 1, bmpFile);
  fread( & edit_bmp.info, sizeof(bmp_infoheader), 1, bmpFile);

  ReadBitmap(bmpFile, & edit_bmp);

  fclose(bmpFile);
  return edit_bmp;
}

void InsertBitmap(FILE * bmpFile, bmp * img, bmp * insert, int y, int x) {
  int width = img -> info.width;
  int height = img -> info.height;
  int bytesPerPixel = img -> info.bitPix / BYTE;

  int width_insert = insert -> info.width;
  int height_insert = insert -> info.height;

  if ((insert -> unpaddedRowsize) > (img -> unpaddedRowsize - (x * bytesPerPixel))) {
    insert -> unpaddedRowsize = (img -> unpaddedRowsize - (x * bytesPerPixel));
  }

  if (height_insert > height - y) {
    height_insert = height - y;
  }

  unsigned char * currentRowPointer = img -> bitmap + ((y * img -> unpaddedRowsize) + (x * bytesPerPixel));
  for (int i = 0; i < height_insert; i++) {
    fseek(bmpFile, insert -> header.imageDataOffset + (i * insert -> paddedRowSize), SEEK_SET);
    fread(currentRowPointer, 1, insert -> unpaddedRowsize, bmpFile);
    currentRowPointer += img -> unpaddedRowsize;
  }
}

bmp insert(bmp edit_bmp, char * path) {
  bmp insert_bmp = edit(path);

  FILE * bmpFile = fopen(path, "rb");

  if (bmpFile == NULL) {
    fprintf(stderr, "ERROR: Can't open file %s", path);
    return edit_bmp;
  }
  int height = 0;
  int width = 0;

  scanf("%d", & width);
  scanf("%d", & height);

  InsertBitmap(bmpFile, & edit_bmp, & insert_bmp, height, width);

  fclose(bmpFile);
  return edit_bmp;
}

int main() {
  char * command = malloc(sizeof(char) * command_lenght);
  char * path = malloc(sizeof(char) * command_lenght);

  bmp in;
  scanf("%s", command);
  while (strcmp(command, "quit")) {
    if (!strcmp(command, "edit")) {
      scanf("%s", path); in = edit(path);
    } else if (!strcmp(command, "save")) {
      scanf("%s", path);
      save(in , path);
    } else if (!strcmp(command, "insert")) {
      scanf("%s", path); in = insert(in , path);
    }

    scanf("%s", command);
  }

  // free memory
  free(command);
  free(path);
  free(in .bitmap);

  return 0;
}
