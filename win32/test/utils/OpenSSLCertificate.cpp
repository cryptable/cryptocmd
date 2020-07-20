/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 20/07/2020
 */

#include "OpenSSLCertificate.h"


std::unique_ptr<X509_NAME> parseName(const std::string &subjectName) {
#if 0
    /*
    TODO: chtype can be MBSTRING_ASC | MBSTRING_UTF8, canmulti is always on 1.

 * name is expected to be in the format /type0=value0/type1=value1/type2=...
 * where characters may be escaped by \
 */
    X509_NAME *parse_name(const char *cp, long chtype, int canmulti)
    {
        int nextismulti = 0;
        char *work;
        X509_NAME *n;

        if (*cp++ != '/') {
            BIO_printf(bio_err,
                       "name is expected to be in the format "
                       "/type0=value0/type1=value1/type2=... where characters may "
                       "be escaped by \\. This name is not in that format: '%s'\n",
                       --cp);
            return NULL;
        }

        n = X509_NAME_new();
        if (n == NULL)
            return NULL;
        work = OPENSSL_strdup(cp);
        if (work == NULL)
            goto err;

        while (*cp) {
            char *bp = work;
            char *typestr = bp;
            unsigned char *valstr;
            int nid;
            int ismulti = nextismulti;
            nextismulti = 0;

            /* Collect the type */
            while (*cp && *cp != '=')
                *bp++ = *cp++;
            if (*cp == '\0') {
                BIO_printf(bio_err,
                           "%s: Hit end of string before finding the equals.\n",
                           opt_getprog());
                goto err;
            }
            *bp++ = '\0';
            ++cp;

            /* Collect the value. */
            valstr = (unsigned char *)bp;
            for (; *cp && *cp != '/'; *bp++ = *cp++) {
                if (canmulti && *cp == '+') {
                    nextismulti = 1;
                    break;
                }
                if (*cp == '\\' && *++cp == '\0') {
                    BIO_printf(bio_err,
                               "%s: escape character at end of string\n",
                               opt_getprog());
                    goto err;
                }
            }
            *bp++ = '\0';

            /* If not at EOS (must be + or /), move forward. */
            if (*cp)
                ++cp;

            /* Parse */
            nid = OBJ_txt2nid(typestr);
            if (nid == NID_undef) {
                BIO_printf(bio_err, "%s: Skipping unknown attribute \"%s\"\n",
                           opt_getprog(), typestr);
                continue;
            }
            if (*valstr == '\0') {
                BIO_printf(bio_err,
                           "%s: No value provided for Subject Attribute %s, skipped\n",
                           opt_getprog(), typestr);
                continue;
            }
            if (!X509_NAME_add_entry_by_NID(n, nid, chtype,
                                            valstr, strlen((char *)valstr),
                                            -1, ismulti ? -1 : 0))
                goto err;
        }

        OPENSSL_free(work);
        return n;

        err:
        X509_NAME_free(n);
        OPENSSL_free(work);
        return NULL;
    }
#endif

}