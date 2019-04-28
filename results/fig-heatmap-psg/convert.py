
import os,sys

app_list = ["cfd", "srad_v2"]

main_folder = "../heatmap/psg-heatmap-results"

for app in app_list:
	for gid in range(1,5):
		for hc in range(10,99, 10):
			src = "%s/%s/result-%s-%d-%d" % (main_folder, app, app, gid, hc)
			dst = "./data/result-%s-h1g%d-%.2f" % (app, gid, 1.0*hc)

			cmd = "cp %s %s" % (src, dst)
			print cmd

			os.system(cmd)
