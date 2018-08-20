
import os,sys,glob

cwd = os.getcwd()

file_list = glob.glob("%s/*" % (cwd))

action = ""

if len(sys.argv) > 1:
	action = sys.argv[1]

for f in file_list:
	full_path = f

	if os.path.isfile(full_path):
		continue

	os.chdir(full_path)
	os.system("make %s" % (action))
	
