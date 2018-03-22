from dolfin import *
import sys

name = sys.argv[1]
mesh = Mesh(name)
domains_name = name.replace(".xml", "_physical_region.xml")
domains = MeshFunction("size_t", mesh, domains_name)
domains_pvd = File(domains_name + ".pvd")
domains_pvd << domains

