from dolfin import *
import matplotlib.pyplot as plt
import numpy as np

eps = DOLFIN_EPS
dyn = 10.0
#parameters['reorder_dofs_serial'] = False

# Generate mesh and plot
mesh = Mesh("lobule.xml")
facets = MeshFunctionSizet(mesh, "lobule_facet_region.xml")
info(mesh)
#plot(mesh, interactive=True)

def leftBnd(x, on_boundary):
    return on_boundary and 0.0 < x[0] < 0.1 + eps and x[1] > 0.5 - eps

def rightBnd(x, on_boundary):
    return on_boundary and abs(x[0] - 3.0) < eps

nTlist = [200]
for nT in nTlist:
    T = 2.0
    dt = T/nT

    # function space
    V = FunctionSpace(mesh, 'CG', 2)
    Ve = FiniteElement('CG', triangle, 2)
    intT = np.linspace(0.0, T, nT + 1)
    intU1 = np.linspace(0.0, T, nT + 1)
    intU2 = np.linspace(0.0, T, nT + 1)
    intT[0] = 0.0
    intU1[0] = 0.0
    intU2[0] = 0.0
    
    cc = 0.01
    cd = 0.01
    cr = 2.0
    cList = [0.1]
    for cd in cList:
	u1_ = Function(V, name='u')
        u1_ = interpolate(Constant(0.0), V)
        u1file = File("res/u1.pvd")
        u1file << u1_
	u2_ = Function(V, name='u')
        u2_ = interpolate(Constant(0.0), V)
        u2file = File("res/u2.pvd")
        u2file << u2_
        
        t = 0.0
        for i in range(1, nT + 1):
            t = i*dt
            
            We = Ve * Ve
	    W = FunctionSpace(mesh, We)
            
            (u1, u2) = TrialFunctions(W)
            (v1, v2) = TestFunctions(W)
            
            g = 1.0 - np.exp(-dyn*t)
            
            bc1 = DirichletBC(W, Constant((g,g)), facets, 16)
            bc2 = DirichletBC(W, Constant((0.0,0.0)), facets, 18)
            bcs = [bc1, bc2]
            
# variational form
            a1 = cc*u1*v1*dx + dt*dot(grad(u1), grad(v1))*dx + dt*cr*(u1-u2)*v1*dx
            a2 = u2*v2*dx + dt*cd*dot(grad(u2), grad(v2))*dx + dt*cr*(u2-u1)*v2*dx
            a = a1 + a2
            L1 = cc*u1_*v1*dx
            L2 = u2_*v2*dx
            L = L1 + L2
            
                    # solution
            z = Function(W)
            solve(a == L, z, bcs)
            (u1, u2) = z.split()
            u1_.assign(u1)
            #assign(u1_,u1)
	    u1file << u1
            u2_.assign(u2)
	    #assign(u2_,u2)
            u2file << u2

            intT[i] = t
            xU = (0.5, 0.5)
            intU1[i] = u1(xU)
            intU2[i] = u2(xU)
            
            print t, intU1[i], intU2[i]
        
            # Plot solution
        s = '$d$ = ' + str(cd) 
        plt.figure(0)
        plt.plot(intT,intU1, label=s)
        plt.figure(1)
        plt.plot(intT,intU2, label=s)

    
    plt.figure(0)
    plt.xlabel('$t$')
    plt.ylabel('$u_1(t)$')
    plt.legend(loc = 4)
    plt.figure(1)
    plt.xlabel('$t$')
    plt.ylabel('$u_2(t)$')
    plt.legend(loc = 4)
    plt.show()
plot(u1, interactive=True)
#plot(u2)
