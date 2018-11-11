#define internal static

typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;

typedef float f32;

#pragma pack(push, 1)
struct bitmap_header {
    u16 FileType;
    u32 FileSize;
    u16 Reserved1;
    u16 Reserved2;
    u32 BitmapOffset;

    u32 Size;
    s32 Width;
    s32 Height;
    u16 Planes;
    u16 BitsPerPixel;

    u32 Compression;
    u32 SizeOfBitmap;
    s32 HorzResolution;
    s32 VertResolution;
    u32 ColorUsed;
    u32 ColorsImportant;
};
#pragma pack(pop)

struct image_u32 {
    u32 Width;
    u32 Height;
    u32 *Pixels;
};

struct material {
    v3 Color;
};

struct plane {
    v3 N;
    f32 d;
    u32 MatIndex;
};

struct sphere {
    u32 MatIndex;
    v3 P;
    f32 r;
};

struct world {
    u32 MaterialCount;
    material *Materials;

    u32 PlaneCount;
    plane *Planes;

    u32 SphereCount;
    sphere *Spheres;
};
