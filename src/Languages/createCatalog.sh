# example sh script to generate catalog file
# it searches for strings marked with _() in all .cpp files in directory ./src
# the generated file is called 'dmgboy.pot'
CPP_FILE_LIST=`find ./.. -name '*.cpp' -print`
xgettext -d dmgboy -s --keyword=_ -p . -o dmgboy.pot $CPP_FILE_LIST