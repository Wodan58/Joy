#
#   module  : banner.sh
#   version : 1.2
#   date    : 04/11/22
#
#   Announce project creation
#
echo Updating $1 version $2
lcov --version
lcov --directory . --zerocounters
