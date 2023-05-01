#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

Fixedpoint fixedpoint_create(uint64_t whole) {
    return fixedpoint_create2(whole,0);
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
    Fixedpoint fp = {
            .whole_p = whole,
            .frac_p = frac,
            .tag = VALID_NONNEG,
    };
    return fp;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
    Fixedpoint fp = {
            .whole_p = 0UL,
            .frac_p = 0UL,
            .tag = VALID_NONNEG,
    };
    char *ptr = hex;
    if (hex[0] == '-') {
        fp.tag = VALID_NEG;
        ptr++;
    }
    char wholeS[17] = "0";
    char fracS[17] = "0";
    int indexW = 0;

    while (ptr != NULL && *ptr != '\0') {
        if ((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'A' && *ptr <= 'F') || (*ptr >= 'a' && *ptr <= 'f')) {
            wholeS[indexW] = *ptr;
            indexW++;
            ptr++;
        } else if (*ptr == '.') {
            ptr++;
            break;
        } else {
            fp.tag = ERROR_VALUE;
            return fp;
        }
    }

    int indexF = 0;
    while (ptr != NULL && *ptr != '\0') {
        if ((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'A' && *ptr <= 'F') || (*ptr >= 'a' && *ptr <= 'f')) {
            fracS[indexF] = *ptr;
            ptr++;
            indexF++;
        } else {
            fp.tag = ERROR_VALUE;
            return fp;
        }
    }

    if (indexW > 16 || indexF > 16 || (indexW == 0 && indexF == 0)) {
        fp.tag = ERROR_VALUE;
        return fp;
    }

    // add trailing zeroes to frac string
    size_t extra = 16 - strlen(fracS);
    char *zeroes = "0000000000000000";
    strncat(fracS, zeroes, extra);
    fp.whole_p = strtoull(wholeS, NULL, 16);
    fp.frac_p = strtoull(fracS, NULL, 16);
    return fp;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
    assert(fixedpoint_is_valid(val));
    return val.whole_p;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
    assert(fixedpoint_is_valid(val));
    return val.frac_p;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
    assert(fixedpoint_is_valid(left));
    assert(fixedpoint_is_valid(right));

    Fixedpoint fp2 = fixedpoint_create(0UL);
    if (left.tag != right.tag) {
        int leftIsGreater;
        if (left.tag == VALID_NONNEG) {
            leftIsGreater = fixedpoint_compare(left, fixedpoint_negate(right));
        } else {
            leftIsGreater = fixedpoint_compare(fixedpoint_negate(left), right);
        }

        if (leftIsGreater == 0) {
            return fixedpoint_create(0UL);
        } else if (leftIsGreater == 1) {
            fp2.tag = left.tag;
            fp2.whole_p = left.whole_p - right.whole_p;
            fp2.frac_p = left.frac_p - right.frac_p;
            // accommodate wrapped fraction
            if (right.frac_p > left.frac_p) {
                fp2.whole_p -= 1UL;
            }
        } else if (leftIsGreater == -1) {
            fp2.tag = right.tag;
            fp2.whole_p = right.whole_p - left.whole_p;
            fp2.frac_p = right.frac_p - left.frac_p;
            // accommodate wrapped fraction
            if (left.frac_p > right.frac_p) {
                fp2.whole_p -= 1UL;
            }
        }
        return fp2;
    }

    // SAME SIGN
    fp2.tag = left.tag;
    fp2.whole_p = left.whole_p + right.whole_p;
    fp2.frac_p = left.frac_p + right.frac_p;

    // Handle fraction part overflow
    if (fp2.frac_p < left.frac_p) {
        fp2.whole_p += 1UL;
    }

    // invalid values
    if (fp2.whole_p < left.whole_p || (fp2.whole_p == left.whole_p && right.whole_p != 0)) {
        if (left.tag == VALID_NONNEG) {
            fp2.tag = POS_OVERFLOW;
        } else if (left.tag == VALID_NEG) {
            fp2.tag = NEG_OVERFLOW;
        }
    }

    return fp2;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
    return fixedpoint_add(left, fixedpoint_negate(right));
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
    assert (fixedpoint_is_valid(val));
    Fixedpoint fp2 = val;
    if (fixedpoint_is_zero(val)) {
        return val;
    }
    if (val.tag == VALID_NONNEG) {
        fp2.tag = VALID_NEG;
    }
    if (val.tag == VALID_NEG) {
        fp2.tag = VALID_NONNEG;
    }
    return fp2;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
    assert(fixedpoint_is_valid(val));
    Fixedpoint fp2;
    fp2.whole_p = val.whole_p/2;
    fp2.frac_p = val.frac_p/2;
    fp2.tag = val.tag;

    // check underflow
    if (val.whole_p%2 == 1) {
        fp2.frac_p += 0x8000000000000000UL;
    }
    if (val.frac_p%2 == 1) {
        if (val.tag == VALID_NONNEG) {
            fp2.tag = POS_UNDERFLOW;
        } else if (val.tag == VALID_NEG) {
            fp2.tag = NEG_UNDERFLOW;
        }
    }
    return fp2;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
    return fixedpoint_add(val, val);
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
    // different signs
    if (left.tag == VALID_NONNEG && right.tag == VALID_NEG) {
        return 1;
    }
    if (right.tag == VALID_NONNEG && left.tag == VALID_NEG) {
        return -1;
    }

    // same sign
    int temp;
    if (left.whole_p > right.whole_p) {
        temp = 1;
    } else if (left.whole_p < right.whole_p) {
        temp = -1;
    } else if (left.frac_p > right.frac_p) {
        temp = 1;
    } else if (left.frac_p < right.frac_p) {
        temp = -1;
    } else {
        temp = 0;
    }

    // if both signs are negative, return opposite value
    if (left.tag == VALID_NONNEG) {
        return temp;
    }
    return -temp;
}

int fixedpoint_is_zero(Fixedpoint val) {
    assert(fixedpoint_is_valid(val));
    if (val.whole_p == 0 && val.frac_p == 0) {
        return 1;
    }
    return 0;
}

int fixedpoint_is_err(Fixedpoint val) {
    return val.tag == ERROR_VALUE;
}

int fixedpoint_is_neg(Fixedpoint val) {
    assert(fixedpoint_is_valid(val));
    return val.tag == VALID_NEG;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
    return val.tag == NEG_OVERFLOW;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
    return val.tag == POS_OVERFLOW;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
    return val.tag == NEG_UNDERFLOW;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
    return val.tag == POS_UNDERFLOW;
}

int fixedpoint_is_valid(Fixedpoint val) {
    return val.tag == VALID_NEG || val.tag == VALID_NONNEG;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
   assert(fixedpoint_is_valid(val));
   char *hex = malloc(34);
   char buffer[17];
   hex[0] = '\0';

   if (val.tag == VALID_NEG) { strcat(hex, "-"); }

   // whole part -> hex
   sprintf(buffer, "%llx", val.whole_p);
   strcat(hex, buffer);

   // Appending frac -> hex
   if (val.frac_p) {
       strcat(hex, ".");
       sprintf(buffer, "%016llx", val.frac_p);

       // Removing zero from frac
       int i = strlen(buffer) - 1;
       while (buffer[i] == '0') {
           i--;
       }
       buffer[i+1] = '\0';

       strcat(hex, buffer);
   }

   return hex;
}
