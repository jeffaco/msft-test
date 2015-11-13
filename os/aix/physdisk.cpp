#include <assert.h>
#include <odmi.h>
#include <stdio.h>
#include <string>
#include <vector>

#include <sys/cfgodm.h>

// Decide which disk we're getting data on
char *criteria = "name=hdisk2";

// Debug output when decoding VPD data?
const int debug_output = 1;

// Decodes a VPD (Vital Product Data) on the AIX platform.
//
// A VPD structure can be in two forms:
// 1. Section 12.5 of document, or
// 2. From IBM's old microchannel bus machines which IBM still uses for
//    devices that are still around from those days.
//
// This information comes via a personal contact at IBM, and is not
// formally documented by IBM.
//
// The old microchannel bus format will always start with an asterick (*)
// character.  In general, it can be described as consisting of one or more
// VPD keywords with each one having the following structure:
//
//    *KKLdd...d
//
// where:
//    *       = The asterick character ('*')
//    KK      = A two-character keyword
//    L       = A one byte binary value of half the length of the entire
//              "*KKLdd...d" string of bytes.  In other words, 2*L is the
//              length of the string of bytes. 
//    dd....d = The string of actual VPD data for the keyword.  This may be
//              binary or ascii string (not NULL terminated) depending on the
//              keyword.
//
// Note: 2*L includes the 4 bytes for the *KKL bytes as well as the number of
// bytes in dd...d.  Also note that because L has to be doubled, the length
// is always an even number of bytes.  If there is an odd number of bytes in
// the dd...d value, then it must be padded with an 0x00 byte.
//
// There should be an 0x00 byte following the last *KKdd...d.  So you process
// the data in a loop looking for * characters.  The L value can be used to
// determine where the next '*' character should be.  However, if it is 0x00,
// then you are done.

void decodeVPD(struct CuVPD *vpdItem)
{
    printf("VPD Name:      %s\n", vpdItem->name);

    char *p = vpdItem->vpd;
    if (*p != '*')
        return;

    std::string tag;
    std::vector<char> value;
    value.resize(sizeof(vpdItem->vpd) + 1, 0);

    while (*p != '\0') {
        // Length includes "*xxl", where XX=2 char ID and l==total len
        int totalLen = ( p[3] * 2 );
        int itemLen = totalLen - 4;

        tag.clear();
        tag.push_back( p[1] );
        tag.push_back( p[2] );

        assert( itemLen < sizeof(vpdItem->vpd) );
        memcpy(&value[0], &p[4], itemLen);
        value[itemLen] = '\0';

        if (debug_output) {
            printf("  Tag: %s, Value: %s\n", &tag[0], &value[0]);
        }

        if (0 == tag.compare("MF"))
            printf("Manufacturer:  %s\n", &value[0]);
        else if (0 == tag.compare("TM"))
            printf("Model:         %s\n", &value[0]);

        p += totalLen;
    }

    return;
}

int main()
{
    int res;

    res = odm_initialize();
    if (res) {
        printf("Error on odm_initialize: %d (error %d)\n", res, odmerrno);
        return res;
    }

    // First get the VPD data, which gives us manufacturer and model
    void *pData;
    struct CuVPD vpdData;

    pData = odm_get_first(CuVPD_CLASS, criteria, &vpdData);
    if ((int) pData == -1) {
        printf("Error on odm_get_first: %d\n", odmerrno);
    }
    else if (pData == NULL) {
        printf("No match found for criteria \"%s\"\n", criteria);
    }
    else {
        assert( pData == &vpdData );
        decodeVPD(&vpdData);
    }

    // Next get the CuAT object, which gives us the size
    char sizeCriteria[128];
    strcpy(sizeCriteria, criteria);
    strcat(sizeCriteria, " and attribute=size_in_mb");

    struct CuAt atData;

    pData = odm_get_first(CuAt_CLASS, sizeCriteria, &atData);
    if ((int) pData == -1) {
        printf("Error on odm_get_first: %d\n", odmerrno);
    }
    else if (pData == NULL) {
        printf("Size not available (no size_in_mb attribute)\n");
    }
    else {
        assert( pData == &atData );
        printf("Size (MB):     %s\n", atData.value);
    }

    // Finally, get the CuDv class, to give us the interface type
    struct CuDv dvData;

    pData = odm_get_first(CuDv_CLASS, criteria, &dvData);
    if ((int) pData == -1) {
        printf("Error on odm_get_first: %d\n", odmerrno);
    }
    else if (pData == NULL) {
        printf("No match found for criteria \"%s\"\n", sizeCriteria);
    }
    else {
        assert( pData == &dvData );
        printf("Device Interface: %s\n", dvData.PdDvLn_Lvalue);
    }

    // Clean up resources
    res = odm_terminate();
    if (res) {
        printf("Error on odm_terminate: %d (error %d)\n", res, odmerrno);
        return res;
    }

    return 0;
}
