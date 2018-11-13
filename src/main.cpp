#include <stdio.h>
#include <stdlib.h>

#include "ray.h"

internal
u32 GetPixelSize(image_u32 Image) {
    return sizeof(u32) * Image.Width * Image.Height;
}

internal
image_u32 AllocateImage(u32 Width, u32 Height) {
    u32 OutputPixelsSize = sizeof(u32) * Width * Height;
    u32 *OutputPixels = (u32 *)malloc(OutputPixelsSize);
    return image_u32{Width, Height, OutputPixels};
}

internal
void WriteImage(image_u32 Image,  char *Filename) {
    u32 OutputPixelsSize = GetPixelSize(Image);
    bitmap_header Header = {};
    Header.FileType = 0x4d42;
    Header.FileSize = OutputPixelsSize + sizeof(Header);
    Header.BitmapOffset = sizeof(Header);
    Header.Size = sizeof(Header) - 14;
    Header.Width = Image.Width;
    Header.Height = Image.Height;
    Header.Planes = 1;
    Header.BitsPerPixel = 32;
    Header.Compression = 0;
    Header.SizeOfBitmap = 0;
    Header.HorzResolution = 2835;
    Header.VertResolution = 2835;
    Header.ColorUsed = 0;
    Header.ColorsImportant = 0;

    FILE *OutFile = fopen(Filename, "wb");
    if (OutFile) {
        fwrite(&Header, sizeof(Header), 1, OutFile);
        fwrite(Image.Pixels, OutputPixelsSize, 1, OutFile);
        fclose(OutFile);
    } else {
        printf("[ERROR] - Unable to write output file %s\n", Filename);
    }

}

v3 RayCast(v3 Origin, v3 Direction, world *World) {
    v3 Result = {};
    Result.r = 0.5;
    Result.g = 0;
    Result.b = 0;
    return Result;
}

int main()
{
    material Materials[2] = {};
    Materials[0].Color = V3(0, 0, 0);
    Materials[1].Color = V3(1, 0, 0);

    plane Plane = {};
    Plane.N = V3(0, 0, 1);
    Plane.d = 0;
    Plane.MatIndex = 1;

    world World = {};
    World.MaterialCount = 2;
    World.Materials = Materials;

    World.PlaneCount = 1;
    World.Planes = &Plane;

    World.SphereCount = 0;
    World.Spheres = 0;

    v3 CameraPos = V3(0, 10, 1);
    v3 CameraZ = NOZ(CameraPos);
    v3 CameraX = NOZ(Cross(CameraZ, V3(0, 0, 1)));
    v3 CameraY = NOZ(Cross(CameraZ, CameraX));

    f32 FilmDistanceToCamera = 1.0f;
    v3 FilmCenter = CameraPos - FilmDistanceToCamera * CameraZ;
    f32 FilmW = 1.0f;
    f32 FilmH = 1.0f;

    f32 HalfFilmW = FilmW / 2.0f;
    f32 HalfFilmH = FilmH / 2.0f;

    image_u32 Image = AllocateImage(1280, 720);

    u32 *Out = Image.Pixels;
    for (u32 y = 0; y < Image.Height; y++) {
        f32 FilmY = -1.0f + 2.0f * ((f32)y / (f32)Image.Height);
        for (u32 x = 0; x < Image.Width; x++) {
            f32 FilmX = -1.0f + 2.0f * ((f32)x / (f32)Image.Width);

            v3 FilmP = FilmCenter + HalfFilmW * FilmX * CameraX + HalfFilmH * FilmY * CameraY;
            v3 RayOrigin = CameraPos;
            v3 RayDirection = NOZ(FilmP - CameraPos);

            v3 Color = RayCast(RayOrigin, RayDirection, &World);

            *Out ++ = BMPPackVector(Color);
        }
    }

    WriteImage(Image, "test.bmp");

    return 0;
}
