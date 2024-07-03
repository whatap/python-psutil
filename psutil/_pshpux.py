
import os
import sys
import re

from collections import namedtuple

from . import _common
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


def net_io_counters(pernic=False):
    ret = cext.net_io_counters()
    return ret


#
#
#

class Process:
    __slots__ = ["pid", "_name", "_ppid", "_procfs_path", "_cache"]

    def __init__(self, pid):
        self.pid = pid
        self._name = None
        self._ppid = None
        self._procfs_path = get_procfs_path()


