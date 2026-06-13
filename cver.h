#ifndef CVER_H
#define CVER_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

// a struct for easily representing parsed version strings
typedef struct {
  uint8_t major; // these each have a max value of 255
  uint8_t minor;
  uint8_t patch;
} Cversion;

// parse a version string into a Cversion struct, returns zero on failure
int cver_parse(const char* version_string, Cversion* out);

// write a Cversion struct out to a version string buffer, returns 0 on failure
int cver_string(const Cversion* version, char* string, size_t max_length);

// basic comparison function for two Cversions:
// if version A is newer than version B, returns negative
// if version B is newer than version A, returns positive
// if they're identical, returns 0
int cver_compare(const Cversion* a, const Cversion* b);

// version compatibility enum, for easily representing version incompatibility errors
typedef enum {
  CVER_COMPARE_ERROR = INT_MIN, // i.e. one or both of the inputs are null
  CVER_IDENTICAL = 0,
  CVER_MAJOR_MISMATCH,
  CVER_MINOR_MISMATCH,
  CVER_PATCH_MISMATCH
} CverCompatibility;

// check the compatibility of two Cversions, returns the largest mismatch (i.e. will return MAJOR_MISMATCH if both MAJOR and MINOR versions don't match).
// will also return CVER_COMPARE_ERROR if one or both arguments are null
CverCompatibility cver_compatible(const Cversion* a, const Cversion* b);

#ifdef CVER_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

// parse string as Cversion, return zero on failure
int cver_parse(const char* version_string, Cversion* out) {
  if (!version_string || !out) return 0;
  
  const char* p = version_string;
  char* end;

  if (*p == '+' || *p == '-') return 0; // reject leading signs
  unsigned long major = strtoul(p, &end, 10);
  if (end == p || *end != '.') return 0;
  if (major > UINT8_MAX) return 0;
  p = end + 1;

  if (*p == '+' || *p == '-') return 0; // reject leading signs
  unsigned long minor = strtoul(p, &end, 10);
  if (end == p || *end != '.') return 0;
  if (minor > UINT8_MAX) return 0;
  p = end + 1;

  if (*p == '+' || *p == '-') return 0; // reject leading signs
  unsigned long patch = strtoul(p, &end, 10);
  if (end == p || *end != '\0') return 0;
  if (patch > UINT8_MAX) return 0;

  out->major = (uint8_t)major;
  out->minor = (uint8_t)minor;
  out->patch = (uint8_t)patch;
  
  return 1; // success
}

// write Cversion to string, return 0 on failure
int cver_string(const Cversion* version, char* string, size_t max_length) {
  if (!version || !string) return 0;

  int written = snprintf(
    string, 
    (size_t)max_length, 
    "%u.%u.%u", 
    version->major, 
    version->minor, 
    version->patch
  );

  if (written < 0) return 0; // formatting error
  if ((size_t)written >= max_length) return 0; // output truncated

  return 1;
}

// if version A is newer than version B, returns negative
// if version B is newer than version A, returns positive
// if they're identical, returns 0
int cver_compare(const Cversion* a, const Cversion* b) {
  // handling nulls:
  if (!a && !b) return 0;
  if (!a) return 1;
  if (!b) return -1;

  if (a->major != b->major) return (a->major > b->major) ? -1 : 1;
  if (a->minor != b->minor) return (a->minor > b->minor) ? -1 : 1;
  if (a->patch != b->patch) return (a->patch > b->patch) ? -1 : 1;
  
  return 0;
}

// check the compatibility of two Cversions, returning the largest mismatch
CverCompatibility cver_compatible(const Cversion* a, const Cversion* b) {
  if (!a || !b) return CVER_COMPARE_ERROR;

  if (a->major != b->major) return CVER_MAJOR_MISMATCH;
  if (a->minor != b->minor) return CVER_MINOR_MISMATCH;
  if (a->patch != b->patch) return CVER_PATCH_MISMATCH;
  
  return CVER_IDENTICAL;
}

#endif // CVER_IMPLEMENTATION

#endif // CVER_H
