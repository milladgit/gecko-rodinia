
import os,sys,glob

def get_name(filename):
	_v = filename
	__v = _v[::-1]
	ind = __v.find("/")
	s = __v[:ind]
	return s[::-1]


gecko_config_folder_base = "/home/millad/gecko-configs/"


config_files = glob.glob(gecko_config_folder_base + "*")
config_files = sorted(config_files)

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

	for cfilename in config_files:
		os.environ["GECKO_CONFIG_FILE"] = cfilename
		config_name = get_name(cfilename)
		output_filename = "%s-%s.txt" % (app, config_name)
		os.environ["GECKO_POLICY"] = "static"
		if "-p" in config_name:
			ind = config_name.find("-p")
			host_share = float(config_name[ind+2:])
			num_gpus = int(config_name[3:4])
			gpu_share = 100.0 - host_share
			gpu_share /= num_gpus
			policy = "percentage:["
			for i in range(num_gpus):
				policy += "%.2f," % (gpu_share)
			policy += "%.2f]" % (host_share)
			os.environ["GECKO_POLICY"] = policy

		os.system("rm -f %s" % (output_filename))

		for i in range(iteration_count):
			os.system("sh run >> %s" % (output_filename))

