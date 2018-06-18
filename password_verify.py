import crypt
import getpass

# Original password was: "password"
stored_pw_hash='$6$5Xa0PKcCvx7kxhy7$ZQZmZqecbGRFJYzziFcK3.S/4fw44mbJGoGmA2EOUwe8QhtaIExWAWzGM78iMeE1QLpomV.Zdc0bAO21nYgRD0'

entered_pw_hash = crypt.crypt(
    getpass.getpass(), stored_pw_hash
)

print(stored_pw_hash)
print(entered_pw_hash)

if entered_pw_hash == stored_pw_hash:
    print('ok')
else:
    print('go away')
