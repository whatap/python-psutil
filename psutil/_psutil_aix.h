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
    u_longlong_t minfaults;     /* number of page faults with no I/O */
    u_longlong_t majfaults;     /* number of page faults with disk I/O */
    u_longlong_t puser;         /* raw number of physical processor tics in user mode */
    u_longlong_t psys;          /* raw number of physical processor tics in system mode */
    u_longlong_t pidle;         /* raw number of physical processor tics idle */
    u_longlong_t pwait;         /* raw number of physical processor tics waiting for I/O */
    u_longlong_t redisp_sd0;    /* number of thread redispatches within the scheduler affinity domain 0 */
    u_longlong_t redisp_sd1;    /* number of thread redispatches within the scheduler affinity domain 1 */
    u_longlong_t redisp_sd2;    /* number of thread redispatches within the scheduler affinity domain 2 */
    u_longlong_t redisp_sd3;    /* number of thread redispatches within the scheduler affinity domain 3 */
    u_longlong_t redisp_sd4;    /* number of thread redispatches within the scheduler affinity domain 4 */
    u_longlong_t redisp_sd5;    /* number of thread redispatches within the scheduler affinity domain 5 */
    u_longlong_t migration_push;    /* number of thread migrations from the local runque to another queue due to starvation load balancing */
    u_longlong_t migration_S3grq;   /* number of  thread migrations from the global runque to the local runque resulting in a move accross scheduling domain 3 */
    u_longlong_t migration_S3pul;   /* number of  thread migrations from another processor's runque resulting in a move accross scheduling domain 3 */
    u_longlong_t invol_cswitch;     /* number of  involuntary thread context switches */
    u_longlong_t vol_cswitch;       /* number of  voluntary thread context switches */
    u_longlong_t runque;            /* number of  threads on the runque */
    u_longlong_t bound;         /* number of  bound threads */
    u_longlong_t decrintrs;     /* number of decrementer tics interrupts */
    u_longlong_t mpcrintrs;     /* number of mpc's received interrupts */
    u_longlong_t mpcsintrs;     /* number of mpc's sent interrupts */
    u_longlong_t devintrs;      /* number of device interrupts */
    u_longlong_t softintrs;     /* number of offlevel handlers called */
    u_longlong_t phantintrs;    /* number of phantom interrupts */
    u_longlong_t idle_donated_purr; /* number of idle cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_donated_spurr;/* number of idle spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_purr; /* number of busy cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_spurr;/* number of busy spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_stolen_purr;  /* number of idle cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t idle_stolen_spurr; /* number of idle spurr cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_purr;  /* number of busy cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_spurr; /* number of busy spurr cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t hpi;               /* number of hypervisor page-ins */
    u_longlong_t hpit;              /* Time spent in hypervisor page-ins (in nanoseconds)*/
    u_longlong_t puser_spurr;       /* number of spurr cycles spent in user mode */
    u_longlong_t psys_spurr;        /* number of spurr cycles spent in kernel mode */
    u_longlong_t pidle_spurr;       /* number of spurr cycles spent in idle mode */
    u_longlong_t pwait_spurr;       /* number of spurr cycles spent in wait mode */
    int spurrflag;                  /* set if running in spurr mode */

    /* home SRAD redispatch statistics */
    u_longlong_t localdispatch;     /* number of local thread dispatches on this logical CPU */
    u_longlong_t neardispatch;      /* number of near thread dispatches on this logical CPU */
    u_longlong_t fardispatch;       /* number of far thread dispatches on this logical CPU */
    u_longlong_t cswitches;         /* Context switches */
    u_longlong_t  version;          /* version number (1, 2, etc.,) */
    u_longlong_t tb_last;           /* timebase counter */
    /*  >>>>> END OF STRUCTURE DEFINITION <<<<<     */
//#define CURR_VERSION_CPU 2 /* Incremented by one for every new release *
//                            * of perfstat_cpu_t data structure         */
} perfstat_cpu_compatible_t;

typedef struct { /* partition total information */
    char name[IDENTIFIER_LENGTH];       /* name of the logical partition */
    perfstat_partition_type_t type;     /* set of bits describing the partition */
    int lpar_id;                        /* logical partition identifier */
    int group_id;                       /* identifier of the LPAR group this partition is a member of */
    int pool_id;                        /* identifier of the shared processor pool of physical processors, to which this partition belongs*/
    int online_cpus;                    /* number of virtual CPUs currently online on the partition */
    int max_cpus;                       /* maximum number of virtual CPUs this parition can ever have */
    int min_cpus;                       /* minimum number of virtual CPUs this partition must have */
    u_longlong_t online_memory;         /* amount of memory currently online */
    u_longlong_t max_memory;            /* maximum amount of memory this partition can ever have */
    u_longlong_t min_memory;            /* minimum amount of memory this partition must have */
    int entitled_proc_capacity;         /* number of processor units this partition is entitled to receive */
    int max_proc_capacity;              /* maximum number of processor units this partition can ever have */
    int min_proc_capacity;              /* minimum number of processor units this partition must have */
    int proc_capacity_increment;        /* increment value to the entitled capacity */
    int unalloc_proc_capacity;          /* unallocated processor units in the shared processor pool, to which partition belongs */
    int var_proc_capacity_weight;       /* unallocated variable processor capacity weight units in the shared processor pool, to which this partition belongs */
    int unalloc_var_proc_capacity_weight; /* number of variable processor capacity weight units currently unallocated  in the shared processor pool this partition belongs to */
    int online_phys_cpus_sys;           /* number of physical CPUs currently active in the system containing this partition */
    int max_phys_cpus_sys;              /* maximum possible number of physical CPUs in the system containing this partition */
    int phys_cpus_pool;                 /* available physical CPUs in the shared processor pool, to which this partition belongs */
    u_longlong_t puser;                 /* raw number of physical processor tics in user mode */
    u_longlong_t psys;                  /* raw number of physical processor tics in system mode */
    u_longlong_t pidle;                 /* raw number of physical processor tics idle */
    u_longlong_t pwait;                 /* raw number of physical processor tics waiting for I/O */
    u_longlong_t pool_idle_time;        /* summation of -idle time (in nano seconds) accumulated across all partitions in the shared processor pool, to which this partition belongs */
    u_longlong_t phantintrs;            /* number of phantom interrupts received by the partition */
    u_longlong_t invol_virt_cswitch;    /* number involuntary virtual CPU context switches */
    u_longlong_t vol_virt_cswitch;      /* number voluntary virtual CPU context switches */
    u_longlong_t timebase_last;         /* Total number of clock ticks */
    u_longlong_t reserved_pages;        /* Currenlty number of 16GB pages. Cannot participate in DR operations */
    u_longlong_t reserved_pagesize;     /*Currently 16GB pagesize Cannot participate in DR operations */
    u_longlong_t idle_donated_purr;     /* number of idle cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_donated_spurr;    /* number of idle spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_purr;     /* number of busy cycles donated by a dedicated partition enabled for donation */
    u_longlong_t busy_donated_spurr;    /* number of busy spurr cycles donated by a dedicated partition enabled for donation */
    u_longlong_t idle_stolen_purr;      /* number of idle cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t idle_stolen_spurr;     /* number of idle spurr cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_purr;      /* number of busy cycles stolen by the hypervisor from a dedicated partition */
    u_longlong_t busy_stolen_spurr;     /* number of busy spurr cycles stolen by the hypervisor from a dedicated partition */

    /* structure members added through Line item 0SK/KER  feature 576885*/
    u_longlong_t shcpus_in_sys;         /* number of physical processors allocated for shared processor pool use, across all shared processorss pool (i.e pool id=0) */
    u_longlong_t max_pool_capacity;     /* Maximum processor capacity of shared processor pool, to which this partition belongs  */
    u_longlong_t entitled_pool_capacity;/* Entitled processor capacity of shared processor pool, to which this partition belongs */
    u_longlong_t pool_max_time;         /* Summation of maximum time (in nano seconds) that could be consumed by the shared processor pool, to which this partition belong */
    u_longlong_t pool_busy_time;        /* Summation of busy (non-idle) time (in nano seconds) accumulated across all partitions in the shared processor pool, to which this partition belongs */
    u_longlong_t pool_scaled_busy_time; /* Summation of busy (non-idle) time (in nano seconds) accumulated across all partitions in the shared processor pool, to which this partition belongs.
                                           The time against rated/nominal frequency  */
    u_longlong_t shcpu_tot_time;        /* Summation of total time (in nano seconds) across all physical processors allocated for shared processor (i.e Pool ID=0) use, across all shared                                                       processor pools. */
    u_longlong_t shcpu_busy_time;       /* Summation of busy (non-idle) time (in nano seconds)accumulated across all physical processors for shared processor (i.e Pool ID=0) use, across all                                                   shared processor pools. */
    u_longlong_t shcpu_scaled_busy_time;/* summation of scaled busy (non-idle ) time  (in nano seconds) accumulated across all physical processors for Shared Processor Pool (i.e Pool ID=0)use,                                                across all share processor pools. The time against rated/nominal frequency */
    int ams_pool_id;                    /* AMS pool id of the pool the LPAR belongs to */
    int var_mem_weight;                 /* variable memory capacity weight */
    u_longlong_t iome;                  /* I/O memory entitlement of the partition in bytes*/
    u_longlong_t pmem;                  /* Physical memory currently backing the partition's logical memory in bytes*/
    u_longlong_t hpi;                   /* number of hypervisor page-ins */
    u_longlong_t hpit;                  /* Time spent in hypervisor page-ins (in nanoseconds)*/
    u_longlong_t hypv_pagesize;         /* Hypervisor page size in KB*/
    uint online_lcpus;                  /*Number of online logical cpus */
    uint smt_thrds;                     /* Number of SMT threads */
    u_longlong_t puser_spurr;           /* number of spurr cycles spent in user mode */
    u_longlong_t psys_spurr;            /* number of spurr cycles spent in kernel mode */
    u_longlong_t pidle_spurr;           /* number of spurr cycles spent in idle mode */
    u_longlong_t pwait_spurr;           /* number of spurr cycles spent in wait mode */
    int spurrflag;                      /* set if running in spurr mode */
    char hardwareid[CEC_ID_LEN];        /* CEC Identifier */
    uint power_save_mode;               /*Power save mode for the LPAR. Introduced through LI 53K PRF : Feature 728292*/
    ushort ame_version;                 /* AME Version */
    u_longlong_t true_memory;           /* True Memory Size in 4KB pages */
    u_longlong_t expanded_memory;       /* Expanded Memory Size in 4KB pages */
    u_longlong_t target_memexp_factr;   /* Target Memory Expansion Factor scaled by 100 */
    u_longlong_t current_memexp_factr;  /* Current Memory Expansion Factor scaled by 100 */
    u_longlong_t target_cpool_size;     /* Target Compressed Pool Size in bytes */
    u_longlong_t max_cpool_size;        /* Max Size of Compressed Pool in bytes */
    u_longlong_t min_ucpool_size;       /* Min Size of Uncompressed Pool in bytes */
    u_longlong_t ame_deficit_size;      /*Deficit memory size in bytes */
    u_longlong_t version;               /* version number (1, 2, etc.,) */
    u_longlong_t cmcs_total_time;       /* Total CPU time spent due to active memory expansion */
    u_longlong_t purr_coalescing;       /* If the calling partition is
                                         * authorized to see pool wide statistics then
                                         *                                          * PURR cycles consumed to coalesce data
                                         *                                                                                   * else set to zero.*/
    u_longlong_t spurr_coalescing;      /* If the calling partition is
                                         * authorized to see pool wide statistics then
                                         *                                          * SPURR cycles consumed to coalesce data
                                         *                                                                                   * else set to zero.*/
    u_longlong_t  MemPoolSize ;         /* Indicates the memory pool size
                                         * of the pool that the partition belongs to (in bytes)., mpsz */

    u_longlong_t   IOMemEntInUse;       /* I/O memory entitlement of the LPAR in use in bytes. iomu */

    u_longlong_t   IOMemEntFree;        /* free I/O memory entitlement in bytes.  iomf */

    u_longlong_t   IOHighWaterMark;     /* high water mark of I/O memory entitlement usage in bytes. iohwn */


    /*  >>>>> END OF STRUCTURE DEFINITION <<<<<     */
//#define CURR_VERSION_PARTITION_TOTAL 4  /* Incremented by one for every new release     *
//                                         * of perfstat_partition_total_t data structure */
} perfstat_partition_compatible_total_t;


#endif
