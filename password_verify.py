#!/usr/bin/python3

import crypt
import getpass

# Get the shadow password entry
stored_pw_hash = getpass.getpass("Shadow line: ")

entered_pw_hash = crypt.crypt(
    getpass.getpass(), stored_pw_hash
)

# print(stored_pw_hash)
# print(entered_pw_hash)

if entered_pw_hash == stored_pw_hash:
    print('ok')
else:
    print('PASSWORDS DO NOT MATCH!')
