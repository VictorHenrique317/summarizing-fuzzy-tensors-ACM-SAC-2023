from base.controller import Controller
from algorithm.getf import Getf
from algorithm.cancer import Cancer
from algorithm.tribiclusterbox import TriBiclusterBox
from algorithm.nclusterbox import NclusterBox
from algorithm.nclusterbox_no_performance_imp import NclusterBoxNoPerformanceImp

controller = Controller()

# ========================== ALGORITHMS USED ========================== #
nclusterbox = NclusterBox(controller)
getf = Getf(controller)
nclusterboxnoperformanceimp = NclusterBoxNoPerformanceImp(controller)
triclusterbox = TriBiclusterBox(controller)
cancer = Cancer(controller)
# ========================== ALGORITHMS USED ========================== #

controller.initiateSession()
controller.initiatePostAnalysis()