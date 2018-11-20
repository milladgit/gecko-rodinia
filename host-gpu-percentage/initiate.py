
import os,sys

job_file = """#!/bin/bash

### Set the job name
#SBATCH -J %s
#SBATCH -o %s.out
#SBATCH -e %s.err 

#SBATCH -p hsw_v100_32g

### Specify the number of cpus for your job.
#SBATCH -N 1                 # total number of nodes
#SBATCH --exclusive



module load cuda
module load pgi


"""


def distribute(num, count):
	delta = (1.0*num/count)
	ret_array = []
	for c in range(count):
		ret_array.append(delta)
	return ret_array

host_percentage = range(100, -1, -10)

main_folder = os.getcwd()

main_folder += "/../openacc/"

app_list = ['b+tree', 'bfs', 'cfd', 'gaussian', 'hotspot', 'lavaMD', 'lud', 'nn', 'nw', 'particlefilter', 'pathfinder', 'srad_v2']


for app in app_list:
	for gpu_count in [1, 4]:
		config_file = "export GECKO_CONFIG_FILE=/home/mghane/gecko-rodinia/config/gecko_host_%d.conf" % (gpu_count)
		for hp in host_percentage:
			job_name = "%s%d-%d" % (app[0:2], gpu_count, hp)
			file_to_save = job_file % (job_name, job_name, job_name)

			percentage_range = "percentage:[%.2f" % (hp*1.0)
			reminder = 100.0 - hp
			arr = distribute(reminder, gpu_count)
			for a in arr:
				percentage_range += ",%.2f" % (a)
			percentage_range += "]"

			percentage_range = "export GECKO_POLICY=%s" % (percentage_range)



			file_to_save += config_file + "\n"
			file_to_save += percentage_range + "\n"
			file_to_save += "\n"

			file_to_save += "cd %s/%s\n" % (main_folder, app)
			file_to_save += "\n"

			result_filename = "%s-percentage-timing-gpu-%d-host-%d.txt" % (app, gpu_count, int(hp))
			file_to_save += "rm -f %s\n" % (result_filename)
			file_to_save += "\n"

			for i in range(5):
				file_to_save += "sh run &>> %s\n" % (result_filename)
				file_to_save += "sleep 3\n"
				file_to_save += "\n\n"

			filename = "%s-gpu_count-%d-host_percent-%d.job" % (app, gpu_count, int(hp))
			f = open(filename, "w")
			f.write(file_to_save)
			f.close()



