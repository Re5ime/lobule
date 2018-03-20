from dolfin import *
import matplotlib.pyplot as plt
import numpy as np

# Generate mesh and plot
mesh0 = 'lobuleMesh/lobule3.xml'
mesh = Mesh(mesh0)
plot(mesh)
info(mesh)
interactive()
