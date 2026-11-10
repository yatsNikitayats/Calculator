// funccos — функция cos(x), x в градусах: cos(60) = 0.5.
#include <cmath>

#include "calc/plugin_api.h"

namespace {

double cosDegrees(const double* args) {
    constexpr double pi = 3.14159265358979323846;
    const double value = std::cos(args[0] * pi / 180.0);
    constexpr double kZeroTolerance = 1e-12;
    return std::fabs(value) < kZeroTolerance ? 0.0 : value;
}

const CalcPluginInfo kInfo = {
    kCalcPluginApiVersion, "cos", CALC_PLUGIN_FUNCTION, 1, 0, 0,
    &calc_plugin::safeCall<cosDegrees>,
};

}  // namespace

CALC_PLUGIN_EXPORT const CalcPluginInfo* calc_get_plugin_info() { return &kInfo; }
