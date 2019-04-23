
import os,sys

job_template = '''#!/bin/bash

### Set the job name
#SBATCH -J %s

#SBATCH -p hsw_v100_32g

### Specify the number of cpus for your job.
#SBATCH -N 1                 # total number of nodes
#SBATCH --exclusive



module load cuda/10.1.156
module load pgi/18.1

export GECKO_CONFIG_FILE=%s
export GECKO_POLICY=%s

cd %s
rm -f %s

'''

gecko_config_folder_base = "/home/mghane/gecko-configs/"
rodinia_base_folder = "/home/mghane/gecko-rodinia/openacc-w-normal/" 


app_list = ['cfd', 'srad_v2']
iteration_count = 5


for gpu_count in range(1, 5):
	for hc in range(10, 99, 10):
		config_name = "h1g%d" % (gpu_count)
		config_file = gecko_config_folder_base + config_name
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
			result_file = "result-%s-%d-%d" % (app, gpu_count, hc)
			job_filename = "job-%s-%d-%d" % (app, gpu_count, hc)
			app_full = rodinia_base_folder + app
			job_text = job_template % ("%s-%d-%d" % (app, gpu_count, hc),config_file, percent, app_full, result_file)
			for i in range(iteration_count):
				job_text += "sh run >> %s \n" % (result_file)
				if(i < iteration_count-1):
					job_text += "sleep 3 \n"

			job_text += "\n\n"
			
			f = open(job_filename, "w")
			f.write(job_text)
			f.close()

			os.system("sbatch %s" % (job_filename))






