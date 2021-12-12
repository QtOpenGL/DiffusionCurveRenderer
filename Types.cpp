#include "Types.h"

RenderMode operator+(const RenderMode &lhs, const RenderMode &rhs)
{
    return RenderMode(int(lhs) ^ int(rhs));
}

bool operator&(const RenderMode &lhs, const RenderMode &rhs)
{
    return (int(lhs) & int(rhs));
}

DirtType operator+(const DirtType &lhs, const DirtType &rhs)
{
    return DirtType(int(lhs) ^ int(rhs));
}

bool operator&(const DirtType &lhs, const DirtType &rhs)
{
    return (int(lhs) & int(rhs));
}
