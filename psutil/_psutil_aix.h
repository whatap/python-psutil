#ifndef _PSUTIL_AIX_
#define _PSUTIL_AIX_

// 하위버전 호환용 구조체
typedef struct { /* cpu information */
    char name[IDENTIFIER_LENGTH]; /* logical processor name (cpu0, cpu1, ..) */
    u_longlong_t user;     /* raw number of clock ticks spent in user mode */
    u_longlong_t sys;      /* raw number of clock ticks spent in system mode */
    u_longlong_t idle;     /* raw number of clock ticks spent idle */
    u_longlong_t wait;     /* raw number of clock ticks spent waiting for I/O */
    u_longlong_t pswitch;  /* number of context switches (changes of currently running process) */
    u_longlong_t syscall;  /* number of system calls executed */
    u_longlong_t sysread;  /* number of read system calls executed */
    u_longlong_t syswrite; /* number of write system calls executed */
    u_longlong_t sysfork;  /* number of fork system call executed */
    u_longlong_t sysexec;  /* number of exec system call executed */
    u_longlong_t readch;   /* number of characters tranferred with read system call */
    u_longlong_t writech;  /* number of characters tranferred with write system call */
    u_longlong_t bread;    /* number of block reads */
    u_longlong_t bwrite;   /* number of block writes */
    u_longlong_t lread;    /* number of logical read requests */
    u_longlong_t lwrite;   /* number of logical write requests */
    u_longlong_t phread;   /* number of physical reads (reads on raw device) */
    u_longlong_t phwrite;  /* number of physical writes (writes on raw device) */
    u_longlong_t iget;     /* number of inode lookups */
    u_longlong_t namei;    /* number of vnode lookup from a path name */
    u_longlong_t dirblk;   /* number of 512-byte block reads by the directory search routine to locate an entry for a file */
    u_longlong_t msg;      /* number of IPC message operations */
    u_longlong_t sema;     /* number of IPC semaphore operations  */
    u_longlong_t minfaults;         /* number of page faults with no I/O */
    u_longlong_t majfaults;         /* number of page faults with disk I/O */
    u_longlong_t puser;             /* raw number of physical processor tics in user mode */
    u_longlong_t psys;              /* raw number of physical processor tics in system mode */
    u_longlong_t pidle;             /* raw number of physical processor tics idle */
    u_longlong_t pwait;             /* raw number of physical processor tics waiting for I/O */
    u_longlong_t redisp_sd0;        /* number of thread redispatches within the scheduler affinity domain 0 */
    u_longlong_t redisp_sd1;        /* number of thread redispatches within the scheduler affinity domain 1 */
    u_longlong_t redisp_sd2;        /* number of thread redispatches within the scheduler affinity domain 2 */
    u_longlong_t redisp_sd3;        /* number of thread redispatches within the scheduler affinity domain 3 */
    u_longlong_t redisp_sd4;        /* number of thread redispatches within the scheduler affinity domain 4 */
    u_longlong_t redisp_sd5;        /* number of thread redispatches within the scheduler affinity domain 5 */
    u_longlong_t migration_push;    /* number of thread migrations from the local runque to another queue due to starvation load balancing */
    u_longlong_t migration_S3grq;   /* number of  thread migrations from the global runque to the local runque resulting in a move accross scheduling domain 3 */
    u_longlong_t migration_S3pul;   /* number of  thread migrations from another processor's runque resulting in a move accross scheduling domain 3 */
    u_longlong_t invol_cswitch;     /* number of  involuntary thread context switches */
    u_longlong_t vol_cswitch;       /* number of  voluntary thread context switches */
    u_longlong_t runque;            /* number of  threads on the runque */
    u_longlong_t bound;             /* number of  bound threads */
    u_longlong_t decrintrs;         /* number of decrementer tics interrupts */
    u_longlong_t mpcrintrs;         /* number of mpc's received interrupts */
    u_longlong_t mpcsintrs;         /* number of mpc's sent interrupts */
    u_longlong_t devintrs;          /* number of device interrupts */
    u_longlong_t softintrs;         /* number of offlevel handlers called */
    u_longlong_t phantintrs;        /* number of phantom interrupts */
    u_longlong_t idle_donated_purr; /* number of idle cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_donated_spurr;/* number of idle spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_purr; /* number of busy cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_spurr;/* number of busy spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_stolen_purr;  /* number of idle cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t idle_stolen_spurr; /* number of idle spurr cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_purr;  /* number of busy cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_spurr; /* number of busy spurr cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t hpi; /* number of hypervisor page-ins */
    u_longlong_t hpit; /* Time spent in hypervisor page-ins (in nanoseconds)*/
    u_longlong_t puser_spurr;        /* number of spurr cycles spent in user mode */
    u_longlong_t psys_spurr;        /* number of spurr cycles spent in kernel mode */
    u_longlong_t pidle_spurr;        /* number of spurr cycles spent in idle mode */
    u_longlong_t pwait_spurr;        /* number of spurr cycles spent in wait mode */
    int spurrflag;                  /* set if running in spurr mode */

    /* home SRAD redispatch statistics */
    u_longlong_t localdispatch;             /* number of local thread dispatches on this logical CPU */
    u_longlong_t neardispatch;              /* number of near thread dispatches on this logical CPU */
    u_longlong_t fardispatch;               /* number of far thread dispatches on this logical CPU */
} perfstat_cpu_compatible_t;

typedef struct { /* global cpu information */
    int ncpus;                /* number of active logical processors */
    int ncpus_cfg;             /* number of configured processors */
    char description[IDENTIFIER_LENGTH]; /* processor description (type/official name) */
    u_longlong_t processorHZ; /* processor speed in Hz */
    u_longlong_t user;        /*  raw total number of clock ticks spent in user mode */
    u_longlong_t sys;         /* raw total number of clock ticks spent in system mode */
    u_longlong_t idle;        /* raw total number of clock ticks spent idle */
    u_longlong_t wait;        /* raw total number of clock ticks spent waiting for I/O */
    u_longlong_t pswitch;     /* number of process switches (change in currently running process) */
    u_longlong_t syscall;     /* number of system calls executed */
    u_longlong_t sysread;     /* number of read system calls executed */
    u_longlong_t syswrite;    /* number of write system calls executed */
    u_longlong_t sysfork;     /* number of forks system calls executed */
    u_longlong_t sysexec;     /* number of execs system calls executed */
    u_longlong_t readch;      /* number of characters tranferred with read system call */
    u_longlong_t writech;     /* number of characters tranferred with write system call */
    u_longlong_t devintrs;    /* number of device interrupts */
    u_longlong_t softintrs;   /* number of software interrupts */
    time_t lbolt;              /* number of ticks since last reboot */
    u_longlong_t loadavg[3];  /* (1<<SBITS) times the average number of runnables processes during the last 1, 5 and 15 minutes.    */
    /* To calculate the load average, divide the numbers by (1<<SBITS). SBITS is defined in <sys/proc.h>. */
    u_longlong_t runque;      /* length of the run queue (processes ready) */
    u_longlong_t swpque;      /* ength of the swap queue (processes waiting to be paged in) */
    u_longlong_t bread;       /* number of blocks read */
    u_longlong_t bwrite;      /* number of blocks written */
    u_longlong_t lread;       /* number of logical read requests */
    u_longlong_t lwrite;      /* number of logical write requests */
    u_longlong_t phread;      /* number of physical reads (reads on raw devices) */
    u_longlong_t phwrite;     /* number of physical writes (writes on raw devices) */
    u_longlong_t runocc;      /* updated whenever runque is updated, i.e. the runqueue is occupied.
                               * This can be used to compute the simple average of ready processes  */
    u_longlong_t swpocc;      /* updated whenever swpque is updated. i.e. the swpqueue is occupied.
                               * This can be used to compute the simple average processes waiting to be paged in */
    u_longlong_t iget;        /* number of inode lookups */
    u_longlong_t namei;       /* number of vnode lookup from a path name */
    u_longlong_t dirblk;      /* number of 512-byte block reads by the directory search routine to locate an entry for a file */
    u_longlong_t msg;         /* number of IPC message operations */
    u_longlong_t sema;        /* number of IPC semaphore operations */
    u_longlong_t rcvint;      /* number of tty receive interrupts */
    u_longlong_t xmtint;      /* number of tyy transmit interrupts */
    u_longlong_t mdmint;      /* number of modem interrupts */
    u_longlong_t tty_rawinch; /* number of raw input characters  */
    u_longlong_t tty_caninch; /* number of canonical input characters (always zero) */
    u_longlong_t tty_rawoutch;/* number of raw output characters */
    u_longlong_t ksched;      /* number of kernel processes created */
    u_longlong_t koverf;      /* kernel process creation attempts where:
                               * -the user has forked to their maximum limit
                               *                                                             * -the configuration limit of processes has been reached */
    u_longlong_t kexit;       /* number of kernel processes that became zombies */
    u_longlong_t rbread;      /* number of remote read requests */
    u_longlong_t rcread;      /* number of cached remote reads */
    u_longlong_t rbwrt;       /* number of remote writes */
    u_longlong_t rcwrt;       /* number of cached remote writes */
    u_longlong_t traps;       /* number of traps */
    int ncpus_high;           /* index of highest processor online */
    u_longlong_t puser;       /* raw number of physical processor tics in user mode */
    u_longlong_t psys;        /* raw number of physical processor tics in system mode */
    u_longlong_t pidle;       /* raw number of physical processor tics idle */
    u_longlong_t pwait;       /* raw number of physical processor tics waiting for I/O */
    u_longlong_t decrintrs;   /* number of decrementer tics interrupts */
    u_longlong_t mpcrintrs;   /* number of mpc's received interrupts */
    u_longlong_t mpcsintrs;   /* number of mpc's sent interrupts */
    u_longlong_t phantintrs;  /* number of phantom interrupts */
    u_longlong_t idle_donated_purr; /* number of idle cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_donated_spurr;/* number of idle spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_purr; /* number of busy cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_spurr;/* number of busy spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_stolen_purr;  /* number of idle cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t idle_stolen_spurr; /* number of idle spurr cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_purr;  /* number of busy cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_spurr; /* number of busy spurr cycles stolen by the hypervisor from a dedicated partition */
    short iowait;  /* number of processes that are asleep waiting for buffered I/O */
    short physio;  /* number of processes waiting for raw I/O */
    longlong_t twait;     /* number of threads that are waiting for filesystem direct(cio) */
    u_longlong_t hpi; /* number of hypervisor page-ins */
    u_longlong_t hpit; /* Time spent in hypervisor page-ins (in nanoseconds) */
    u_longlong_t puser_spurr;        /* number of spurr cycles spent in user mode */
    u_longlong_t psys_spurr;        /* number of spurr cycles spent in kernel mode */
    u_longlong_t pidle_spurr;        /* number of spurr cycles spent in idle mode */
    u_longlong_t pwait_spurr;        /* number of spurr cycles spent in wait mode */
    int spurrflag;                  /* set if running in spurr mode */
} perfstat_cpu_total_compatible_t;


typedef union {
    uint    w;
    struct {
        unsigned smt_capable :1;        /* OS supports SMT mode */
        unsigned smt_enabled :1;        /* SMT mode is on */
        unsigned lpar_capable :1;       /* OS supports logical partitioning */
        unsigned lpar_enabled :1;       /* logical partitioning is on */
        unsigned shared_capable :1;     /* OS supports shared processor LPAR */
        unsigned shared_enabled :1;     /* partition runs in shared mode */
        unsigned dlpar_capable :1;      /* OS supports dynamic LPAR */
        unsigned capped :1;             /* partition is capped */
        unsigned kernel_is_64 :1;       /* kernel is 64 bit */
        unsigned pool_util_authority :1;/* pool utilization available */
        unsigned donate_capable :1;     /* capable of donating cycles */
        unsigned donate_enabled :1;     /* enabled for donating cycles */
        unsigned ams_capable:1; /* 1 = AMS(Active Memory Sharing) capable, 0 = Not AMS capable */
        unsigned ams_enabled:1; /* 1 = AMS(Active Memory Sharing) enabled, 0 = Not AMS enabled */
        unsigned spare :18;             /* reserved for future usage */
    } b;
} perfstat_partition_type_compatible_t;



typedef struct { /* partition total information */
    char name[IDENTIFIER_LENGTH]; /* name of the logical partition */
    perfstat_partition_type_compatible_t type; /* set of bits describing the partition */
    int lpar_id;            /* logical partition identifier */
    int group_id;           /* identifier of the LPAR group this partition is a member of */
    int pool_id;            /* identifier of the shared pool of physical processors this partition is a member of */
    int online_cpus;        /* number of virtual CPUs currently online on the partition */
    int max_cpus;           /* maximum number of virtual CPUs this parition can ever have */
    int min_cpus;           /* minimum number of virtual CPUs this partition must have */
    u_longlong_t online_memory; /* amount of memory currently online */
    u_longlong_t max_memory;    /* maximum amount of memory this partition can ever have */
    u_longlong_t min_memory;    /* minimum amount of memory this partition must have */
    int entitled_proc_capacity; /* number of processor units this partition is entitled to receive */
    int max_proc_capacity;      /* maximum number of processor units this partition can ever have */
    int min_proc_capacity;      /* minimum number of processor units this partition must have */
    int proc_capacity_increment;   /* increment value to the entitled capacity */
    int unalloc_proc_capacity;     /* number of processor units currently unallocated in the shared processor pool this partition belongs to */
    int var_proc_capacity_weight;  /* partition priority weight to receive extra capacity */
    int unalloc_var_proc_capacity_weight; /* number of variable processor capacity weight units currently unallocated  in the shared processor pool this partition belongs to */
    int online_phys_cpus_sys;   /* number of physical CPUs currently active in the system containing this partition */
    int max_phys_cpus_sys;  /* maximum possible number of physical CPUs in the system containing this partition */
    int phys_cpus_pool;     /* number of the physical CPUs currently in the shared processor pool this partition belong to */
    u_longlong_t puser;     /* raw number of physical processor tics in user mode */
    u_longlong_t psys;      /* raw number of physical processor tics in system mode */
    u_longlong_t pidle;     /* raw number of physical processor tics idle */
    u_longlong_t pwait;     /* raw number of physical processor tics waiting for I/O */
    u_longlong_t pool_idle_time;    /* number of clock tics a processor in the shared pool was idle */
    u_longlong_t phantintrs;        /* number of phantom interrupts received by the partition */
    u_longlong_t invol_virt_cswitch;  /* number involuntary virtual CPU context switches */
    u_longlong_t vol_virt_cswitch;  /* number voluntary virtual CPU context switches */
    u_longlong_t timebase_last;     /* most recently cpu time base */
    u_longlong_t reserved_pages;     /* Currenlty number of 16GB pages. Cannot participate in DR operations */
    u_longlong_t reserved_pagesize;    /*Currently 16GB pagesize Cannot participate in DR operations */
    u_longlong_t idle_donated_purr; /* number of idle cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_donated_spurr;/* number of idle spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_purr; /* number of busy cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_spurr;/* number of busy spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_stolen_purr;  /* number of idle cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t idle_stolen_spurr; /* number of idle spurr cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_purr;  /* number of busy cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_spurr; /* number of busy spurr cycles stolen by the hypervisor from a dedicated partition */

    /* structure members added through Line item 0SK/KER  feature 576885*/
    u_longlong_t shcpus_in_sys; /* Number of physical processors allocated for shared processor use */
    u_longlong_t max_pool_capacity; /* Maximum processor capacity of partitions pool */
    u_longlong_t entitled_pool_capacity; /* Entitled processor capacity of partitions pool */
    u_longlong_t pool_max_time; /* Summation of maximum time that could be consumed by the pool (nano seconds) */
    u_longlong_t pool_busy_time; /* Summation of busy (non-idle) time accumulated across all partitions in the pool (nano seconds) */
    u_longlong_t pool_scaled_busy_time; /* Scaled summation of busy (non-idle) time accumulated across all partitions in the pool (nano seconds) */
    u_longlong_t shcpu_tot_time; /* Summation of total time across all physical processors allocated for shared processor use (nano seconds) */
    u_longlong_t shcpu_busy_time; /* Summation of busy (non-idle) time accumulated across all shared processor partitions (nano seconds) */
    u_longlong_t shcpu_scaled_busy_time; /* Scaled summation of busy time accumulated across all shared processor partitions (nano seconds) */
    int ams_pool_id; /* AMS pool id of the pool the LPAR belongs to */
    int var_mem_weight; /* variable memory capacity weight */
    u_longlong_t iome; /* I/O memory entitlement of the partition in bytes*/
    u_longlong_t pmem; /* Physical memory currently backing the partition's logical memory in bytes*/
    u_longlong_t hpi; /* number of hypervisor page-ins */
    u_longlong_t hpit; /* Time spent in hypervisor page-ins (in nanoseconds)*/
    u_longlong_t hypv_pagesize; /* Hypervisor page size in KB*/
    uint online_lcpus; /*Number of online logical cpus */
    uint smt_thrds;    /* Number of SMT threads */
    u_longlong_t puser_spurr;        /* number of spurr cycles spent in user mode */
    u_longlong_t psys_spurr;        /* number of spurr cycles spent in kernel mode */
    u_longlong_t pidle_spurr;        /* number of spurr cycles spent in idle mode */
    u_longlong_t pwait_spurr;        /* number of spurr cycles spent in wait mode */
    int spurrflag;                  /* set if running in spurr mode */

} perfstat_partition_total_compatible_t;


typedef struct { /* Description of the network interface */
    char name[IDENTIFIER_LENGTH];   /* name of the interface */
    char description[IDENTIFIER_LENGTH]; /* interface description (from ODM, similar to lscfg output) */
    uchar type;               /* ethernet, tokenring, etc. interpretation can be done using /usr/include/net/if_types.h */
    u_longlong_t mtu;         /* network frame size */
    u_longlong_t ipackets;    /* number of packets received on interface */
    u_longlong_t ibytes;      /* number of bytes received on interface */
    u_longlong_t ierrors;     /* number of input errors on interface */
    u_longlong_t opackets;    /* number of packets sent on interface */
    u_longlong_t obytes;      /* number of bytes sent on interface */
    u_longlong_t oerrors;     /* number of output errors on interface */
    u_longlong_t collisions;  /* number of collisions on csma interface */
    u_longlong_t bitrate;     /* adapter rating in bit per second */
} perfstat_netinterface_compatible_t;

typedef struct { /* Description of the network interfaces */
    int number;               /* number of network interfaces  */
    u_longlong_t ipackets;    /* number of packets received on interface */
    u_longlong_t ibytes;      /* number of bytes received on interface */
    u_longlong_t ierrors;     /* number of input errors on interface */
    u_longlong_t opackets;    /* number of packets sent on interface */
    u_longlong_t obytes;      /* number of bytes sent on interface */
    u_longlong_t oerrors;     /* number of output errors on interface */
    u_longlong_t collisions;  /* number of collisions on csma interface */
} perfstat_netinterface_total_compatible_t;


typedef struct { /* disk information */
    char name[IDENTIFIER_LENGTH];        /* name of the disk */
    char description[IDENTIFIER_LENGTH]; /* disk description (from ODM) */
    char vgname[IDENTIFIER_LENGTH];      /* volume group name (from ODM) */
    u_longlong_t size;              /* size of the disk (in MB) */
    u_longlong_t free;              /* free portion of the disk (in MB) */
    u_longlong_t bsize;             /* disk block size (in bytes) */
    u_longlong_t xrate;             /* OBSOLETE: xrate capability */
#define __rxfers xrate                  /* number of transfers from disk */
    u_longlong_t xfers;             /* number of transfers to/from disk */
    u_longlong_t wblks;             /* number of blocks written to disk */
    u_longlong_t rblks;             /* number of blocks read from disk */
    u_longlong_t qdepth;            /* instantaneous "service" queue depth
                                       (number of requests sent to disk and not completed yet) */
    u_longlong_t time;              /* amount of time disk is active */
    char adapter[IDENTIFIER_LENGTH];/* disk adapter name */
    uint paths_count;               /* number of paths to this disk */
    u_longlong_t  q_full;           /* "service" queue full occurrence count
                                       (number of times the disk is not accepting any more request) */
    u_longlong_t  rserv;            /* read or receive service time */
    u_longlong_t  rtimeout;         /* number of read request timeouts */
    u_longlong_t  rfailed;          /* number of failed read requests */
    u_longlong_t  min_rserv;        /* min read or receive service time */
    u_longlong_t  max_rserv;        /* max read or receive service time */
    u_longlong_t  wserv;            /* write or send service time */
    u_longlong_t  wtimeout;         /* number of write request timeouts */
    u_longlong_t  wfailed;          /* number of failed write requests */
    u_longlong_t  min_wserv;        /* min write or send service time */
    u_longlong_t  max_wserv;        /* max write or send service time */
    u_longlong_t  wq_depth;         /* instantaneous wait queue depth
                                       (number of requests waiting to be sent to disk) */
    u_longlong_t  wq_sampled;       /* accumulated sampled dk_wq_depth */
    u_longlong_t  wq_time;          /* accumulated wait queueing time */
    u_longlong_t  wq_min_time;      /* min wait queueing time */
    u_longlong_t  wq_max_time;      /* max wait queueing time */
    u_longlong_t  q_sampled;        /* accumulated sampled dk_q_depth */
    cid_t         wpar_id;          /* WPAR identifier.*/
    /* Pad of 3 short is available here*/
} perfstat_disk_compatible_t;

typedef struct { /* global disk information */
    int number;          /* total number of disks */
    u_longlong_t size;   /* total size of all disks (in MB) */
    u_longlong_t free;   /* free portion of all disks (in MB) */
    u_longlong_t xrate;  /* __rxfers: total number of transfers from disk */
    u_longlong_t xfers;  /* total number of transfers to/from disk */
    u_longlong_t wblks;  /* 512 bytes blocks written to all disks */
    u_longlong_t rblks;  /* 512 bytes blocks read from all disks */
    u_longlong_t time;   /* amount of time disks are active */
    cid_t   wpar_id;     /* WPAR identifier.*/
    /* Pad of 3 short is available here*/
} perfstat_disk_total_compatible_t;

typedef struct { /* Disk adapter information */
    char name[IDENTIFIER_LENGTH];        /* name of the adapter (from ODM)*/
    char description[IDENTIFIER_LENGTH]; /* adapter description (from ODM)*/
    int number;          /* number of disks connected to adapter */
    u_longlong_t size;   /* total size of all disks (in MB)  */
    u_longlong_t free;   /* free portion of all disks (in MB)  */
    u_longlong_t xrate;  /* __rxfers: total number of reads via adapter */
    u_longlong_t xfers;  /* total number of transfers via adapter */
    u_longlong_t rblks;  /* 512 bytes blocks written via adapter */
    u_longlong_t wblks;  /* 512 bytes blocks read via adapter  */
    u_longlong_t time;   /* amount of time disks are active */
} perfstat_diskadapter_compatible_t;

typedef struct { /* Virtual memory utilization */
    u_longlong_t virt_total;    /* total virtual memory (in 4KB pages) */
    u_longlong_t real_total;    /* total real memory (in 4KB pages) */
    u_longlong_t real_free;     /* free real memory (in 4KB pages) */
    u_longlong_t real_pinned;   /* real memory which is pinned (in 4KB pages) */
    u_longlong_t real_inuse;    /* real memory which is in use (in 4KB pages) */
    u_longlong_t pgbad;         /* number of bad pages */
    u_longlong_t pgexct;        /* number of page faults */
    u_longlong_t pgins;         /* number of pages paged in */
    u_longlong_t pgouts;        /* number of pages paged out */
    u_longlong_t pgspins;       /* number of page ins from paging space */
    u_longlong_t pgspouts;      /* number of page outs from paging space */
    u_longlong_t scans;         /* number of page scans by clock */
    u_longlong_t cycles;        /* number of page replacement cycles */
    u_longlong_t pgsteals;      /* number of page steals */
    u_longlong_t numperm;       /* number of frames used for files (in 4KB pages) */
    u_longlong_t pgsp_total;    /* total paging space (in 4KB pages) */
    u_longlong_t pgsp_free;     /* free paging space (in 4KB pages) */
    u_longlong_t pgsp_rsvd;     /* reserved paging space (in 4KB pages) */
    u_longlong_t real_system;   /* real memory used by system segments (in 4KB pages). This is the sum of all the used pages in segment marked for system usage.
                                 * Since segment classifications are not always guaranteed to be accurate, this number is only an approximation. */
    u_longlong_t real_user;     /* real memory used by non-system segments (in 4KB pages). This is the sum of all pages used in segments not marked for system usage.
                                 * Since segment classifications are not always guaranteed to be accurate, this number is only an approximation. */
    u_longlong_t real_process;  /* real memory used by process segments (in 4KB pages). This is real_total-real_free-numperm-real_system. Since real_system is an
                                 * approximation, this number is too. */
    u_longlong_t virt_active;   /* Active virtual pages. Virtual pages are considered active if they have been accessed */
    u_longlong_t iome; /* I/O memory entitlement of the partition in bytes*/
    u_longlong_t iomu; /* I/O memory entitlement of the partition in use in bytes*/
    u_longlong_t iohwm; /* High water mark of I/O memory entitlement used in bytes*/
    u_longlong_t pmem; /* Amount of physical mmeory currently backing partition's logical memory in bytes*/
} perfstat_memory_total_compatible_t;



#endif
