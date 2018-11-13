#include <algorithm>
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

f32 RayInersectPlane(v3 RayOrigin, v3 RayDireciton, v3 PlaneNormal, f32 PlaneDist) {
    f32 Result = (f32)( - PlaneDist  - PlaneNormal * RayOrigin )/ (f32)(PlaneNormal * RayDireciton);
    return Result;
}

f32 RayInersectSphere(v3 RayOrigin, v3 RayDireciton, sphere Sphere) {
    v3 SphereRelativeOrigin = RayOrigin - Sphere.P;

    f32 Result = F32Max;
    f32 a = RayDireciton * RayDireciton;
    f32 b = 2 * RayDireciton * SphereRelativeOrigin;
    f32 c = SphereRelativeOrigin * SphereRelativeOrigin - Sphere.r * Sphere.r;

    f32 x1 = (f32)(-b + sqrt(b * b - 4 * a * c))/(f32)(2 * a);
    f32 x2 = (f32)(-b - sqrt(b * b - 4 * a * c))/(f32)(2 * a);
    return std::min(x1, std::min(x2, Result));
}

v3 RayCast(v3 RayOrigin, v3 RayDirection, world *World) {
    f32 MinHitDistance = 0.001f;

    v3 Result = {};
    v3 Attenuation = V3(1, 1, 1);
    for (auto RayCount = 0; RayCount < 8; ++RayCount) {
        f32 HitDistance = F32Max;
        bool HitSomething = false;

        u32 HitMaterialIndex = 0;
        v3 NextOrigin = {};
        v3 NextNormal = {};

        for (u32 PlaneIndex = 0; PlaneIndex < World->PlaneCount; PlaneIndex++) {
            plane *Plane = &World->Planes[PlaneIndex];
            f32 ThisDistance = RayInersectPlane(RayOrigin, RayDirection, Plane->N, Plane->d);

            f32 Denom = Plane->N * RayDirection;
            if ((Denom > -Epsilon) && (Denom < Epsilon)) {
                continue;
            }
            if ((ThisDistance > MinHitDistance) && (ThisDistance < HitDistance)) {
                HitSomething = true;
                HitDistance = ThisDistance;
                HitMaterialIndex = Plane->MatIndex;
                NextOrigin = ThisDistance*RayDirection;
                NextNormal = Plane->N;
            }
        }

        for (u32 SphereIndex = 0; SphereIndex < World->SphereCount; SphereIndex++) {
            sphere *Sphere = &World->Spheres[SphereIndex];
            f32 ThisDistance = RayInersectSphere(RayOrigin, RayDirection, *Sphere);

            if ((ThisDistance > MinHitDistance) && (ThisDistance < HitDistance)) {
                HitSomething = true;
                HitDistance = ThisDistance;
                HitMaterialIndex = Sphere->MatIndex;
                NextOrigin = ThisDistance*RayDirection;
                NextNormal = NOZ(NextOrigin);
            }
        }

        if (HitMaterialIndex) {
            material Mat = World->Materials[HitMaterialIndex];

            Result = Result +  Hadamard(Attenuation, Mat.EmitColor);
            Attenuation = Hadamard(Attenuation, Mat.ReflectColor);
            RayOrigin = NextOrigin;
            // TODO: reflection
            RayDirection = NextNormal;

        } else {
            material Mat = World->Materials[HitMaterialIndex];
            Result = Result +  Hadamard(Attenuation, Mat.EmitColor);
            break;
        }
    }
    return Result;
}

int main()
{
    material Materials[3] = {};
    Materials[0].EmitColor= V3(0.3, 0.4, 0.5);
    Materials[1].EmitColor= V3(0.4, 0.7, 0.4);
    Materials[2].EmitColor= V3(0, 0, 0.8);

    plane Plane = {};
    Plane.N = V3(0, 0, 1);
    Plane.d = 0;
    Plane.MatIndex = 1;

    world World = {};
    World.MaterialCount = 2;
    World.Materials = Materials;

    World.PlaneCount = 1;
    World.Planes = &Plane;

    sphere Spheres[1] = {};
    Spheres[0].MatIndex = 2;
    Spheres[0].P = V3(0, 0, 0.63);
    Spheres[0].r = 1.0f;

    World.SphereCount = 1;
    World.Spheres = Spheres;
    image_u32 Image = AllocateImage(1280, 720);

    v3 CameraPos = V3(0, 10, 1);
    v3 CameraZ = NOZ(CameraPos);
    v3 CameraX = NOZ(Cross(CameraZ, V3(0, 0, 1)));
    v3 CameraY = NOZ(Cross(CameraZ, CameraX));

    f32 FilmDistanceToCamera = 1.0f;
    v3 FilmCenter = CameraPos - FilmDistanceToCamera * CameraZ;
    f32 FilmW = 1.0f;
    f32 FilmH = 1.0f;
    if (Image.Width > Image.Height) {
        FilmH = FilmW * ((f32) Image.Height / (f32) Image.Width);
    }
    if (Image.Height > Image.Width) {
        FilmW = FilmH * ((f32) Image.Width / (f32) Image.Height);
    }

    f32 HalfFilmW = FilmW * 0.5f;
    f32 HalfFilmH = FilmH * 0.5f;


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
        if ( y % 64 == 0) {
            printf("Calculating %d\%\n", (u32)(100.0f * (f32)y/(f32)Image.Height));
        }
    }

    WriteImage(Image, "test.bmp");

    return 0;
}
