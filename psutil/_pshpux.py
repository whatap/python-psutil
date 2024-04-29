
import os
import sys

from collections import namedtuple

from . import _common
from . import _psutil_hpux as cext


__extra__all__ = []



#


#



AF_LINK = None



scputimes = namedtuple('scputimes', ['user', 'system', 'idle', 'iowait'])
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


class Process:
    __slots__ = ["pid", "_name", "_ppid", "_procfs_path", "_cache"]

    def __init__(self, pid):
        self.pid = pid
        self._name = None
        self._ppid = None
        self._procfs_path = get_procfs_path()


