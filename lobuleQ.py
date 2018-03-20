from dolfin import *
import matplotlib.pyplot as plt
import numpy as np
from itertools import cycle

dyn = 10.0
cc = 0.01
cd = 0.01
cr = cd
parameters["allow_extrapolation"] = True

# Generate mesh and plot
mesh0 = 'lobuleMesh/lobule3.xml'
mesh = Mesh(mesh0)
if mesh0 == 'lobuleMesh/lobule1.xml':
    mesh1 = 'l1'
    facets = MeshFunctionSizet(mesh, "lobuleMesh/lobule1_facet_region.xml")
if mesh0 == 'lobuleMesh/lobule2.xml':
    mesh1 = 'l2'
    facets = MeshFunctionSizet(mesh, "lobuleMesh/lobule2_facet_region.xml")
if mesh0 == 'lobuleMesh/lobule3.xml':
    mesh1 = 'l3'
    facets = MeshFunctionSizet(mesh, "lobuleMesh/lobule3_facet_region.xml")
if mesh0 == 'lobuleMesh/lobule4.xml':
    mesh1 = 'l4'
    facets = MeshFunctionSizet(mesh, "lobuleMesh/lobule4_facet_region.xml")
if mesh0 == 'lobuleMesh/lobule5.xml':
    mesh1 = 'l5'
    facets = MeshFunctionSizet(mesh, "lobuleMesh/lobule5_facet_region.xml")

emesh = Mesh("lobuleMesh/lobule5.xml")
efacets = MeshFunctionSizet(mesh, "lobuleMesh/lobule5_facet_region.xml")

info(mesh)
plot(mesh, interactive=True)

enT = 10000
T = 1.0
edt = T/enT

z_series = TimeSeries("store/zstorage")

# function espace
eV = FunctionSpace(emesh, "CG", 1)

eu11 = interpolate(Constant(0.0), eV)
eu22 = interpolate(Constant(0.0), eV)

et = 0.0
eW = eV * eV
ez = Function(eW)
eu1 = TrialFunction(eV) 
eu2 = TrialFunction(eV)
ev1 = TestFunction(eV)
ev2 = TestFunction(eV)
 
ea1 = cc*eu1*ev1*dx + edt*dot(grad(eu1), grad(ev1))*dx 
ea2 = eu2*ev2*dx + edt*cd*dot(grad(eu2), grad(ev2))*dx
# ea = ea1 + ea2
eL1 = cc*eu11*ev1*dx - edt*cr*(eu11-eu22)*ev1*dx
eL2 = eu22*ev2*dx - edt*cr*(eu22-eu11)*ev2*dx
# eL = eL1 + eL2

eu1 = Function(eV) 
eu2 = Function(eV)
# for i in range(1, enT + 1):
#     et = i*edt
#     print(et)
#    
#     g = 1.0 - np.exp(-dyn*et)
#    
#     ebc1 = DirichletBC(eV, Constant(g), efacets, 16)
#     ebc2 = DirichletBC(eV, Constant(0.0), efacets, 18)
#     ebcs = [ebc1, ebc2]
#    
#     #solution
#     solve(ea1 == eL1, eu1, ebcs)
#     solve(ea2 == eL2, eu2)
#     assign(ez, [eu1, eu2])
#     eu11.assign(eu1)
#     eu22.assign(eu2)
#    
#     z_series.store(ez.vector(), et)
 
# plot(eu1)
# plot(eu2)
g = 0.0

lines = ["k-","k--","k-.","k:"]
linecycler1 = cycle(lines)
linecycler2 = cycle(lines)
nTlist = [500]
for nT in nTlist:
    dt = T/nT

    # function space
    V = FunctionSpace(mesh, "CG", 1)

    intT = np.linspace(0.0, T, nT + 1)
    intU1 = np.linspace(0.0, T, nT + 1)
    intU2 = np.linspace(0.0, T, nT + 1)
    intT[0] = 0.0
    intU1[0] = 0.0
    intU2[0] = 0.0
    
    ui1 = Constant(0.0)
    #ui1 = Expression('0.2*sqrt(x[0]*x[0] + x[1]*x[1])')
    u11 = interpolate(ui1, V)
    u1file = File("res1/u1.pvd")
    u1file << u11
    
    ui2 = Constant(0.0)
    #ui2 = Expression('0.1*sqrt(x[0]*x[0] + x[1]*x[1])')
    u22 = interpolate(ui2, V)
    u2file = File("res1/u2.pvd")
    u2file << u22
    
    #W = V * V
    #z = Function(W)
    u1 = TrialFunction(V) 
    u2 = TrialFunction(V)
    v1 = TestFunction(V)
    v2 = TestFunction(V)

    # variational form
    a1 = cc*u1*v1*dx + dt*dot(grad(u1), grad(v1))*dx 
    a2 = u2*v2*dx + dt*cd*dot(grad(u2), grad(v2))*dx 
    #a = a1 + a2
    L1 = cc*u11*v1*dx - dt*cr*(u11-u22)*v1*dx
    L2 = u22*v2*dx - dt*cr*(u22-u11)*v2*dx
    #L = L1 + L2

    u1 = Function(V, name="u1") 
    u2 = Function(V, name="u2")
    t = 0.0
    for i in range(1, nT + 1):
        t = i*dt

        g = 1.0 - np.exp(-dyn*t)

        bc1 = DirichletBC(V, Constant(g), facets, 16)
        bc2 = DirichletBC(V, Constant(0.0), facets, 18)
        bcs = [bc1, bc2]

        # solution
        solve(a1 == L1, u1, bcs)
        solve(a2 == L2, u2)

        u11.assign(u1)
        u1file << u1
        u22.assign(u2)
        u2file << u2

        z_series.retrieve(ez.vector(), t)
        (eu1, eu2) = ez.split()

        qu1 = interpolate(eu1, V)
        qu2 = interpolate(eu2, V)
        intT[i] = t
        intU1[i] = errornorm(u1, eu1, norm_type='l2')#norm(u1.vector()-qu1.vector(), 'linf')#
        intU2[i] = errornorm(u2, eu2, norm_type='l2')#norm(u2.vector()-qu2.vector(), 'linf')#

        print t, intU1[i], intU2[i]

    # Plot solution
    s = 'dt = ' + str(dt) 
    plt.figure(0)
    plt.plot(intT,intU1, next(linecycler1), label=s)
    #plt.plot(intT,intU1, label=s)
    plt.figure(1)
    plt.plot(intT,intU2, next(linecycler2), label=s)
    #plt.plot(intT,intU2, label=s)

plt.figure(0)
plt.xlabel('$t$')
plt.ylabel('$\\varepsilon_1(t)$')
plt.yscale('log')
plt.ylim(1e-5, 1e2)
plt.legend(loc = 2)
f1 = 'e1' + str(mesh1) + 'r' + str(cr) + '.png'
plt.savefig(f1, bbox_inches=0)
plt.clf()
plt.figure(1)
plt.xlabel('$t$')
plt.ylabel('$\\varepsilon_2(t)$')
plt.yscale('log')
plt.ylim(1e-5, 1e2)
plt.legend(loc = 2)
f2 = 'e2' + str(mesh1) + 'r'  + str(cr) + '.png'
plt.savefig(f2, bbox_inches=0)
plt.clf()

plot(u1)
plot(u2)
interactive()