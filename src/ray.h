#include <math.h>
#define internal static


typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;

typedef char s8;
typedef short s16;
typedef int s32;

typedef float f32;

union v3 {
    struct
    {
        f32 u, v, _;
    };
    struct
    {
        f32 x, y, z;
    };
    struct
    {
        f32 r, g, b;
    };
};

v3 V3(f32 x, f32 y, f32 z) {
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

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

v3 operator*(f32 multiplyer, v3 A ) {
    v3 Result = {};
    Result.x = A.x * multiplyer;
    Result.y = A.y * multiplyer;
    Result.z = A.z * multiplyer;
    return Result;
};

v3 operator*(v3 A, f32 multiplyer) {
    return operator*(multiplyer, A);
}

v3 operator-(v3 A, v3 B ) {
    v3 Result = {};
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return Result;
};


v3 operator+(v3 A, v3 B ) {
    v3 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return Result;
};


f32 Inner(v3 A, v3 B) {
    f32 result = A.x * B.x + A.y * B.y + A.z * B.z;
    return result;
}

f32 LengthSq(v3 A) {
    return Inner(A, A);
}

v3 Cross(v3 A, v3 B) {
    v3 Result = {};

    Result.x = A.y * B.y + A.z * B.z;
    Result.y = A.x * B.x + A.z * B.z;
    Result.z = A.x * B.x + A.y * B.y;

    return Result;
}

v3 NOZ(v3 A) {
    v3 Result = {};
    f32 LenSq = LengthSq(A);
    if (LenSq > 0.00001 * 0.00001) {
        Result = A * (1.0f / sqrt(LenSq));
    }
    return Result;
}

u32 BMPPackVector(v3 v) {
    u32 Result = (
        (0xFF << 24) |
        ((u32)(255.0f * v.r) << 16) |
        ((u32)(255.0f * v.g) << 8) |
        ((u32)(255.0f * v.b) << 0)
        );
    return Result;
}
