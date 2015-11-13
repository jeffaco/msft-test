#!/bin/sh
# the following outputs what John said his ommand would output:
#     \"%%/%%"\
sh -c "echo "'\\\"%%/%%\"\\' 3 more parts

# the following is also interesting, it outputs:
#     "%%/%%" != '%%/%%'
sh -c "echo "'\"'"%%/%%"'\"'" \!= \'%%/%%\'"

# "sh -c \"echo \"'\\\"%%/%%\\\"'\" != \'%%/%%\'\""
