#! /bin/bash

FindClientLibrary() {
    MYSQL_NAME=libmysqlclient.so
    CLIENT_LIBRARY=

    # We use a variety of means to find libmysqlclient.so:
    #
    # 1. Via ldd of 'mysql' client program (may not be installed, or may link statically to MySQL)
    # 2. Via ldconfig -p (doesn't work properly on all systems)
    # 3. Search a list of likely directories
    #
    # If, after all that, we don't find it, then CLIENT_LIBRARY is empty, signifying a failure

    MYSQL_PATH=`which mysql`
    [ -n "${MYSQL_PATH}" ] && CLIENT_LIBRARY=`ldd ${MYSQL_PATH} | grep ${MYSQL_NAME} | awk '{print $3}'`

    # Be aware that /sbin/ldconfig may return multiple hits; if so, we'll use the first one
    [ -z "${CLIENT_LIBRARY}" ] && CLIENT_LIBRARY=`/sbin/ldconfig -p | grep ${MYSQL_NAME} | head -1 | awk '{print $NF}'`

    if [ -z "${CLIENT_LIBRARY}" ]; then
        SEARCH_LIST_64="/usr/lib64 /usr/lib64/mysql /lib64 /lib64/mysql /usr/lib/x86_64-linux-gnu"
        SEARCH_LIST_32="/usr/lib /usr/lib/mysql /lib /lib/mysql /usr/lib/*86-linux-gnu /usr/local/mysql/lib"

        if [ `uname -m` = "x86_64" ]; then
            SEARCH_LIST="${SEARCH_LIST_64} ${SEARCH_LIST_32}"
        else
            SEARCH_LIST="${SEARCH_LIST_32}"
        fi

        # echo "Searching for ${MYSQL_NAME} in: ${SEARCH_LIST}"
        for i in ${SEARCH_LIST}; do
            # echo "  Checking $i ..."
            if [ -e $i/${MYSQL_NAME} ]; then
                CLIENT_LIBRARY=$i/${MYSQL_NAME}
                break
            fi
        done
    fi
}

FindClientLibrary


if [ -z "${CLIENT_LIBRARY}" ]; then
    echo "Unable to find prerequisite library ${MYSQL_NAME}" 1>&2
    exit 1
fi


# Remove linkage in case it exists
MYSQL_FULLFILENAME=`ldd testMySqlRealQuery | grep libmysqlclient.so | awk '{print $1}'`
MYSQL_BUILD_LIBRARY=`basename ${MYSQL_FULLFILENAME}`
LIBRARY_DIR='/tmp'
[ -e ${LIBRARY_DIR}/${MYSQL_BUILD_LIBRARY} ] && rm ${LIBRARY_DIR}/${MYSQL_BUILD_LIBRARY}
ln -s ${CLIENT_LIBRARY} ${LIBRARY_DIR}/${MYSQL_BUILD_LIBRARY}
