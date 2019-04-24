
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
config_list = ["gpu1", "gpu2"]

rodinia_folder = "/home/mghane2/gecko-rodinia/openacc-w-normal/"

app_list = ["bfs", "cfd", "gaussian", "hotspot", "lavaMD", "lud", "nn", "nw", "particlefilter", "pathfinder", "srad_v2"]


iteration_count = 5

for conf in config_list:
	for app in app_list:
		job_name = "%s-%s" % (app[0:2], conf)
		result_filename = "result-%s-%s" % (app, conf)
		job_filename = "job-%s-%s" % (app, conf)
		policy = "static"

		app_folder = rodinia_folder + app
		result_filename = app_folder + "/" + result_filename

		conf_filename = gecko_config_folder_base + conf

		cmd = ""
		for i in range(iteration_count):
			cmd += "sh run &>> %s\n" % (result_filename)
			cmd += "sleep 3\n"

		job_content = job_template % (job_name, conf_filename, policy, app_folder, result_filename, cmd)

		f = open(job_filename, "w")
		f.write(job_content)
		f.close()

		#os.system("sbatch -p gpu -N 1 --exclusive %s" % (job_filename))
		print job_filename
		os.system("sh %s" % (job_filename))

	

