#!/bin/sh

. ../_functions.sh

announce "generate_passwords"
rm -f fake_lock
./stub 13 3 | sed 's/././g' > output
echo "............." > expected
echo "............." >> expected
echo "............." >> expected
assert_output && pass

exit 0
