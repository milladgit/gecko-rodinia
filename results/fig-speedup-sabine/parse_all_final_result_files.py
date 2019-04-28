
import os,sys,glob

sys.path.insert(0, '../python-code/')

from call_parse_funcs import *

if len(sys.argv) < 3:
	print "Please provide input: <folder> <gpu_count>"
	exit(1)


main_folder = "%s" % (sys.argv[1])
gpu_count = int(sys.argv[2])

app_list = ["bfs", "cfd", "gaussian", "hotspot", "lavaMD", "lud", "nn", "nw", "particlefilter", "pathfinder", "srad_v2"]


for app in app_list:

	results = []

	for gpu_id in range(gpu_count):

		fname = "%s/result-%s-gpu%d" % (main_folder, app, gpu_id+1)

		f = open(fname, "r")
		lines = f.readlines()
		f.close()

		if len(lines) == 0:
			# print "Unable to load data for %s" % (fname)
			nums = [0]
		else:
			nums = call_parse(app, lines)

		arch = "Gecko(f)-k=%d" % (gpu_id)
			
		if len(nums) == 0:
			results.append(0)
		else:
			# results.append("%s;%.2f" % (arch, sum(nums)/len(nums)))
			results.append(sum(nums)/len(nums))


	# v0 = max(results)
	v0 = results[0]
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

