
def parse_bfs(lines):
	nums = list()
	for l in lines:
		if "Total time:" in l:
			if "us" in l:
				t = float(l.split()[-1][:-2])
			else:
				t = float(l.split()[-1])
			nums.append(t)
	return nums

def parse_cfd(lines):
	nums = list()
	for l in lines:
		if "seconds per iteration" in l:
			t = float(l.split()[0]) * 1E6
			nums.append(t)
	return nums

def parse_gaussian(lines):
	nums = list()
	for l in lines:
		if "Time for kernels:" in l:
			t = float(l.split()[-2]) * 1E6
			nums.append(t)
	return nums


def parse_hotspot(lines):
	nums = list()
	for l in lines:
		if "Total time:" in l:
			t = float(l.split()[-1][:-2])
			nums.append(t)
	return nums

def parse_lavaMD(lines):
	nums = list()
	i = 0
	while i < len(lines):
		l = lines[i]
		if "Total time:" in l:
			i += 1
			l = lines[i]
			t = float(l.split()[0])
			nums.append(t)
		i += 1

	return nums


def parse_lud(lines):
	nums = list()
	for l in lines:
		if "Time consumed(ms):" in l:
			t = float(l.split()[-1])
			nums.append(t)
	return nums


def parse_nn(lines):
	nums = list()
	for l in lines:
		if "Total time:" in l:
			t = float(l.split()[-1][:-2])
			nums.append(t)
	return nums


def parse_nw(lines):
	nums = list()
	for l in lines:
		if "Total time:" in l:
			t = float(l.split()[-1][:-2])
			nums.append(t)
	return nums

def parse_pathfinder(lines):
	nums = list()
	for l in lines:
		if "timer:" in l:
			t = float(l.split()[-1][:-2])
			nums.append(t)
	return nums

def parse_particlefilter(lines):
	if len(lines) == 0:
		print "UNABLE TO LOAD DATA"
		return [0]
	l = lines[-1]
	nums = []
	f = l.split()[-1]
	f = float(f)
	nums.append(f)
	return nums

def parse_srad_v2(lines):
	nums = list()
	for l in lines:
		if "Total Time:" in l:
		# if "Main Loop Time:" in l:
			t = float(l.split()[-1][:-2])
			nums.append(t)
	return nums





def call_parse(app, lines):
	if app == "bfs":
		return parse_bfs(lines)
	elif app == "cfd":
		return parse_cfd(lines)
	elif app == "gaussian":
		return parse_gaussian(lines)
	elif app == "hotspot":
		return parse_hotspot(lines)
	elif app == "lavaMD":
		return parse_lavaMD(lines)
	elif app == "lud":
		return parse_lud(lines)
	elif app == "nn":
		return parse_nn(lines)
	elif app == "nw":
		return parse_nw(lines)
	elif app == "pathfinder":
		return parse_pathfinder(lines)
	elif app == "particlefilter":
		return parse_particlefilter(lines)
	elif app == "srad_v2":
		return parse_srad_v2(lines)
