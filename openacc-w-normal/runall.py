
import os,sys,glob

gecko_config_folder_base = "/home/millad/gecko-rodinia/config/"

cwd = os.getcwd()

app_list = ["bfs", "cfd", "gaussian", "hotspot", "lavaMD", "lud", "nn", "nw", "particlefilter", "pathfinder", "srad_v2"]

# file_list = glob.glob("%s/*" % (cwd))
file_list = ["%s/%s" % (cwd, f) for f in app_list]


iteration_count = 5

for a in zip(app_list, file_list):
	full_path = a[1]
	app = a[0]

	if os.path.isfile(full_path):
		continue

	os.chdir(full_path)

	for devcount in [1, 2, 3, 4]:
		output_filename = "%s-%d-gecko-summit.txt" % (app, devcount)

		os.environ["GECKO_CONFIG_FILE"] = "%s/gecko%d.conf" % (gecko_config_folder_base, devcount)

		os.system("rm -f %s" % (output_filename))

		for i in range(iteration_count):
			os.system("sh run >> %s" % (action))

