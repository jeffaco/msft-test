
#include <sys/types.h>
#include <procinfo.h>
#include <iostream>
#include <vector>

class ProcLister {
    static const size_t burst_size = 10; //!< Number of entries to read per burst

public:
    /** Starts a new process iterator */
    ProcLister() : m_index(0), m_procIndex(0), m_burst(0), m_handle(0) {
        m_procs.reserve( burst_size );
        readburst();
    }

    /** Deallocates memory */
    ~ProcLister() {
    }

    /**
     * Advances iterator to next process.
     * \returns false if the iterator has reached the end
     */
    bool nextProc() {
        if (m_index >= m_burst) {
            if (readburst() == false) {
                return false;
            }
        }
        m_handle = &m_procs[m_index];
        m_index++;
        return true;
    }

    /**
     * Gets data structure for next process
     * \returns process data in AIX specific format
     */
    struct procentry64 *getHandle() { return m_handle; }

    /**
     * Returns the pid of the next process.
     * \returns the process id
     */
    pid_t getPid() { return m_handle->pi_pid; }

private:
    /**
     * Reads a variable size burst of process data entries.
     * \returns true if method was able to read process data
     */
    bool readburst() {
        std::cerr << "Calling getprocs64, sizeof=" << sizeof(struct procentry64) << ", indexP=" << m_procIndex << ", burst_size=" << burst_size << "... ";
        m_burst = getprocs64(&m_procs[0], sizeof(struct procentry64), 0, 0, &m_procIndex, burst_size);
        std::cerr << "Results: m_burst=" << m_burst << ", errno=" << errno << std::endl;
        if (m_burst < 0) { std::cerr << "Fatal error, errno=" << errno << ", aborting!" << std::endl; exit(1); }
        m_index = 0;
        return m_burst != 0;
    }

    int m_index;                 //!< Next index within current m_procs array
    pid_t m_procIndex;           //!< Process identifier of required process table entry
    int m_burst;                 //!< Number of items in last read burst
    struct procentry64 *m_handle; //!< Pointer to data that is current in iterator
    std::vector< struct procentry64 > m_procs; //!< Array that holds the current burst for procentry64
};


int main()
{
/*
    pid_t firstproc = 0;
    struct procentry64 pe;

    int count = getprocs64(&pe, sizeof(pe), 0, 0, &firstproc, 1);
    if (count != 1)
    {
        std::cerr << "Results: count=" << count << ", errno=" << errno << std::endl;
        return 1;
    }
*/

    int count = 0;

    ProcLister pl;
    while (pl.nextProc())
    {
        struct procentry64 *pent = pl.getHandle();
        if (pent == NULL || ((pent->pi_flags & SKPROC) != 0))
            continue;

        std::cout << "PID:  " << pent->pi_pid << "\tName: " << pent->pi_comm << "\tPrio: " << pent->pi_nice << std::endl;
        count++;
    }

    std::cout << "Success ... found " << count << " processes" << std::endl;
    return 0;
}
