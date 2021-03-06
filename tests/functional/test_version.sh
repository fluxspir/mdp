# Make sure -V works.

use_config simple

if ! run_mdp -V > test.stdout; then
	echo "fail (wrong return code on -V)"
	exit 1
fi

if ! grep '^mdp-[0-9]\{1,2\}\.[0-9]\{1,2\}\.[0-9]\{1,2\}$' test.stdout > /dev/null; then
	echo "fail (help does not contain version string)"
	exit 1
fi

echo pass
exit 0
