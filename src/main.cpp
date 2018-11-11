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

int main(int argc, char **argv)
{
    image_u32 Image = AllocateImage(1280, 720);

    u32 *Out = Image.Pixels;
    for (u32 y = 0; y < Image.Height; y++) {
        for (u32 x = 0; x < Image.Width; x++) {
            *Out++ = (y < 32) ?  0xffff0000: 0xff0000ff ;
        }
    }

    WriteImage(Image, "test.bmp");
    material Materials[2] = {};
    Materials[0].Color = V3(0, 0, 0);
    Materials[1].Color = V3(1, 0, 0);

    plane Plane = {};
    Plane.MatIndex = 1;

    world World = {};
    World.MaterialCount = 2;
    World.Materials = Materials;

    World.PlaneCount = 1;
    World.Planes = &Plane;

    World.SphereCount = 0;
    World.Spheres = 0;

    return 0;
}
