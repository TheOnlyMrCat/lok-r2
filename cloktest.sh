#! /bin/sh
./clok $1
mainRet=$?

cat $1 | sed -e 's://.*::g' | tr -d '[:space:]' > test/temp.lok
./clok test/temp.lok
whiteRet=$?
rm test/temp.lok

exit $((mainRet + whiteRet))