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
#include "_psutil_common.h"

static PyObject *psutil_proc_cpu_num(PyObject *self, PyObject *args) {
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        perror("pstat_getdynamic failed");
        return NULL;
    }
    //return psd.psd_proc_cnt;
    return Py_BuildValue("i", psd.psd_proc_cnt);
}

static PyObject *psutil_per_cpu_times(PyObject *self, PyObject *args) {
    //매번 호출 말고 매개변수로 변경 검토
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        perror("pstat_getdynamic failed");
        return NULL;
    }
 
    size_t ncpu = psd.psd_proc_cnt;
    struct pst_processor *psp;

    psp = (struct pst_processor *)malloc(ncpu * sizeof(struct pst_processor));
    if (pstat_getprocessor(psp, sizeof(struct pst_processor), ncpu, 0) == -1) {
        perror("pstat_getprocessor failed");
        return NULL;
    }

    PyObject *py_retlist = PyList_New(0);
    PyObject *py_cputime = NULL;

    int i = 0;
    //{"USER","NICE","SYS","IDLE","WAIT","BLOCK","SWAIT","INTR","SSYS"};
    for (i = 0; i < ncpu; i++) {
        py_cputime = Py_BuildValue(
                "(KKKKK)",
                psp[i].psp_cpu_time[CP_USER],
                psp[i].psp_cpu_time[CP_SYS],
                psp[i].psp_cpu_time[CP_IDLE],
                psp[i].psp_cpu_time[CP_SWAIT] + psp[i].psp_cpu_time[CP_BLOCK],
                psp[i].psp_cpu_time[CP_NICE]);
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

// load
static PyObject *psutil_cpu_load(PyObject *self, PyObject *args) {
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        perror("pstat_getdynamic failed");
        return NULL;
    }
    return Py_BuildValue(
            "ddd",
            psd.psd_avg_1_min,
            psd.psd_avg_5_min,
            psd.psd_avg_15_min
            );


}

// interrupts
// syscall
// ctxswitch
static PyObject *psutil_cpu_stats(PyObject *self, PyObject *args) {
    struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        perror("pstat_getdynamic failed");
        return NULL;
    }
 
    size_t ncpu = psd.psd_proc_cnt;
    struct pst_processor *psp;

    psp = (struct pst_processor *)malloc(ncpu * sizeof(struct pst_processor));
    if (pstat_getprocessor(psp, sizeof(struct pst_processor), ncpu, 0) == -1) {
        perror("pstat_getprocessor failed");
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


static PyObject *psutil_total_memory(PyObject *self, PyObject *args) {
    struct pst_static pst;

    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1 ) {
        perror("pstat_getstatic failed");
        return NULL;
    }
    return Py_BuildValue("K", (unsigned long long)pst.physical_memory * (unsigned long long)pst.page_size);
}

static PyObject *psutil_virtual_memory(PyObject *self, PyObject *args) {
    struct pst_static pst;
    struct pst_dynamic psd;
    struct pst_vminfo psv;


    if (pstat_getstatic(&pst, sizeof(pst), (size_t)1, 0) == -1 ) {
        perror("pstat_getstatic failed");
        return NULL;
    }

    int page_size = pst.page_size;

    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1 ) {
        perror("pstat_getdynamic failed");
        return NULL;
    }

    if (pstat_getvminfo(&psv, sizeof(psv), (size_t)1, 0) == -1 ) {
        perror("pstat_getvminfo failed");
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
        perror("pstat_getstatic failed");
        return NULL;
    }

    return Py_BuildValue(
            "I",
            pst.boot_time);
}


static PyObject *psutil_disk_info(PyObject *self,  PyObject *args) {
    
    PyObject *py_retdict = PyDict_New();
    PyObject *py_disk_info = NULL;

    const char *dir_path = "/dev/dsk";
    struct dirent *entry;
    DIR *dp;

    struct stat sb;

    dp = opendir(dir_path);
    if (dp == NULL) {
        Py_XDECREF(py_disk_info);
        Py_DECREF(py_retdict);
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
                psd.psd_dktime * 10
                );

        if (!py_disk_info)
            continue;

        PyDict_SetItemString(py_retdict, device, py_disk_info);
        Py_CLEAR(py_disk_info);
    }


    return py_retdict;
}



static PyObject *psutil_disk_io_counters(PyObject *self,  PyObject *args) {
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
                perror("pstat_getlv failed");
                goto error;
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
    /*
    if (pstat_getlv(&psl, sizeof(psl), 0, 1) == -1) {
        perror("pstat_getlv failed");
        goto error;
    }

    py_disk_info = Py_BuildValue(
            "KKKKKKK",
            psl.psl_rxfer,
            psl.psl_wxfer,
            psl.psl_rcount,
            psl.psl_wcount,
            0

            );

    if (!py_disk_info)
        goto error;

    if (PyDict_SetItemString(py_retdict, "TEST",
                py_disk_info))
        goto error;
        */

    endmntent(mounts);
    return py_retdict;

error:
    Py_XDECREF(py_disk_info);
    Py_DECREF(py_retdict);
    endmntent(mounts);
    return NULL;

}


static PyObject *psutil_net_io_counters(PyObject *self, PyObject *args) {
    int rtv; 
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
   int getCpuCoreNum() {
   struct pst_dynamic psd;
    if (pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) == -1) {
        perror("pstat_getdynamic failed");
        return -1;
    }
    return psd.psd_proc_cnt;
}
*/
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
    {"disk_io_counters", psutil_disk_io_counters, METH_VARARGS},
    {"disk_io_info", psutil_disk_info, METH_VARARGS},
    {"disk_io_time", psutil_disk_io_time, METH_VARARGS},
    {"net_io_counters", psutil_net_io_counters, METH_VARARGS},
    {NULL, NULL, 0, NULL}
};

void init_psutil_hpux(void) {
    PyObject *module = Py_InitModule("_psutil_hpux", PsutilMethods);
    PyModule_AddIntConstant(module, "version", PSUTIL_VERSION);
}


