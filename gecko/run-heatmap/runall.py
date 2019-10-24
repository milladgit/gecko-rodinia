
import os,sys,glob

job_template = '''#!/bin/bash

### Set the job name
#SBATCH -J %s

#SBATCH -p gpu

### Specify the number of cpus for your job.
#SBATCH -N 1                 # total number of nodes
#SBATCH --exclusive


module load CUDA/10.0.130
module load pgi/18.7-64-bit

export GECKO_CONFIG_FILE='%s'
export GECKO_POLICY='%s'


cd %s
rm -f %s
%s

'''


gecko_config_folder_base = "/home/mghane2/gecko-configs/"
config_list = ["h1g1", "h1g2"]

rodinia_folder = "/home/mghane2/gecko-rodinia/openacc-w-normal/"

app_list = ["bfs", "cfd", "gaussian", "hotspot", "lavaMD", "lud", "nn", "nw", "particlefilter", "pathfinder", "srad_v2"]
app_list = ["cfd", "srad_v2"]


hc_list = range(10, 99, 10) + [5.0,1.0,0.5,0.1,0.01]


iteration_count = 5

for gpu_count in range(1, 3):
	for hc in hc_list:
		config_name = "h1g%d" % (gpu_count)
		percent = "percentage:[%.2f" % (1.0*hc)

		rest = 100 - hc
		delta = 1.0 * rest / gpu_count
		i = 0
		while i < gpu_count:
			if i == gpu_count-1:
				r = 100.0 - (gpu_count-1)*delta - hc
				percent += ",%.2f" % (r)
			else:
				percent += ",%.2f" % (delta)
			i += 1

		percent += "]"


		for app in app_list:
			job_name = "%s%s%.2f" % (app[0:2], config_name, hc)
			result_filename = "result-%s-%s-%.2f" % (app, config_name, hc)
			job_filename = "job-%s-%s-%.2f" % (app, config_name, hc)
			policy = percent

			app_folder = rodinia_folder + app
			result_filename = app_folder + "/" + result_filename

			conf_filename = gecko_config_folder_base + config_name

			cmd = ""
			for i in range(iteration_count):
				cmd += "sh run >> %s\n" % (result_filename)
				cmd += "sleep 3\n"

			job_content = job_template % (job_name, conf_filename, policy, app_folder, result_filename, cmd)

			f = open(job_filename, "w")
			f.write(job_content)
			f.close()

			#os.system("sbatch %s" % (job_filename))
			print job_filename
			os.system("sh %s" % (job_filename))



