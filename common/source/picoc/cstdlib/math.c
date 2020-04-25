/*  */
#include <math.h>

#include "interpreter.h"

static float M_EValue        = 2.7182818284590452354;  /* e */
static float M_LOG2EValue    = 1.4426950408889634074;  /* log_2 e */
static float M_LOG10EValue   = 0.43429448190325182765; /* log_10 e */
static float M_LN2Value      = 0.69314718055994530942; /* log_e 2 */
static float M_LN10Value     = 2.30258509299404568402; /* log_e 10 */
static float M_PIValue       = 3.14159265358979323846; /* pi */
static float M_PI_2Value     = 1.57079632679489661923; /* pi/2 */
static float M_PI_4Value     = 0.78539816339744830962; /* pi/4 */
static float M_1_PIValue     = 0.31830988618379067154; /* 1/pi */
static float M_2_PIValue     = 0.63661977236758134308; /* 2/pi */
static float M_2_SQRTPIValue = 1.12837916709551257390; /* 2/sqrt(pi) */
static float M_SQRT2Value    = 1.41421356237309504880; /* sqrt(2) */
static float M_SQRT1_2Value  = 0.70710678118654752440; /* 1/sqrt(2) */

void MathSin(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = sin(Param[0]->Val->Float);
}

void MathCos(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = cos(Param[0]->Val->Float);
}

void MathTan(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = tan(Param[0]->Val->Float);
}

void MathAsin(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = asin(Param[0]->Val->Float);
}

void MathAcos(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = acos(Param[0]->Val->Float);
}

void MathAtan(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = atan(Param[0]->Val->Float);
}

void MathAtan2(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = atan2(Param[0]->Val->Float, Param[1]->Val->Float);
}

void MathSinh(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = sinh(Param[0]->Val->Float);
}

void MathCosh(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = cosh(Param[0]->Val->Float);
}

void MathTanh(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = tanh(Param[0]->Val->Float);
}

void MathExp(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = exp(Param[0]->Val->Float);
}

void MathFabs(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = fabs(Param[0]->Val->Float);
}

void MathFmod(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = fmod(Param[0]->Val->Float, Param[1]->Val->Float);
}

void MathFrexp(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = frexp(Param[0]->Val->Float, Param[1]->Val->Pointer);
}

void MathLdexp(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = ldexp(Param[0]->Val->Float, Param[1]->Val->Integer);
}

void MathLog(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = log(Param[0]->Val->Float);
}

void MathLog10(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = log10(Param[0]->Val->Float);
}

void MathModf(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = modf(Param[0]->Val->Float, Param[0]->Val->Pointer);
}

void MathPow(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = pow(Param[0]->Val->Float, Param[1]->Val->Float);
}

void MathSqrt(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = sqrt(Param[0]->Val->Float);
}

void MathRound(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    /* this awkward definition of "round()" due to it being inconsistently
     * declared in math.h */
    ReturnValue->Val->Float = ceil(Param[0]->Val->Float - 0.5);
}

void MathCeil(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = ceil(Param[0]->Val->Float);
}

void MathFloor(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Float = floor(Param[0]->Val->Float);
}

/* all math.h functions */
struct LibraryFunction MathFunctions[] = {{MathAcos, "float acos(float);"}, {MathAsin, "float asin(float);"}, {MathAtan, "float atan(float);"},
    {MathAtan2, "float atan2(float, float);"}, {MathCeil, "float ceil(float);"}, {MathCos, "float cos(float);"}, {MathCosh, "float cosh(float);"},
    {MathExp, "float exp(float);"}, {MathFabs, "float fabs(float);"}, {MathFloor, "float floor(float);"}, {MathFmod, "float fmod(float, float);"},
    {MathFrexp, "float frexp(float, int *);"}, {MathLdexp, "float ldexp(float, int);"}, {MathLog, "float log(float);"},
    {MathLog10, "float log10(float);"}, {MathModf, "float modf(float, float *);"}, {MathPow, "float pow(float,float);"},
    {MathRound, "float round(float);"}, {MathSin, "float sin(float);"}, {MathSinh, "float sinh(float);"}, {MathSqrt, "float sqrt(float);"},
    {MathTan, "float tan(float);"}, {MathTanh, "float tanh(float);"}, {NULL, NULL}};

/* creates various system-dependent definitions */
void MathSetupFunc(Picoc* pc)
{
    VariableDefinePlatformVar(pc, NULL, "M_E", &pc->FloatType, (union AnyValue*)&M_EValue, false);
    VariableDefinePlatformVar(pc, NULL, "M_LOG2E", &pc->FloatType, (union AnyValue*)&M_LOG2EValue, false);
    VariableDefinePlatformVar(pc, NULL, "M_LOG10E", &pc->FloatType, (union AnyValue*)&M_LOG10EValue, false);
    VariableDefinePlatformVar(pc, NULL, "M_LN2", &pc->FloatType, (union AnyValue*)&M_LN2Value, false);
    VariableDefinePlatformVar(pc, NULL, "M_LN10", &pc->FloatType, (union AnyValue*)&M_LN10Value, false);
    VariableDefinePlatformVar(pc, NULL, "M_PI", &pc->FloatType, (union AnyValue*)&M_PIValue, false);
    VariableDefinePlatformVar(pc, NULL, "M_PI_2", &pc->FloatType, (union AnyValue*)&M_PI_2Value, false);
    VariableDefinePlatformVar(pc, NULL, "M_PI_4", &pc->FloatType, (union AnyValue*)&M_PI_4Value, false);
    VariableDefinePlatformVar(pc, NULL, "M_1_PI", &pc->FloatType, (union AnyValue*)&M_1_PIValue, false);
    VariableDefinePlatformVar(pc, NULL, "M_2_PI", &pc->FloatType, (union AnyValue*)&M_2_PIValue, false);
    VariableDefinePlatformVar(pc, NULL, "M_2_SQRTPI", &pc->FloatType, (union AnyValue*)&M_2_SQRTPIValue, false);
    VariableDefinePlatformVar(pc, NULL, "M_SQRT2", &pc->FloatType, (union AnyValue*)&M_SQRT2Value, false);
    VariableDefinePlatformVar(pc, NULL, "M_SQRT1_2", &pc->FloatType, (union AnyValue*)&M_SQRT1_2Value, false);
}
