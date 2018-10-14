import sys, os, subprocess, csv, shlex, re, time

def get_dsid(datasetname):
    matches = re.findall(r"\D.(\d{6}).\D", datasetname)
    return matches

def get_tag(tag_letter, datasetname):
    matches = re.findall(r"..*%s(\d{4}).*."%(tag_letter), datasetname)
    matches.extend(re.findall(r"..*%s(\d{5})..*"%(tag_letter), datasetname))
    return matches

def generate_sample_list(filelistname):
    """
    We use sample lists which are stored in spreadsheets to coordinate which samples to run over
    We can use something similar to assign a category to a sample
    """
    samplelist = list(csv.reader(open(filelistname, 'rb'), delimiter='\t'))
    # Find category headings
    categories = []
    thisSample = "None"
    for i,sample in enumerate(samplelist):
        if i == 0:
            sample.insert(1, "Category")
            continue
        if sample[0].startswith("###"):
            thisSample = sample[0].split("###")[1].strip()
            categories.append( (i, thisSample) )
        else:
            sample.insert(1, thisSample)
    # Now remove the sample headings in reverse order
    for ind,cat in reversed(categories):
        del samplelist[ind]
    # Now print our finalised list
    return samplelist

def get_datasets(scope, tag, version, suffix):
    """
    scope: group.phys-higgs, user.xyz
    tag: 212XX (ie 21.2.XX.Y)
    version : Y (ie 21.2.XX.Y)
    suffix: 1l,2l,1l_sys,2l_sys
    """
    # Our rucio listing command
    cmd = "rucio ls --short --filter type=CONTAINER %s:%s.*TTHbb*%s*%s*%s_out.root"%(scope, scope, tag, version, suffix)
    print cmd
    # Process this and store the output
    proc = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    proc.wait()
    stdout,stderr = proc.communicate()
    datasetlist = stdout.split()
    return datasetlist

def generate_sample_datasets(samplelist, datasetlist):
    sample_datasets = {}
    for dataset in datasetlist:
        dsid   = get_dsid(dataset)
        etag   = get_tag("e",dataset)
        rtag   = get_tag("r",dataset)
        stag   = get_tag("s",dataset)
        # Match all the things
        result = filter(lambda x : any(y in x[0] for y in dsid) and any(y in x[0] for y in etag) and any(y in x[0] for y in rtag) and any(y in x[0] for y in stag), samplelist)
        #print dataset,"->",result
        if result:
            r = result[0] # Only need one result
            sample = r[1]
            if sample not in sample_datasets:
                sample_datasets[sample] = []
            sample_datasets[sample].append(dataset)
    return sample_datasets

def configure_download_jobs(outdir, sample_datasets):
    subprocess.call("mkdir -p %s"%(outdir), shell=True)
    commands = []
    for sample in sample_datasets:
        download_location = outdir+"/"+sample
        datasets = " ".join(sample_datasets[sample])
        cmd = "rucio -T 30 download --ndownloader 5 --dir %s %s"%(download_location, datasets)
        commands.append(cmd)
    return commands

def run_and_monitor(commands):
    """
    We should try to run rucio download commands and monitor the state
    """
    rucioLog     = open("rucio.log","a")
    rucioSummary = open("rucio.summary","a")

    rucioLog.write( time.strftime("%x %X") + "\n" )
    rucioSummary.write( time.strftime("%x %X") + "\n" )

    for cmd in commands:
        print cmd
        proc = subprocess.Popen(shlex.split(cmd), stdout=rucioSummary, stderr=rucioLog)
        try:
            proc.wait()
        except KeyboardInterrupt:
            print "Terminating process as caught Ctrl+C"
            proc.kill()
            sys.exit(1)


if __name__ == "__main__":
    # Provide your download location as argument
    outdir = sys.argv[1]
    # Parse the input and output datasets
    samplelist      = generate_sample_list("../share/MC16a_inputs.txt")
    datasetlist     = get_datasets("group.phys-higgs","21216","1","2l")
    sample_datasets = generate_sample_datasets(samplelist, datasetlist)
    # Configure rucio submission jobs
    commands        = configure_download_jobs(outdir,sample_datasets)
    # Start running and logging
    run_and_monitor(commands)
