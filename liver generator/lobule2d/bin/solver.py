from dolfin import *
import time
import getopt, sys
import math
import numpy as np
p = 1
alpha = 0.5
t = 0
ms = "6"
options, args = getopt.getopt(sys.argv[1:], "", ["p=", "a=", "m=", "t="])

for option, value in options:
    if   option == '--p':
        p = int(value)
    elif option == '--a':
        alpha = float(value)
    elif option == '--t':
        t = int(value)
    elif option == '--m':
        ms = str(value)

print p, alpha, ms

# Test for PETSc and SLEPc
if not has_linear_algebra_backend("PETSc"):
    print "DOLFIN has not been configured with PETSc. Exiting."
    exit()

if not has_slepc():
    print "DOLFIN has not been configured with SLEPc. Exiting."
    exit()

class Coefficient(Expression):
    def __init__(self, name, domains):
        Expression.__init__(self)
        self.name = name
        self.domains = domains
        file = File("coefficients.xml")
        coeffs = Parameters("coefficients")
        file >> coeffs
        self.values = {}
        for key, val in coeffs[name].iterdata():
            self.values[int(key)] = val[0]
        
    def eval_cell(self, values, x, cell):
        if self.domains[cell.index] in self.values:
            values[0] = self.values[self.domains[cell.index]]
        else:
            warning("In coefficient " + self.name + " not set subdomain " + str(self.domains[cell.index]))
            values[0] = 0.0
            
    def to_func(self, V):
        f = Function(V)
        f.assign(self)
        return f      

mesh = Mesh("domain" + ms + ".xml")
subdomains = MeshFunction("size_t", mesh, "domain" + ms + "_physical_region.xml")
dx = Measure("dx")[subdomains]

V0 = FunctionSpace(mesh, "DG", 0)
V = FunctionSpace(mesh, "Lagrange", p)
W = V * V

# Define trial and test functions
(u1, u2) = TrialFunctions(W)
(v1, v2) = TestFunctions(W)

# Constants
D1 = Coefficient("diffusion1", subdomains).to_func(V0)
D2 = Coefficient("diffusion2", subdomains).to_func(V0)
E1 = Coefficient("absortion1", subdomains).to_func(V0)
E2 = Coefficient("absortion2", subdomains).to_func(V0)
Er = Coefficient("absortionR", subdomains).to_func(V0)
Ef1 = Coefficient("fusion1", subdomains).to_func(V0)
Ef2 = Coefficient("fusion2", subdomains).to_func(V0)
alpha1 = Constant(alpha)
alpha2 = Constant(alpha)

# Define variational form 
a11 = (inner(D1*grad(u1), grad(v1)) + (E1 + Er)*u1*v1)*dx() + alpha1*u1*v1*ds()
a21 = Er*u1*v2*dx()
a22 = (inner(D2*grad(u2), grad(v2)) + E2*u2*v2)*dx() + alpha2*u2*v2*ds()
a = a11 - a21 + a22
b11 = Ef1*u1*v1*dx()
b12 = Ef2*u2*v1*dx()
b = b11 + b12 

# Assemble stiffness form
print "Assembling matrices. This can take a minute"
start = time.time()

M = PETScMatrix()
F = PETScMatrix()
assemble(a, tensor=M,  cell_domains=subdomains)
assemble(b, tensor=F,  cell_domains=subdomains)
midle = time.time()

# Create eigensolver
eigensolver = SLEPcEigenSolver(F, M)

print "Computing eigenvalues. This can take a minute."
eigensolver.solve(0)
end = time.time()
#niter = eigensolver.get_number_converged()
#for i in range(niter):
r, c, rx, cx = eigensolver.get_eigenpair(0)
it = eigensolver.get_iteration_number()

print "eigenvalue: ", r

# Initialize function and assign eigenvector
u = Function(W)
u.vector()[:] = rx
u1, u2 = u.split()

#t1 = time.time()
if (t != 0):
    f1 = open("tvs" + ms + ".dat", "w")
    f1.write(str(p) + '\n')
    f1.write(str(alpha) + '\n')
    core = (Ef1*u1 + Ef2*u2)*dx()
    core_int = assemble(core)
    P = []
    coef = Coefficient("diffusion1", subdomains)
    ids = coef.values.keys()
    for i in range(len(ids)):
        hexi = (Ef1*u1 + Ef2*u2)*dx(ids[i])
        hexi_int = assemble(hexi)
        v = len(ids) * hexi_int / core_int
        P.append(v)
        f1.write(str(ids[i]) + ';' + str(v) + '\n')
    f1.close()
    p = project((Ef1*u1 + Ef2*u2)/core_int, V)
    File("p" + ms + ".pvd") << p
else:
    f = open("result" + ms + ".dat", "w")
    f.write(str(r) + '\n')
    f.write(str(p) + '\n')
    f.write(str(alpha) + '\n')
    f.close()

    File("u1_" + ms + ".pvd") << u1
    File("u2_" + ms + ".pvd") << u2

