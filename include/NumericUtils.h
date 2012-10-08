#ifndef NUMERICUTILS_H
#define NUMERICUTILS_H

#include <limits>

namespace PS { namespace Util {
  class NumericUtils {
  public:
    /**
     * @brief DOUBLE_EPSILON smallest difference between two numbers
     * that can be recognized.
     *
     * TODO: replace this with std::numeric_limits<double>::epsilon()
     */
    static const double DOUBLE_EPSILON = 0.0000001;

    /* double absolute */
    static double absDouble(double a) {
      return a < 0 ? a * -1.0f : a;
    }

    /* a equal b? */
    static bool equalWithEpsilon(double a, double b) {
      return equalWithCustomEpsilon(a, b, DOUBLE_EPSILON);
    }

    /* a smaller b? */
    static bool smallerWithEpsilon(double a, double b) {
      return smallerWithCustomEpsilon(a, b, DOUBLE_EPSILON);
    }

    /* a greater b? */
    static bool greaterWithEpsilon(double a, double b) {
      return greaterWithCustomEpsilon(a, b, DOUBLE_EPSILON);
    }

    /* a smaller or equal b? */
    static bool smallerOrEqualWithEpsilon(double a, double b) {
      return smallerOrEqualWithCustomEpsilon(a, b, DOUBLE_EPSILON);
    }

    /* a greater or equal b? */
    static bool greaterOrEqualWithEpsilon(double a, double b) {
      return greaterOrEqualWithCustomEpsilon(a, b, DOUBLE_EPSILON);
    }

    /* a smaller b with custom epsilon */
    static bool smallerWithCustomEpsilon(double a, double b, double epsilon) {
      bool result = a < b ? absDouble(b - a) > epsilon : false;
      return result;
    }

    /* a greater b with custom epsilon */
    static bool greaterWithCustomEpsilon(double a, double b, double epsilon) {
      bool result = a > b ? absDouble(a - b) > epsilon : false;
      return result;
    }

    /* a equal b with custom epsilon */
    static bool equalWithCustomEpsilon(double a, double b, double epsilon) {
      return absDouble(a - b) < epsilon;
    }

    /* a smaller or equal b with custom epsilon */
    static bool smallerOrEqualWithCustomEpsilon(double a, double b, double epsilon) {
      bool result = equalWithCustomEpsilon(a, b, epsilon)
        ? true
        : smallerWithCustomEpsilon(a, b, epsilon);

      return result;
    }

    /* a greater or equal b with custom epsilon */
    static bool greaterOrEqualWithCustomEpsilon(double a, double b, double epsilon) {
      bool result = equalWithCustomEpsilon(a, b, epsilon)
        ? true
        : greaterWithCustomEpsilon(a, b, epsilon);

      return result;
    }
  };
} }
#endif // NUMERICUTILS_H
