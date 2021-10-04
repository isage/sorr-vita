/*
 *  Copyright (C) 2014-2016 Joseba García Etxebarria <joseba.gar@gmail.com>
 *  Copyright (C) 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright (C) 2002-2006 Fenix Team (Fenix)
 *  Copyright (C) 1999-2002 José Luis Cebrián Pagüe (Fenix)
 *
 *  This file is part of BennuGD
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgddl.h"

#include "bgdrtm.h"

#include "files.h"
#include "xstrings.h"

#include "dlvaracc.h"
#include <tre/tre.h>

#ifndef __MONOLITHIC__
#include "mod_regex_symbols.h"
#else
extern DLVARFIXUP __bgdexport(mod_regex, globals_fixup)[];
#endif

/* ----------------------------------------------------------------- */

enum { REGEX_REG = 0 };

/* ----------------------------------------------------------------- */
// Generic replace function based on TRE
static int replace(const char *reg, const char *str, const char *rep, int cflags) {
    /* Replacing is, basically,
       splitting followed by joining */
    int result_string = 0;
    unsigned n = 0, count = 0, total_length = 0;
    char *pieces[16];
    char *result, *ptr;
    regex_t pb;
    regmatch_t pmatch[1];

    if (tre_regcomp(&pb, reg, cflags) == REG_OK) {
        // Match repeatedly, until we can't match anymore
        while (tre_regexec(&pb, str, 1, pmatch, 0) != REG_NOMATCH) {
            pieces[count] = malloc(pmatch[0].rm_so + 1);
            strncpy(pieces[count], str, pmatch[0].rm_so);
            pieces[count][pmatch[0].rm_so] = '\0';
            str += pmatch[0].rm_eo;
            count++;
            if (count == 16) {
                break;
            }
        }

        /* We're missing the last chunk of text, so
           we add it here */
        if (count < 15) {
            pieces[count] = malloc(strlen(str) + 1);
            strncpy(pieces[count], str, strlen(str));
            pieces[count][strlen(str)] = '\0';
            count++;
        }
    } else {
        // Return an empty string
        result_string = string_new("");
        string_use(result_string);
        return result_string;
    }

    // Compute the total length of the resulting string
    for (n = 0; n < count; n++) {
        total_length += strlen(pieces[n]);
    }
    total_length += (count - 1) * strlen(rep);
    result = malloc(total_length + 1);
    ptr    = result;

    // Copy the strings to result, and free the pieces
    for (n = 0; n < count; n++) {
        memcpy(ptr, pieces[n], strlen(pieces[n]));
        ptr += strlen(pieces[n]);
        if (n < count - 1) {
            memcpy(ptr, rep, strlen(rep));
            ptr += strlen(rep);
        }
        free(pieces[n]);
    }
    *ptr = 0;

    // Return the new string
    result_string = string_new(result);
    string_use(result_string);

    free(result);

    return result_string;
}

/* ----------------------------------------------------------------- */

/** REGEX (STRING pattern, STRING string)
 *  Match a regular expresion to the given string. Fills the
 *  REGEX_REG global variables and returns the character position
 *  of the match or -1 if none found.
 */

int modregex_regex(INSTANCE *my, int *params) {
    const char *reg = string_get(params[0]);
    const char *str = string_get(params[1]);
    int pos         = -1;
    unsigned n;

    regex_t pb;
    regmatch_t pmatch[1];

    int *regex_reg;

    // Compile the regular expression, then match it
    if (tre_regcomp(&pb, reg, REG_EXTENDED | REG_ICASE) == REG_OK) {
        // Fill the regex_reg global variables
        regex_reg = (int *)&GLODWORD(mod_regex, REGEX_REG);
        n = 0;
        while (tre_regexec(&pb, str, 1, pmatch, 0) != REG_NOMATCH && n < 16) {
            // Store the position
            if (pos == -1) {
                pos = pmatch[0].rm_so;
            }
            // Store the strings for PixTudio use
            string_discard(regex_reg[n]);
            regex_reg[n] = string_newa(str + pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);
            string_use(regex_reg[n]);
            n++;

            // Move the pointer to the end of the previous match
            str += pmatch[0].rm_eo;
            if (strlen(str) <= 0) {
                break;
            }
        }

        tre_regfree(&pb);
    }

    /* Free the resources */
    string_discard(params[0]);
    string_discard(params[1]);

    return pos;
}

/** STR_REPLACE (STRING search, STRING string, STRING replacement)
 *  Replaces the given "search" occurences in "string" for
 * "replacement".
 *  Returns the resulting string. REGEX_REG variables are
 *  filled with information about the first match.
 */
int modregex_string_replace(INSTANCE *my, int *params) {
    /* Replacing is, basically,
       splitting followed by joining */
    const char *reg = string_get(params[0]);
    const char *str = string_get(params[1]);
    const char *rep = string_get(params[2]);

    int result_string = replace(reg, str, rep, REG_LITERAL);

    // Free resource
    string_discard(params[0]);
    string_discard(params[1]);
    string_discard(params[2]);

    return result_string;
}

/** REGEX_REPLACE (STRING pattern, STRING string, STRING replacement)
 *  Match a regular expresion to the given string. For each
 *  match, substitute it with the given replacement.
 *  Returns the resulting string. REGEX_REG variables are
 *  filled with information about the first match.
 */
int modregex_regex_replace(INSTANCE *my, int *params) {
    /* Replacing is, basically,
       splitting followed by joining */
    const char *reg = string_get(params[0]);
    const char *str = string_get(params[1]);
    const char *rep = string_get(params[2]);

    int result_string = replace(reg, str, rep, REG_EXTENDED);

    // Free resource
    string_discard(params[0]);
    string_discard(params[1]);
    string_discard(params[2]);

    return result_string;
}

/** SPLIT (STRING regex, STRING string, STRING POINTER array, INT array_size)
 *  Fills the given array with sections of the given string, using
 *  the given regular expression as separators. Returns the number
 *  of elements filled in the array.
 *
 */

int modregex_split(INSTANCE *my, int *params) {
    const char *reg       = string_get(params[0]);
    const char *str       = string_get(params[1]);
    int *result_array     = (int *)params[2];
    int result_array_size = params[3];
    int count             = 0;

    regex_t pb;
    regmatch_t pmatch[1];

    if (tre_regcomp(&pb, reg, REG_EXTENDED) == REG_OK) {
        // Match repeatedly, until we can't match anymore
        while (tre_regexec(&pb, str, 1, pmatch, 0) != REG_NOMATCH) {
            *result_array = string_newa(str, pmatch[0].rm_so);
            str += pmatch[0].rm_eo;
            string_use(*result_array);
            result_array++;
            count++;
            result_array_size--;
            if (result_array_size == 0) {
                break;
            }
        }

        /* We're missing the last chunk of text, so
           we add it here */
        if (result_array_size > 0) {
            *result_array = string_new(str);
            result_array++;
            string_use(*result_array);
            result_array_size--;
            count++;
        }
    }

    /* Free the resources */
    string_discard(params[0]);
    string_discard(params[1]);

    return count;
}

/** JOIN (STRING separator, STRING POINTER array, INT array_size)
 *  Joins an array of strings, given a separator. Returns the
 *  resulting string.
 */

int modregex_join(INSTANCE *my, int *params) {
    const char *sep   = string_get(params[0]);
    int *string_array = (int *)params[1];
    int count         = params[2];
    int total_length  = 0;
    int sep_len       = strlen(sep);
    int n;
    char *buffer;
    char *ptr;
    int result;

    for (n = 0; n < count; n++) {
        total_length += strlen(string_get(string_array[n]));
        if (n < count - 1)
            total_length += sep_len;
    }

    buffer = malloc(total_length + 1);
    ptr    = buffer;

    for (n = 0; n < count; n++) {
        memcpy(ptr, string_get(string_array[n]), strlen(string_get(string_array[n])));
        ptr += strlen(string_get(string_array[n]));
        if (n < count - 1) {
            memcpy(ptr, sep, sep_len);
            ptr += sep_len;
        }
    }
    *ptr   = 0;
    result = string_new(buffer);
    free(buffer);
    string_use(result);
    return result;
}
