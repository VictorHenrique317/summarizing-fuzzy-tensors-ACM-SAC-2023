from abc import ABC, abstractmethod
from multiprocessing import Process
from base.file_system import FileSystem

from utils.commands import Commands

class Algorithm(ABC):
    def __init__(self):
        self.time_out_info = dict()
        self.experiment_path = None
        self.log_path = None

    @abstractmethod
    def run(self, u, timeout):
        pass

    def timedOut(self, u):
        self.time_out_info[u] = True

    def hasTimedOut(self, u):
        return self.time_out_info.get(u, False)

    def resetTimeOutInfo(self):
        self.time_out_info = dict()