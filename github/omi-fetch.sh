#!/usr/bin/env bash

# Let's quit on interrupt of subcommands
trap '
  trap - INT # restore default INT handler
  echo "Interrupted"
  kill -s INT "$$"
' INT

# Retrieves asset ID and package name of asset ending in argument
# $info looks like: "id": 1698239, "name": "powershell_0.4.0-1_amd64.deb",
get_info() {
    curl -s https://api.github.com/repos/Microsoft/omi/releases/latest | grep -B 1 "name.*$1"
}

# Parsing logic

usage()
{
    echo "$0 <options>"
    echo
    echo "Valid options are:"
    echo "  -b: Bit width (either \"32\" or \"64\")"
    echo "  -s: SSL version (either \"098\" or \"100\")"
    echo
    echo "Note that the -b qualifier is mandatory, and must be specified."
    echo "If the -s qualfiier is NOT specified, all SSL versions (both"
    echo "SSL 0.9.8 and SSL 1.0.0) versions of OMI are retrived for the"
    echo "specified bit-width.  This is a total of 4 kits:"
    echo "    OMI [bit-depth] SSL 0.9.8 RPM"
    echo "    OMI [bit-depth] SSL 1.0.0 RPM"
    echo "    OMI [bit-depth] SSL 0.9.8 DEB"
    echo "    OMI [bit-depth] SSL 1.0.0 DEB"
    echo
    echo "If both 32-bit kits and 64-bit kits are desired, then this script"
    echo "must be run twice."

    exit 1
}

BITS=
SSL_VERSION=

while getopts "h?b:s:" opt; do
    case "$opt" in
        h)
            usage
            ;;

        \?)
            echo "$0: Invalid option specified, use -h for usage" >&2
            exit 1
            ;;

        b)
            BITS=$OPTARG

            if [ $BITS != "32" -a $BITS != "64" ]; then
                echo "$0: Invalid value for -b: \"$BITS\"" >&2
                exit 1
            fi
            ;;

        s)
            SSL_VERSION=$OPTARG

            if [ $SSL_VERSION != "098" -a $SSL_VERSION != "100" ]; then
                echo "$0: Invalid value for -s: \"$SSL_VERSION\"" >&2
                exit 1
            fi
            ;;
    esac
done
shift $((OPTIND-1))

if [ "$@ " != " " ]; then
    echo "$0: Parsing error: '$@' is unparsed, use -h for help" >&2
    exit 1
fi

if [ "$BITS " = " " ]; then
    echo "$0: Mandatory qualifier -b not specified, please use -h for help" >&2
    exit 1
fi

echo "Bits: $BITS"
echo "SSL Version: $SSL_VERSION"

exit 0



info=$(get_info "omi.*100\.x64\.rpm")

# Parses $info for asset ID and package name
read asset package <<< $(echo "$info" | sed 's/[,"]//g' | awk '{ print $2; print $4 }')

echo "Asset: $asset"
echo "Package: $package"

# Downloads asset to file
packageuri=$(curl -s -i -H 'Accept: application/octet-stream' "https://api.github.com/repos/Microsoft/omi/releases/assets/$asset" |
    grep location | sed 's/location: //g')
curl -C - -o "$package" ${packageuri%$'\r'}

if [[ ! -r "$package" ]]; then
    echo "ERROR: $package failed to download! Aborting..." >&2
    exit 1
fi

exit 0
