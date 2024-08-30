import subprocess
import functools
import os
import sys
import re

from collections import namedtuple

from . import _common
from ._common import get_procfs_path
from ._common import memoize
from ._common import memoize_when_activated
from ._common import NoSuchProcess
from ._common import ZombieProcess
from ._compat import FileNotFoundError
from ._compat import PermissionError
from ._compat import ProcessLookupError
from ._compat import which
from . import _psutil_hpux as cext


__extra__all__ = []

sdiskio = namedtuple(
        'sdiskio', ['read_count', 'write_count',
            'read_bytes', 'write_bytes',
            'read_time', 'write_time',
            'busy_time'])


#


#

def boot_time():
    return cext.boot_time()



AF_LINK = None

scputimes = namedtuple('scputimes', ['user', 'system', 'idle', 'iowait', 'nice'])


cpuloads = namedtuple('cpuloads', ['load1', 'load5', 'load15'])





#
# CPU
#

def cpu_times():
    ret = cext.per_cpu_times()
    return scputimes(*[sum(x) for x in zip(*ret)])


def per_cpu_times():
    """Return system per-CPU times as a list of named tuples."""
    ret = cext.per_cpu_times()
    return [scputimes(*x) for x in ret]

def cpu_count_logical():
    return cext.proc_cpu_num()


def cpu_stats():
    ctx_switches, interrupts, syscalls, traps = cext.cpu_stats()
    soft_interrupts = 0

    ret = cext.per_cpu_times()
    return _common.scpustats(*[sum(x) for x in zip(*ret)])
#    return _common.scpustats(ctx_switches, interrupts, soft_interrupts, syscalls)


def getloadavg():
    load1, load5, load15 = cext.cpu_load()
    return cpuloads(load1, load5, load15)


#
# MEMORY
#


# psutil.virtual_memory()
svmem = namedtuple('svmem', ['total', 'available', 'used', 'free', 'cached', 'pagesize'])

# psutil.Process.memory_info()
total_memory = cext.total_memory()

def memory_test():
    print cext.total_memory()
    print total_memory



def virtual_memory():
    total, available, used, free, cached, page = cext.virtual_memory()

    return svmem(total, available, used, free, cached, page)



def swap_memory():
    swap_total, swap_free, swap_used, sin, sout = cext.swap_memory()
    percent = float(swap_used) / float(swap_total) * 100
    return _common.sswap(swap_total, swap_used, swap_free, percent, sin, sout)

#
# DISK
#
#disk_io_counters = cext.disk_io_counters


def getLvmMap():
    lvmtabFile = open('/etc/lvmtab', 'rb')
    if not lvmtabFile:
        return None

    lvm = lvmtabFile.read()
    lvm = re.findall("[^\x00-\x1F\x7F-\xFF]{4,}", lvm)

    vg_map = {}
    current_vg = None

    vg_pattern = re.compile(r'/dev/vg\w+')
    pv_pattern = re.compile(r'/dev/dsk/\w+')

    for line in lvm:
        vg_match = vg_pattern.match(line.strip())
        pv_match = pv_pattern.match(line.strip())

        if vg_match:
            current_vg = vg_match.group(0)
            vg_map[current_vg] = []
        elif pv_match and current_vg:
            pv = pv_match.group(0)
            vg_map[current_vg].append(pv)

    lvmtabFile.close()

    return vg_map




def disk_io_counters(perdisk=False):
    ioCounters = cext.disk_io_counters()
    infos = cext.disk_io_info()
    times = cext.disk_io_time()
    vg_map = getLvmMap()


    ioTimeMap = {}
    for k, v in times.items():
        ioTimeMap[infos[k]] = v

    ret = {}

    vg_pattern = re.compile(r'/dev/vg\w+')
    pv_pattern = re.compile(r'(/dev/dsk/c\d+t\d+d\d+)')
    
    for k, v in ioCounters.items():
        keyArr = k.split('|')
        vg_match = vg_pattern.match(keyArr[1])
        pvs = []
        if vg_match:
            current_vg = vg_match.group(0)
            if current_vg in vg_map:
                pvs = vg_map[current_vg]

        times = {}
        for pv in pvs:
            pv_match = pv_pattern.match(pv).group(1)
            times[pv_match] = ioTimeMap[pv_match]

        ret[keyArr[0]] = (v[0], v[1], v[2], v[3], 0, 0, times)
    return ret


def net_io_counters(pernic=False):
    ret = cext.net_io_counters()
    return ret


#
#
#


def wrap_exceptions(fun):
    """Call callable into a try/except clause and translate ENOENT,
    EACCES and EPERM in NoSuchProcess or AccessDenied exceptions.
    """
    @functools.wraps(fun)
    def wrapper(self, *args, **kwargs):
        try:
            return fun(self, *args, **kwargs)
        except (FileNotFoundError, ProcessLookupError):
            # ENOENT (no such file or directory) gets raised on open().
            # ESRCH (no such process) can get raised on read() if
            # process is gone in meantime.
            if not pid_exists(self.pid):
                raise NoSuchProcess(self.pid, self._name)
            else:
                raise ZombieProcess(self.pid, self._name, self._ppid)
        except PermissionError:
            raise AccessDenied(self.pid, self._name)
        except RuntimeError:
            raise NoSuchProcess(self.pid, self._name)

    return wrapper



def pids():
    """Returns a list of PIDs currently running on the system."""
    process = subprocess.Popen(['ps', '-e'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    processes = stdout.decode('utf-8').split('\n')

    pid_list = []

    for process in processes[1:]:
        if process.strip():
            pid = int(process.split()[0])
            if pid == 0: continue
            pid_list.append(pid)

    return pid_list



kinfo_proc_map = dict(
        pid=0,
        ppid=1,
        utime=2,
        stime=3,
        pcpu=4,
        rss=5,
        totalIO=6,
        cmdline=7,
        username=8,
        status=9,
        starttime=10,
        inblock=11,
        outblock=12,
)

pcputimes = namedtuple('pcputimes', ['user', 'system', 'pcpu'])
pmem = namedtuple('pmem', ['rss'])

pio = namedtuple('pio', ['read_count', 'write_count',
                         'read_bytes', 'write_bytes',
                         'total_bytes'])

PROC_STATUSES = {
    cext.SSLEEP: _common.STATUS_SLEEPING,
    cext.SRUN: _common.STATUS_RUNNING,
    cext.SSTOP: _common.STATUS_STOPPED,
    cext.SZOMB: _common.STATUS_ZOMBIE,
    cext.SIDL: _common.STATUS_IDLE,
    cext.SOTHER: _common.STATUS_RUNNING
}


class Process:
    """Wrapper class around underlying C implementation."""

    __slots__ = ["pid", "_name", "_ppid", "_cache"]

    def __init__(self, pid):
        self.pid = pid
        self._name = None
        self._ppid = None

    @wrap_exceptions
    def create_time(self):
        return self.oneshot_info()[kinfo_proc_map['starttime']]

    @wrap_exceptions
    @memoize_when_activated
    def oneshot_info(self):
        ret = cext.proc_oneshot_info(self.pid)
        return ret

    def oneshot_enter(self):
        self.oneshot_info.cache_activate(self)

    def oneshot_exit(self):
        self.oneshot_info.cache_deactivate(self)

    @wrap_exceptions
    def ppid(self):
        self._ppid = self.oneshot_info()[kinfo_proc_map['ppid']]
        return self._ppid

    @wrap_exceptions
    def name(self):
        cmd = self.cmdline()
        if len(cmd) == 0:
            return ""
        exe = cmd[0]
        return exe.split('/')[-1]


    @wrap_exceptions
    def exe(self):
        return self.name()

    @wrap_exceptions
    def cmdline(self):
        cmd = self.oneshot_info()[kinfo_proc_map['cmdline']]
        if cmd == "":
            return []
        return cmd.split()

    @wrap_exceptions
    def status(self):
        code = self.oneshot_info()[kinfo_proc_map['status']]
        return PROC_STATUSES.get(code, '?')

    @wrap_exceptions
    def username(self):

        return self.oneshot_info()[kinfo_proc_map['username']]

    @wrap_exceptions
    def cwd(self):
        return "cwd"
    
    @wrap_exceptions
    def nice_get(self):
        return "nice_get"
    
    @wrap_exceptions
    def nice_set(self):
        return "nice_set"

    @wrap_exceptions
    def uids(self):
        return "uids"

    @wrap_exceptions
    def gids(self):
        return "gids"

    @wrap_exceptions
    def memory_info(self):
        meminfo = (self.oneshot_info()[kinfo_proc_map['rss']])
        return pmem(meminfo)

    @wrap_exceptions
    def cpu_times(self):
        v = self.oneshot_info()
        times = (v[kinfo_proc_map['utime']], v[kinfo_proc_map['stime']], v[kinfo_proc_map['pcpu']])
        print times
        return pcputimes(*times)


    @wrap_exceptions
    def io_counters(self):
        v = self.oneshot_info()
        return pio(0, 0, v[kinfo_proc_map['inblock']], v[kinfo_proc_map['outblock']], v[kinfo_proc_map['totalIO']])


    @wrap_exceptions
    def open_files(self):
        rets = cext.proc_open_file(self.pid)
        retlist = []
        for path, fd in rets.items():
            retlist.append(_common.popenfile(path, int(fd)))
        return retlist 
        
