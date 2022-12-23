import os 
from os.path import exists
import shutil
import subprocess
import signal, psutil

class Commands():
    
    @staticmethod
    def createFolder(folder):
        try:
            os.makedirs(folder)
        except FileExistsError:
            pass

    @staticmethod
    def deleteFolder(folder):
        try:
            shutil.rmtree(folder)
        except OSError as e:
            print("Error: %s - %s." % (e.filename, e.strerror))

    @staticmethod
    def deleteFile(path):
        try:
            os.remove(path)
        except FileNotFoundError:
            pass
        
    @staticmethod
    def createFolders(folders):
        for folder in folders:
            Commands.createFolder(folder)

    @staticmethod
    def execute(command):
        os.system(command)

    @staticmethod
    def fileExists(path):
        return exists(path)

    @staticmethod
    def executeWithTimeout(command, timeout):
        process = subprocess.Popen([command], shell=True)

        try:
            process.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            pid = process.pid
            print(f"ERROR: Process {pid} timed out")
            Commands.killChildProcesses(pid)
            process.kill()
            return True

        return False

    @staticmethod
    def killChildProcesses(parent_pid):
        try:
            parent = psutil.Process(parent_pid)
        except psutil.NoSuchProcess:
            return
        children = parent.children(recursive=True)
        for process in children:
            process.send_signal(signal.SIGKILL)

    @staticmethod
    def listFolder(folder):
        return os.listdir(folder)