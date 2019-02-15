
import os,sys,glob

from call_parse_funcs import *


system = sys.argv[1]
main_folder = "%s/openacc-w-normal" % (system)

app_list = ["bfs", "cfd", "gaussian", "hotspot", "lavaMD", "lud", "nn", "nw", "particlefilter", "pathfinder", "srad_v2"]
# app_list = ["bfs", "cfd"]

for app in app_list:

	results = []

	file_list = ["host", "gpu1", "gpu2", "gpu3", "gpu4", "gpu4-p0.5", "gpu4-p1", "gpu4-p2"]
	file_list = ["%s/%s/%s-%s.txt" % (main_folder, app, app, _f) for _f in file_list]

	for fname in file_list:
		f = open(fname, "r")
		lines = f.readlines()
		f.close()

		if len(lines) == 0:
			# print "Unable to load data for %s" % (fname)
			nums = [0]
		else:
			nums = call_parse(app, lines)

		fname = fname[::-1]
		ind = fname.find("/")	
		fname2 = fname[:ind]
		fname2 = fname2[::-1]
		fname2 = fname2[:-4]
		arch = "NOTSPECIFIED"
		if "host" in fname2:
			arch = "Gecko(a)"
		elif "gpu1" in fname2:
			arch = "Gecko(e)-k=1"
		elif "gpu2" in fname2:
			arch = "Gecko(e)-k=2"
		elif "gpu3" in fname2:
			arch = "Gecko(e)-k=3"
		elif "gpu4-p" in fname2:
			ind = fname2[::-1].find("p")
			perc = fname2[::-1][:ind]
			perc = perc[::-1]
			perc = float(perc)
			if perc < 1:
				arch = "Gecko(f)-k=4-HS=%.1f%%" % (perc)
			else:
				arch = "Gecko(f)-k=4-HS=%.0f%%" % (perc)
		elif "gpu4" in fname2:
			arch = "Gecko(e)-k=4"
			
		if len(nums) == 0:
			results.append(0)
		else:
			# results.append("%s;%.2f" % (arch, sum(nums)/len(nums)))
			results.append(sum(nums)/len(nums))


	v0 = max(results)
	v0 = results[1]
	results_out = []
	for _v in results:
		if _v == 0.0:
			results_out.append(0)
		else:
			results_out.append(v0/_v)
	results = results_out


	print app + ";",
	for r in results:
		print str(r)+";",
	print ""

