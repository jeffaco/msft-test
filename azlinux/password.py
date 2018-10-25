#!/usr/bin/python3

import crypt
import getpass

# requires python >=3.4
print(
    crypt.crypt(
        getpass.getpass(),
        crypt.mksalt(crypt.METHOD_SHA512)
    )
)
