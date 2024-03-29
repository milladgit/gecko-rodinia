import os,sys,glob

app = "gaussian"
folder = "openacc-w-normal/" + app

file_list = ["host", "gpu1", "gpu2", "gpu3", "gpu4", "gpu4-p0.5", "gpu4-p1", "gpu4-p2"]
file_list = ["%s/%s-%s.txt" % (folder, app, _f) for _f in file_list]

results = []

for fname in file_list:
	f = open(fname, "r")
	lines = f.readlines()
	f.close()

	nums = list()
	for l in lines:
		if "Time for kernels:" in l:
			t = float(l.split()[-2]) * 1E6
			nums.append(t)
	fname = fname[::-1]
	ind = fname.find("/")	
	fname2 = fname[:ind]
	fname2 = fname2[::-1]
	fname2 = fname2[:-4]
	arch = "NOTSPECIFIED"
	if "host" in fname2:
		arch = "Gecko(a)"
	elif "gpu1" in fname2:
		arch = "Gecko(e)-k=1"
	elif "gpu2" in fname2:
		arch = "Gecko(e)-k=2"
	elif "gpu3" in fname2:
		arch = "Gecko(e)-k=3"
	elif "gpu4-p" in fname2:
		ind = fname2[::-1].find("p")
		perc = fname2[::-1][:ind]
		perc = perc[::-1]
		perc = float(perc)
		if perc < 1:
			arch = "Gecko(f)-k=4-HS=%.1f%%" % (perc)
		else:
			arch = "Gecko(f)-k=4-HS=%.0f%%" % (perc)
	elif "gpu4" in fname2:
		arch = "Gecko(e)-k=4"
	if len(nums) == 0:
		results.append(0)
	else:
		# results.append("%s;%.2f" % (arch, sum(nums)/len(nums)))
		results.append(sum(nums)/len(nums))


if results[0] > 0:
	v0 = max(results)
	results = [v0/_v for _v in results]

for r in results:
	print r,";",
print ""
