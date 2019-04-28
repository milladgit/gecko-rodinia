
import os,sys,glob

from call_parse_funcs import *

if(len(sys.argv) < 2):
	print "Please provide the folder."
	exit(1)

folder = sys.argv[1]

# data: 
# columns: 1, 2, 3, 4
# 90: 0.8, 2.4, 2.5, 3.9, 0.0, 4.0, 0.0
# 80: 2.4, 0.0, 4.0, 1.0, 2.7, 0.0, 0.0
# 70: 1.1, 2.4, 0.8, 4.3, 1.9, 4.4, 0.0
# 60: 0.6, 0.0, 0.3, 0.0, 3.1, 0.0, 0.0
# 50: 0.7, 1.7, 0.6, 2.6, 2.2, 6.2, 0.0
# 40: 1.3, 1.2, 0.0, 0.0, 0.0, 3.2, 5.1
# 30: 0.1, 2.0, 4.0, 1.4, 0.0, 1.9, 6.3
# 20: 0.1, 2.0, 4.0, 1.4, 0.0, 1.9, 6.3
# 10: 0.1, 2.0, 4.0, 1.4, 0.0, 1.9, 6.3



def get_results_for_app(app, coeff=1.0):
	print "App: %s" % (app)
	print "data:"
	print "columns: ",
	for i in range(1, 5):
		if i < 4:
			print i, ",",
		else:
			print i

	for percent in range(90,9,-10):
		output_str = "%d: " % (percent)
		for gpu in range(1,5):
			filename = "%s/%s/result-%s-%d-%d" % (folder,app,app, gpu, percent)
			f = open(filename, "r")
			lines = f.readlines()
			f.close()

			nums = call_parse(app, lines)

			if len(nums) == 0:
				print "Error in following file: %s" % (filename)
				continue

			avg = sum(nums)/len(nums) * coeff
			output_str += "%.2f, " % (avg)

		output_str = output_str[:-2]
		print output_str

	print "-------------"



get_results_for_app("cfd")
get_results_for_app("srad_v2", 1e-6) # srad results are in ns scale

