/* Code to test the pstat_getpathname code */
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/fcntl.h>

#include <sys/pstat.h>

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#define BURST ((size_t)10)
class ProcLister {
public:
    ProcLister() : m_idx(0) {
        readburst();
    }
    
    bool nextProc(struct pst_status* &pstp) {
        if (m_index >= m_burstsize) {
            if (readburst() == false) {
                return false;
            }
        }
        pstp = &pst[m_index];
        m_index++;
        return true;
    }
    


private:
    bool readburst() {
        cout << "Reading burst at " << m_idx << endl;

        m_burstsize = pstat_getproc(pst, sizeof(pst[0]), BURST, m_idx);
        if (m_burstsize < 0) throw "Jaix";
        m_idx = m_burstsize > 0 ? pst[m_burstsize - 1].pst_idx + 1 : -1;
        m_index = 0;
        return m_burstsize != 0;
    }
    
    int m_index;                // Next index within current pst array
    int m_idx;                  // Next index after currect burst
    int m_burstsize;            // Number of items in last read burst
    struct pst_status pst[BURST];

};

void
testProcLister()
{
  char filepath[256];
  char charbuf[256];
  string pth, pth_path;
  struct pst_status* pstp;
  ProcLister p;
  
  while (p.nextProc(pstp)) {
    int count = pstat_getpathname(filepath, sizeof(filepath), 
                                  const_cast<pst_fid *>(&(pstp->pst_fid_text)));
    if (count > 0) {
      pth.assign(filepath, count);
      pth_path.assign(filepath, count);
    } else {
      size_t cnt = sprintf(charbuf, "(Errno = %d)", errno);
      pth.assign(charbuf, cnt);
      pth_path.assign(pstp->pst_ucomm);
    }

    cout << "pid=" << pstp->pst_pid 
         << "\tcomm=" << pstp->pst_ucomm 
         << "\tcount=" << count
         << "\tpath=" << pth
         << "\treturn=" << pth_path
         << endl;
  }
}


main()
{
    testProcLister();
}
