#!/bin/sh

. ../_functions.sh

announce "profile.c:profile_fprint_passwords() - normal"
rm -f fake_lock
./stub fprint_passwords 13 3 qwerty \
	| sed 's/[qwerty]/./g' \
	> test.stdout
echo "............." > test.expected
echo "............." >> test.expected
echo "............." >> test.expected
assert_stdout && pass

announce "profile.c:profile_fprint_passwords() - zero character count"
if ./stub fprint_passwords 0 3 qwerty 2>test.stderr; then
	fail "zero length password should error out"
fi
pass

announce "profile.c:profile_fprint_passwords() - 1000 character count"
if ./stub fprint_passwords 1000 3 qwerty 2>test.stderr; then
	fail "large length password should error out"
fi
pass

exit 0
