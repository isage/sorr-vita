/*
 *  Copyright © 2006-2012 SplinterGU (Fenix/Bennugd)
 *  Copyright © 2002-2006 Fenix Team (Fenix)
 *  Copyright © 1999-2002 José Luis Cebrián Pagüe (Fenix)
 *
 *  This file is part of Bennu - Game Development
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

#define ISALPHA(c)     (c_type [(unsigned char)c] & CTYPE_ALPHA)
#define ISSPACE(c)     (c_type [(unsigned char)c] & CTYPE_SPACE)
#define ISNUM(c)       (c_type [(unsigned char)c] & CTYPE_NUM)
#define ISALNUM(c)     (c_type [(unsigned char)c] & CTYPE_ALNUM)
#define ISWORDCHAR(c)  (c_type [(unsigned char)c] & CTYPE_WORDCHAR)
#define ISWORDFIRST(c) (c_type [(unsigned char)c] & CTYPE_WORDFIRST)
#define TOUPPER(c)     (c_upper[(unsigned char)c])
#define TOLOWER(c)     (c_lower[(unsigned char)c])

#define CTYPE_ALPHA     1
#define CTYPE_NUM       2
#define CTYPE_WORDCHAR  4
#define CTYPE_WORDFIRST 8
#define CTYPE_SPACE     16
#define CTYPE_ALNUM     32

