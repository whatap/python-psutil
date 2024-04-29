#include <stdio.h>
#include <stdlib.h>
#include <sys/pstat.h>
#include <sys/utsname.h>
#include <Python.h>

int system_uname() {
    struct utsname name;

    if (-1 == uname(&name)) {
        return -1;
    }

    printf("%s %s %s\n", name.sysname, name.nodename, name.release);
    return 0;
}

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
                "(iiii)",
                psp[i].psp_cpu_time[0],
                psp[i].psp_cpu_time[2],
                psp[i].psp_cpu_time[3],
                psp[i].psp_cpu_time[5] + psp[i].psp_cpu_time[6]);
        if (!py_cputime)
            return NULL;
        if (PyList_Append(py_retlist, py_cputime))
            return NULL;
        Py_DECREF(py_cputime);
    }

    free(psp);
    return py_retlist; 

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
    {NULL, NULL, 0, NULL}
};

void init_psutil_hpux(void) {
    PyObject *module = Py_InitModule("_psutil_hpux", PsutilMethods);
    PyModule_AddIntConstant(module, "version", PSUTIL_VERSION);
    return module;
}


