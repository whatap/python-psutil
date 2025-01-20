#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/pstat.h>
#include <sys/param.h>
#include <sys/dk.h>
#include <sys/unistd.h>
#include <sys/mib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <mntent.h>
#include <unistd.h>
#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <Python.h>
#include <pwd.h>
#include "_psutil_common.h"


#define TV2MICRO(t)   (((t).pst_usec * 0.001) + ((t).pst_sec * 1000))

static PyObject *psutil_proc_cpu_num(PyObject *self, PyObject *args) {
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getdynamic failed");
        return NULL;
    }
    //return psd.psd_proc_cnt;
    return Py_BuildValue("i", psd.psd_proc_cnt);
}


static PyObject *psutil_per_cpu_times(PyObject *self, PyObject *args) {
   struct pst_processor psp;
    PyObject *py_retlist = PyList_New(0);
    PyObject *py_cputime = NULL;

    int i = 0;
    //{"USER","NICE","SYS","IDLE","WAIT","BLOCK","SWAIT","INTR","SSYS"};
    while (pstat_getprocessor(&psp, sizeof(struct pst_processor), 1, i++) > 0) {
        if (psp.psp_pset_id == -1) continue;
        py_cputime = Py_BuildValue(
                "(KKKKKK)",
#ifdef IS_HPUX_11_31
                psp.psp_core_id,
#else
                i-1,
#endif
                psp.psp_cpu_time[CP_USER],
                psp.psp_cpu_time[CP_SYS],
                psp.psp_cpu_time[CP_IDLE],
                psp.psp_cpu_time[CP_SWAIT] + psp.psp_cpu_time[CP_BLOCK] + psp.psp_cpu_time[CP_WAIT],
                psp.psp_cpu_time[CP_NICE]);
        if (!py_cputime) {
            return NULL;
        }
        if (PyList_Append(py_retlist, py_cputime)) {
            return NULL;
        }
        Py_DECREF(py_cputime);
    }

    return py_retlist; 
}

// load
static PyObject *psutil_cpu_load(PyObject *self, PyObject *args) {
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getdynamic failed");
        return NULL;
    }
    return Py_BuildValue(
            "ddd",
            psd.psd_avg_1_min,
            psd.psd_avg_5_min,
            psd.psd_avg_15_min
            );


}

static PyObject *
psutil_cpu_stats_detail(PyObject *self, PyObject *args) {
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getdynamic failed");
        return NULL;
    }

    struct pst_vminfo psv;
    if (pstat_getvminfo(&psv, sizeof(psv), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getvminfo failed");
        return NULL;
    }

    //unsigned long long runq = 0;
    unsigned long long runq = psd.psd_rq;
    //unsigned long long blockq = psv.psv_swpque;
    unsigned long long blockq = psd.psd_dw + psd.psd_pw;
    unsigned long long waitq = psd.psd_sw;
    unsigned long long fork = psv.psv_cntfork;
    unsigned long long interrupt = psv.psv_sintr;
    unsigned long long syscall = psv.psv_ssyscall;
    unsigned long long ctxsw = psv.psv_sswtch;
    unsigned long long exec = 0;
 
    struct pst_processor psp;

    int i = 0;
    while (pstat_getprocessor(&psp, sizeof(struct pst_processor), 1, i++) > 0) {
        if (psp.psp_pset_id == -1) continue;
        exec += psp.psp_sysexec;
    }

    return Py_BuildValue(
        "KKKKKKKKK",
        ctxsw,
        interrupt,
        syscall,
        fork,
        exec,
        runq,
        blockq,
        waitq
    );
}
// interrupts
// syscall
// ctxswitch
static PyObject *psutil_cpu_stats(PyObject *self, PyObject *args) {
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getdynamic failed");
        return NULL;
    }
 
    size_t ncpu = psd.psd_proc_cnt;
    struct pst_processor *psp;

    psp = (struct pst_processor *)malloc(ncpu * sizeof(struct pst_processor));
    if (pstat_getprocessor(psp, sizeof(struct pst_processor), ncpu, 0) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getprocessor failed");
        return NULL;
    }

    PyObject *py_retlist = PyList_New(0);
    PyObject *py_cputime = NULL;

    int i = 0;

    //{"USER","NICE","SYS","IDLE","WAIT","BLOCK","SWAIT","INTR","SSYS"};
    for (i = 0; i < ncpu; i++) {
        py_cputime = Py_BuildValue(
                "(iiii)",
                0,
                psp[i].psp_cpu_time[7],
                0,
                psp[i].psp_cpu_time[8]
                );
        if (!py_cputime) {
            free(psp);
            return NULL;
        }
        if (PyList_Append(py_retlist, py_cputime)) {
            free(psp);
            return NULL;
        }
        Py_DECREF(py_cputime);
    }

    free(psp);
    return py_retlist; 

}




static PyObject *psutil_virtual_memory_detail(PyObject *self, PyObject *args) {
    struct pst_static pst;
    struct pst_dynamic psd;
    struct pst_vminfo psv;


    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getstatic failed");
        return NULL;
    }

    int page_size = pst.page_size;

    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getdynamic failed");
        return NULL;
    }

    if (pstat_getvminfo(&psv, sizeof(psv), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getvminfo failed");
        return NULL;
    }
 
    unsigned long long total = (unsigned long long)pst.physical_memory * page_size;
    unsigned long long free = (unsigned long long)psd.psd_free * page_size;
    unsigned long long cached = (unsigned long long)psv.psv_kern_dynmem * page_size;
    unsigned long long used = total - free;
    unsigned long long available = free;
    unsigned long long swapin = (unsigned long long)psv.psv_spswpin;
    unsigned long long swapout = (unsigned long long)psv.psv_spswpout;
    unsigned long long filein = (unsigned long long)psv.psv_spgpgin;
    unsigned long long fileout = (unsigned long long)psv.psv_spgpgout;
    unsigned long long scanned = (unsigned long long)psv.psv_sscan;
    unsigned long long freed = (unsigned long long)psv.psv_sdfree;

    return Py_BuildValue(
            "KKKKKiKKKKKK",
            total,
            available,
            used,
            free,
            cached,
            page_size,
            swapin,
            swapout,
            filein,
            fileout,
            scanned,
            freed
            );
}

static PyObject *psutil_total_memory(PyObject *self, PyObject *args) {
    struct pst_static pst;

    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getstatic failed");
        return NULL;
    }
    return Py_BuildValue("K", (unsigned long long)pst.physical_memory * (unsigned long long)pst.page_size);
}

static PyObject *psutil_virtual_memory(PyObject *self, PyObject *args) {
    struct pst_static pst;
    struct pst_dynamic psd;
    struct pst_vminfo psv;


    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getstatic failed");
        return NULL;
    }

    int page_size = pst.page_size;

    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getdynamic failed");
        return NULL;
    }

    if (pstat_getvminfo(&psv, sizeof(psv), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getvminfo failed");
        return NULL;
    }
 
    unsigned long long total = (unsigned long long)pst.physical_memory * page_size;
    unsigned long long free = (unsigned long long)psd.psd_free * page_size;
    unsigned long long cached = (unsigned long long)psv.psv_kern_dynmem * page_size;
    unsigned long long used = total - free;
    unsigned long long available = free;

    return Py_BuildValue(
            "KKKKKi",
            total,
            available,
            used,
            free,
            cached,
            page_size
            );
}

static PyObject *psutil_swap_memory(PyObject *self, PyObject *args) {
    struct pst_swapinfo pss;
    unsigned long long swap_total = 0;
    unsigned long long swap_free = 0;
    unsigned long long swap_used = 0;
    int i = 0;


    while (pstat_getswap(&pss, sizeof(pss), 1, i++) > 0) {
        pss.pss_nfpgs *= 4;  /* nfpgs is in 512 byte blocks */

        if (pss.pss_nblksenabled == 0) {
            pss.pss_nblksenabled = pss.pss_nfpgs;
        }

        swap_total += pss.pss_nblksenabled;
        swap_free  += pss.pss_nfpgs;
    }

    swap_used = swap_total - swap_free;


    struct pst_vminfo psv;

    pstat_getvminfo(&psv, sizeof(psv), 1, 0);

    unsigned long long page_in = psv.psv_spgin;
    unsigned long long page_out = psv.psv_spgout;

    return Py_BuildValue(
            "KKKKK",
            swap_total,
            swap_free,
            swap_used,
            page_in,
            page_out
            );
}

static PyObject *psutil_boot_time(PyObject *self, PyObject *args) {
    struct pst_static pst;

    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1 ) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getstatic failed");
        return NULL;
    }

    return Py_BuildValue(
            "I",
            pst.boot_time);
}


static PyObject *psutil_disk_info(PyObject *self,  PyObject *args) {
    const char *disk_path = "/dev/disk";
    const char *dsk_path = "/dev/dsk";
    const char *dir_path = NULL;
    DIR *dp;

    if (access(disk_path, F_OK) == 0) {
        dp = opendir(disk_path);
        dir_path = disk_path;
        if (dp == NULL) {
            return NULL;
        }
    } else if (access(dsk_path, F_OK) == 0) {
        dp = opendir(dsk_path);
        dir_path = dsk_path;
        if (dp == NULL) {
            return NULL;
        }
    } else {
        return NULL;
    }

    struct dirent *entry;
    struct stat sb;

    PyObject *py_retdict = PyDict_New();
    PyObject *py_disk_info = NULL;

    if (py_retdict == NULL) {
        return NULL;
    }



    while ((entry = readdir(dp))) {
        char path[512];
        char device[512];

        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || 
                    (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
            continue;
        }
        
        snprintf(path, sizeof(path), "%s/%s", dir_path,  entry->d_name);
        

        if (stat(path, &sb) == 0) {

            snprintf(device, sizeof(device), "%d|%d", major(sb.st_rdev), minor(sb.st_rdev));
            py_disk_info = Py_BuildValue(
                    "s",
                    path
                    );

            if (!py_disk_info) {
                continue;
            }

            PyDict_SetItemString(py_retdict, device , py_disk_info);
            Py_CLEAR(py_disk_info);
        }
    }

    closedir(dp);

    return py_retdict;


}

#define DEV_NAME_LEN 64
typedef struct disk {
    int devid;
    char devname[DEV_NAME_LEN];
}Disk;

static PyObject *psutil_disk_io_counters(PyObject *self,  PyObject *args) {
    const char *disk_path = "/dev/disk";
    const char *dsk_path = "/dev/dsk";
    const char *dir_path = NULL;

    if (access(dsk_path, F_OK) == 0) {
        dir_path = dsk_path;
    } else if (access(disk_path, F_OK) == 0) {
        dir_path = disk_path;
    } else {
        PyErr_SetString(PyExc_RuntimeError, "/dev dir access error");
        return NULL;
    }
    
    struct dirent **entry;
    int n;

    Disk * diskMap;
    struct stat sb;

    n = scandir(dsk_path, &entry, NULL, NULL);
    
    if (n < 0) {
        PyErr_SetString(PyExc_RuntimeError, "dev scan error");
        return NULL;
    }

    diskMap = (Disk *) malloc(n * sizeof(Disk));
    if (diskMap == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Disk Map malloc Fail");
    }

    int i;
    int j;
    for (i = 0; i < n; i++) {
        char path[256];
        snprintf(path, sizeof(path), "%s/%s", dir_path,  entry[i]->d_name);
        if (stat(path, &sb) == 0) {
            diskMap[i].devid = sb.st_rdev;
            snprintf(diskMap[i].devname, DEV_NAME_LEN, "%s", entry[i]->d_name);
        } 
        
        free(entry[i]);
    }
    
    free(entry);

    struct pst_diskinfo psd;
    PyObject *py_retdict = PyDict_New();
    if (py_retdict == NULL) {
        return NULL;
    }
    PyObject *py_disk_info = NULL;

    i = 0;
    while (pstat_getdisk(&psd, sizeof(psd), 1, i++) > 0) {

        int devid = makedev(psd.psd_dev.psd_major, psd.psd_dev.psd_minor);
        for(j = 0; j < n; j++) {
            if (diskMap[j].devid != devid) continue;
            py_disk_info = Py_BuildValue(
                "KKKKKKKdd",
                (unsigned long long) psd.psd_dkxfer,
                (unsigned long long) psd.psd_dkwds * 64LL, //word size 
#ifdef IS_HPUX_11_31 
                (unsigned long long) psd.psd_dkread,
                (unsigned long long) psd.psd_dkwrite,
                (unsigned long long) psd.psd_dkbyteread,
                (unsigned long long) psd.psd_dkbytewrite,
#else
                (unsigned long long) 0,
                (unsigned long long) 0,
                (unsigned long long) 0,
                (unsigned long long) 0,
#endif
                (unsigned long long)psd.psd_dktime * 10, //tick *1000 / 100 
                TV2MICRO(psd.psd_dkwait),
                TV2MICRO(psd.psd_dkresp)
                );

            if (!py_disk_info)
                continue;

            PyDict_SetItemString(py_retdict, diskMap[j].devname, py_disk_info);
            Py_CLEAR(py_disk_info);

            break;
        }
    }

    return py_retdict;
}

static PyObject *psutil_disk_io_time(PyObject *self,  PyObject *args) {
    struct pst_diskinfo psd;
    
    PyObject *py_retdict = PyDict_New();
    PyObject *py_disk_info = NULL;

    int i = 0;
    
    while (pstat_getdisk(&psd, sizeof(psd), 1, i++) > 0) {
        char device[512] = {0, };
        snprintf(device, sizeof(device), "%d|%d", psd.psd_dev.psd_major, psd.psd_dev.psd_minor);
        
        py_disk_info = Py_BuildValue(
                "K",
                psd.psd_dktime * 10 //tick
                );

        if (!py_disk_info)
            continue;

        PyDict_SetItemString(py_retdict, device, py_disk_info);
        Py_CLEAR(py_disk_info);
    }


    return py_retdict;
}

static PyObject *psutil_fs_io_counters(PyObject *self,  PyObject *args) {
    FILE *mounts;
    struct mntent *ent;
    mounts = setmntent("/etc/mnttab", "r");
    if (mounts == NULL) {
        return NULL;
    }

    PyObject *py_retdict = PyDict_New();
    PyObject *py_disk_info = NULL;
    struct pst_lvinfo psl; 

    if (py_retdict == NULL) {
        endmntent(mounts);
        return NULL;
    }

    struct stat sb;
    char key[128];

    while ((ent = getmntent(mounts)) != NULL) {
        if (stat(ent->mnt_fsname, &sb) == 0) {
            if (pstat_getlv(&psl, sizeof(psl), 0, (int)sb.st_rdev) == -1) {
                //PyErr_SetString(PyExc_RuntimeError, "pstat_getlv failed");
                continue;
            }
            snprintf(key, sizeof(key), "%s|%s", ent->mnt_dir, ent->mnt_fsname);

            py_disk_info = Py_BuildValue(
                    "KKKK",
                    psl.psl_rxfer,
                    psl.psl_wxfer,
                    psl.psl_rcount,
                    psl.psl_wcount
                    );

            if (!py_disk_info)
                continue;

            PyDict_SetItemString(py_retdict, key, py_disk_info);

            Py_CLEAR(py_disk_info);
        }
    }

    endmntent(mounts);
    return py_retdict;
}


static PyObject *psutil_net_io_counters(PyObject *self, PyObject *args) {
    int fd; 
    int val = 0; 
    int ret; 
    unsigned int ulen; 
    int count = -1; 

    struct nmparms p; 
    

    if ((fd = open_mib("/dev/ip", O_RDONLY, 0, NM_ASYNC_OFF)) < 0) { 
        return NULL; 
    }  
    p.objid = ID_ifNumber;   
    p.buffer = (void *)&val; 
    ulen = sizeof(int); 
    p.len = &ulen; 
    if((ret = get_mib_info(fd, &p)) == 0) 
        count = val; 
    close_mib(fd); 

    nmapi_phystat *ifptr;
    ulen = (unsigned int) count * sizeof(nmapi_phystat);
    ifptr = (nmapi_phystat *)malloc(ulen);

    if ((ret = get_physical_stat(ifptr, &ulen)) < 0) {
        free(ifptr);
        return NULL;
    }

    int i = 0;
    
    PyObject *py_retdict = PyDict_New();
    PyObject *py_net_info = NULL;
    mib_ifEntry *mib;

    for (; i < count; i++) {
        if(ifptr[i].nm_device[0] == '\0') {
            continue;
        }
        mib = &ifptr[i].if_entry;
        py_net_info = Py_BuildValue(
                "(KKKKiiii)",
                mib->ifOutOctets,
                mib->ifInOctets,
                mib->ifOutUcastPkts + mib->ifOutNUcastPkts,
                mib->ifInUcastPkts + mib->ifInNUcastPkts,
                mib->ifInErrors,
                mib->ifOutErrors,
                mib->ifInDiscards,
                mib->ifOutDiscards
                );
        if (!py_net_info)
            continue;

        PyDict_SetItemString(py_retdict, ifptr[i].nm_device, py_net_info);
        Py_DECREF(py_net_info);
    }

    free(ifptr);
    return py_retdict;
}

/*
#define PROC_LEN 16
static PyObject *psutil_pids(PyObject *self, PyObject *args) {

    struct pst_status pss[100];

    PyObject *py_retdict = PyDict_New();
    PyObject *py_pid_info = NULL;
    char pid[16];

    int i = 1;

    int ret = pstat_getproc(pss, sizeof(pss[0]), 0, 1);
    while (pstat_getproc(pss, sizeof(pss[0]), 0, i++) > 0) {
        py_pid_info = Py_BuildValue(
                "i",
                pss[0].pst_pid
                );
        if (!py_pid_info)
            continue;

        snprintf(pid, sizeof(pid), "%d", pss[0].pst_pid);
        PyDict_SetItemString(py_retdict, pid, py_pid_info);
        Py_CLEAR(py_pid_info);
    }

    return py_retdict;
}

   int getCpuCoreNum() {
   struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        perror("pstat_getdynamic failed");
        return -1;
    }
    return psd.psd_proc_cnt;
}
*/

static PyObject *psutil_proc_open_file(PyObject *self, PyObject *args) {
    pid_t pid;

    if (! PyArg_ParseTuple(args, _Py_PARSE_PID, &pid)){
        PyErr_SetString(PyExc_RuntimeError, "pid error");
        return NULL;
    }

    struct pst_fileinfo2 psf;
    int i = 0;
    int fd_count;


    PyObject *py_retdict = PyDict_New();
    PyObject *py_file_info = NULL;

    int count = 0;

    while ((fd_count = pstat_getfile2(&psf, sizeof(psf), 1, i, pid)) > 0) {
        i++;
        char filename[64];
        memset(filename, 0, sizeof(filename));
        //PyObject * pFileName= NULL;

        count = pstat_getpathname(filename, 64, &(psf.psf_fid));
        if (count <= 0) {
            continue;
        }
        
        //pFileName = PyUnicode_DecodeFSDefault(filename);
        py_file_info = Py_BuildValue(
                "i",
                psf.psf_fd
                );

        if (!py_file_info)
            continue;

        PyDict_SetItemString(py_retdict, filename, py_file_info);
        Py_DECREF(py_file_info);
    }

    return py_retdict;
}

static PyObject* psutil_proc_total_info (PyObject* self, PyObject* args) {
    int total_threads = 0;
    int defunct_processes = 0;
    int idx = 0;

    struct pst_static pst_static;
    struct pst_status *pst;
    int max_proc; //default

    if (pstat_getstatic(&pst_static, sizeof(pst_static), 1, 0) == -1) {
        return NULL;
    }
    max_proc = pst_static.max_proc;


    pst = malloc(sizeof(struct pst_status) * max_proc);
    if (pst == NULL) {
        return NULL;
    }

    int ret;
    ret = pstat_getproc(pst, sizeof(struct pst_status), max_proc, 0);
    if (ret == -1) {
        free(pst);
        return NULL;
    }

    for (idx = 0; idx < ret; idx++) {
        if (pst[idx].pst_stat == PS_ZOMBIE) {
            defunct_processes++;
        }
        total_threads += pst[idx].pst_nlwps; 
    }

    free(pst);


    return Py_BuildValue("(iii)", ret, total_threads, defunct_processes);
}

static PyObject* psutil_proc_detail_info (PyObject* self, PyObject* args) {
    int idx = 0;

    struct pst_static pst_static;
    struct pst_status *pst;
    int max_proc; //default

    if (pstat_getstatic(&pst_static, sizeof(pst_static), 1, 0) == -1) {
        PyErr_SetString(PyExc_RuntimeError, "pstat_getstatic failed");
        return NULL;
    }
    max_proc = pst_static.max_proc;

    pst = malloc(sizeof(struct pst_status) * max_proc);
    if (pst == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "malloc failed");
        return NULL;
    }

    int ret;
    ret = pstat_getproc(pst, sizeof(struct pst_status), max_proc, 0);
    if (ret == -1) {
        free(pst);
        PyErr_SetString(PyExc_RuntimeError, "pstat_getproc failed");
        return NULL;
    }


    PyObject *py_proc_info = NULL;
    PyObject *py_retdict = PyDict_New();
    if (! py_retdict)
        return PyErr_NoMemory();
    

    char pidStr[32];
    PyObject * comm = NULL;
    PyObject * cmdline = NULL;
    PyObject * username = NULL;
    char name[32] = {0, };

    for (idx = 0; idx < ret; idx++) {
        snprintf(pidStr, sizeof(pidStr), "%d", pst[idx].pst_pid);
        comm= PyUnicode_DecodeFSDefault(pst[idx].pst_ucomm);
        cmdline = PyUnicode_DecodeFSDefault(pst[idx].pst_cmd);
        
        struct passwd *pw = getpwuid(pst[idx].pst_uid);
        if (pw != NULL) {
            snprintf(name, sizeof(name), pw->pw_name);
        } else {
            snprintf(name, sizeof(name), "unknown");
        }
        username = PyUnicode_DecodeFSDefault(name);

        py_proc_info = Py_BuildValue(
                "(iiOOOKKiiiiifKKKKKKKK)",
                pst[idx].pst_pid,
                pst[idx].pst_ppid,
                comm,
                cmdline,
                username,
                pst[idx].pst_rssize,
                pst[idx].pst_vssize + pst[idx].pst_vtsize + pst[idx].pst_vdsize + pst[idx].pst_vshmsize + pst[idx].pst_vusize + pst[idx].pst_vmmsize,
                pst[idx].pst_start,
                pst[idx].pst_stat,
                pst[idx].pst_nlwps,
                pst[idx].pst_utime,
                pst[idx].pst_stime,
                pst[idx].pst_pctcpu * 100, // / psd.psd_proc_cnt, // cpu 
                pst[idx].pst_ioch,
                pst[idx].pst_vssize,
                pst[idx].pst_vtsize,
                pst[idx].pst_vdsize,
                pst[idx].pst_vshmsize,
                pst[idx].pst_vusize,
                pst[idx].pst_vmmsize,
                pst[idx].pst_cptickstotal
                );
        
        //pss.pst_pid
        //pss.pst_ppid
        //comm
        //cmdline
        //username
        //rss
        //vsz
        //start
        //stat
        //pst_nlwps
        //utime
        //stime
        //pctcpu
        //pi_ioch
        

        PyDict_SetItemString(py_retdict, pidStr, py_proc_info);
        Py_DECREF(comm);
        Py_DECREF(cmdline);
        Py_DECREF(username);
        Py_CLEAR(py_proc_info);
    }

    free(pst);

    return py_retdict;
}

static PyObject *psutil_proc_oneshot_info(PyObject *self, PyObject *args) {
    pid_t pid;

    if (! PyArg_ParseTuple(args, _Py_PARSE_PID, &pid)){
        PyErr_SetString(PyExc_RuntimeError, "pid error");
        return NULL;
    }

    struct pst_status pst;

    PyObject * cmdline = NULL;
    PyObject * username = NULL;
    char name[32] = {0, };

    if (pstat_getproc(&pst, sizeof(pst), 0, pid) > 0) {
        cmdline = PyUnicode_DecodeFSDefault(pst.pst_cmd);
        struct passwd *pw = getpwuid(pst.pst_uid);
        if (pw != NULL) {
            snprintf(name, sizeof(name), pw->pw_name);
        } else {
            snprintf(name, sizeof(name), "unknown");
        }
        
        username = PyUnicode_DecodeFSDefault(name);
        PyObject *value = Py_BuildValue(
                "iiiifKKSSiiiiK",
                pst.pst_pid,
                pst.pst_ppid,
                pst.pst_utime,
                pst.pst_stime,
                pst.pst_pctcpu * 100, // / psd.psd_proc_cnt, // cpu 
                pst.pst_rssize,  // * pst.page_size,
                pst.pst_ioch,
                cmdline,
                username,
                pst.pst_stat,
                pst.pst_start,
                pst.pst_inblock,
                pst.pst_oublock,
                pst.pst_cptickstotal
                );

        Py_DECREF(cmdline);
        Py_DECREF(username);
        return value;
    }
    PyErr_SetString(PyExc_RuntimeError, "pstat_getproc failed");
    return NULL;
}

static PyObject *psutil_proc_create_time(PyObject *self, PyObject *args) {
    pid_t pid;

    if (! PyArg_ParseTuple(args, _Py_PARSE_PID, &pid)){
        PyErr_SetString(PyExc_RuntimeError, "pid error");
        return NULL;
    }

    struct pst_status pst;

    if (pstat_getproc(&pst, sizeof(pst), 0, pid) > 0) {
        PyObject *value = Py_BuildValue(
                "i",
                pst.pst_start
                );
        return value;
    }
    PyErr_SetString(PyExc_RuntimeError, "pstat_getproc failed");
    return NULL;
}

//
static PyObject *psutil_proc_disk_io(PyObject *self, PyObject *args) {
    pid_t pid;

    if (! PyArg_ParseTuple(args, _Py_PARSE_PID, &pid)){
        PyErr_SetString(PyExc_RuntimeError, "pid error");
        return NULL;
    }

    struct pst_status pst;

    if (pstat_getproc(&pst, sizeof(pst), 0, pid) > 0) {
        PyObject *value = Py_BuildValue(
                "Kii",
                pst.pst_ioch,
                pst.pst_inblock,
                pst.pst_oublock
                );
        return value;
    }
    PyErr_SetString(PyExc_RuntimeError, "pstat_getproc failed");
    return NULL;
}

static PyMethodDef
PsutilMethods[] = {
    {"proc_cpu_num", psutil_proc_cpu_num, METH_VARARGS},
    {"per_cpu_times", psutil_per_cpu_times, METH_VARARGS},
    {"cpu_stats", psutil_cpu_stats, METH_VARARGS},
    {"total_memory", psutil_total_memory, METH_VARARGS},
    {"virtual_memory", psutil_virtual_memory, METH_VARARGS},
    {"swap_memory", psutil_swap_memory, METH_VARARGS},
    {"cpu_load", psutil_cpu_load, METH_VARARGS},
    {"boot_time", psutil_boot_time, METH_VARARGS},
    {"fs_io_counters", psutil_fs_io_counters, METH_VARARGS},
    {"disk_io_counters", psutil_disk_io_counters, METH_VARARGS},
    {"disk_io_info", psutil_disk_info, METH_VARARGS},
    {"disk_io_time", psutil_disk_io_time, METH_VARARGS},
    {"net_io_counters", psutil_net_io_counters, METH_VARARGS},
    {"check_pid_range", psutil_check_pid_range, METH_VARARGS}, 
    {"proc_oneshot_info", psutil_proc_oneshot_info, METH_VARARGS},
    {"proc_disk_io", psutil_proc_disk_io, METH_VARARGS},
    {"proc_create_time", psutil_proc_create_time, METH_VARARGS},
    {"proc_open_file", psutil_proc_open_file, METH_VARARGS},
    {"cpu_stats_detail", psutil_cpu_stats_detail, METH_VARARGS},
    {"virtual_memory_detail", psutil_virtual_memory_detail, METH_VARARGS},
    {"proc_total_info", psutil_proc_total_info, METH_VARARGS},
    {"proc_detail_info", psutil_proc_detail_info, METH_VARARGS},
    {NULL, NULL, 0, NULL}
};

void init_psutil_hpux(void) {
    PyObject *module = Py_InitModule("_psutil_hpux", PsutilMethods);
    PyModule_AddIntConstant(module, "version", PSUTIL_VERSION);

    PyModule_AddIntConstant(module, "SSLEEP", PS_SLEEP);
    PyModule_AddIntConstant(module, "SRUN", PS_RUN);
    PyModule_AddIntConstant(module, "SSTOP", PS_STOP);
    PyModule_AddIntConstant(module, "SZOMB", PS_ZOMBIE);
    PyModule_AddIntConstant(module, "SIDL", PS_IDLE);
    PyModule_AddIntConstant(module, "SOTHER", PS_OTHER);
}


