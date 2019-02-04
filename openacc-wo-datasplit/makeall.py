
import os,sys,glob

cwd = os.getcwd()

app_list = ["old-vector_add", "old-stencil", "acc-vector_add", "acc-stencil"]

# file_list = glob.glob("%s/*" % (cwd))
file_list = ["%s/%s" % (cwd, f) for f in app_list]

action = ""

if len(sys.argv) > 1:
	action = sys.argv[1]

for f in file_list:
	full_path = f

	if os.path.isfile(full_path):
		continue

	os.chdir(full_path)
	os.system("make %s" % (action))

