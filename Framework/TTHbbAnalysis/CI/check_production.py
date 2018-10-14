#!/bin/env python
# Script to monitor production status
# Ian Connelly
# 15 Jan 2018

import subprocess, json, os, sys

def listTags():
    """ Use the command git tag -l and parse """
    return None

def calcPercentage(dsinfo):
    # "dsinfo": {"nfilesfailed": 0, "nfiles": 61, "pctfailed": 0, "pctfinished": 0, "nfilesfinished": 0}
    nFiles      = dsinfo["nfiles"]
    pctComplete = dsinfo["pctfinished"]
    return {"nFiles":nFiles, "pctComplete":pctComplete}

def getKeys():
    return ["taskname", "username", "reqid", "parent_tid", "superstatus", "jeditaskid", "modificationtime", "status", "starttime"]


def checkAndGetCookie():
    if not os.path.exists("bigpanda.cookie.txt"):
        cmd='ssh tthbbana@lxplus.cern.ch "cern-get-sso-cookie -u https://bigpanda.cern.ch/ -o bigpanda.cookie.txt;"'
        subprocess.call(cmd,shell=True)
    return

def MakeJSONList(J):
    """ Make a list from JSON info. Reduce it down to the parts we care about """
    json_data = open(J)
    data = json.load(json_data)
    json_data.close()
    keys = getKeys()
    slim_data = []
    # Also hardcode some tasks to skip
    skip_tasks = [ ]
    # Make a list containing a dictionary for each job
    for element in data:
        # Error catching for accidental submission
        if element["jeditaskid"] in skip_tasks:
            print "Skipping %s as this was retried with a bad configuration"%(element["taskname"])
            continue
        
        #print element
        slim_dict = { key: element[key] for key in element if key in keys }
        slim_dict.update( calcPercentage(element["dsinfo"]) )
        slim_data.append(slim_dict)
    # Check if we have repeated outDS entries as this represented resubmitted jobs
    outDSDict = {}
    for i,x in enumerate(slim_data):
        taskname   = x["taskname"]
        jeditaskid = x["jeditaskid"]
        if taskname not in outDSDict:
            outDSDict[taskname] = []
        outDSDict[x["taskname"]].append((i, jeditaskid))
    # If our new dict has more than one entry per taskname, we should only keep the largest jeditaskid
    indices_to_remove = []
    # We should check for new suffixes
    for taskname in outDSDict:
        #nonChannelTaskname = "-".join(taskname.split("-")[:-1])
        nonChannelTaskname = taskname.split("_v")[0]
        channel            = taskname.split("-")[-1]
        filteredList = filter( lambda x : nonChannelTaskname in x and channel in x, outDSDict.keys() )
        if len(filteredList) == 1:
            continue
        # Message
        print "Retried job with new suffix found for :",taskname
        print filteredList        
        # Which taskname has the newest jeditaskid, this is the one we should keep track of
        newList = []
        for task in filteredList:
            for entry in outDSDict[task]:
                newList.append( (task, entry[0], entry[1]) )
        # Now sort this new list by jeditaskid like below
        entry = sorted(newList, key = lambda x:x[2], reverse=True)
        indices_to_remove.extend([ x[1] for x in entry[1:] ])
        print "Keep only :",entry[0]
        if "[V]" not in slim_data[entry[0][1]]["superstatus"]:
            slim_data[entry[0][1]]["superstatus"] = slim_data[entry[0][1]]["superstatus"]+" [V]"
    for taskname in outDSDict:
        # One entry == One job == Keep
        if len(outDSDict[taskname]) == 1:
            continue
        # Zero should never happen...
        elif len(outDSDict[taskname]) == 0:
            print "Warning - We have zero jobs for %s...?"%(taskname)
        # More than one == Delete some info
        else:
            print "Taskname %s has been retried, only store the latest job"%(taskname)
            # Sort on second entry, reverse will put the largest job ID as first entry
            entry = sorted(outDSDict[taskname], key = lambda x:x[1], reverse=True)
            to_remove = indices_to_remove.extend([ x[0] for x in entry[1:] ])
            # Decorate the job which has been retried
            slim_data[entry[0][0]]["superstatus"] = slim_data[entry[0][0]]["superstatus"]+" [R]"
                
    # Now we have a list of indices which should be delted from slim_data
    # We need to run over them in reverse, such as not to throw off the ordering of earlier elements
    # Should use a set to prevent mutliple entries from the two loops
    for i in sorted(set(indices_to_remove), reverse=True):
        print i
        del slim_data[i]
    # Return our list of dictionaries which removed retried jobs
    return slim_data


def getProductionJobs(tag="", version=""):
    """ Get the json file for all production jobs. This may need to be configured carefully to pick up all tasks """
    # Search string
    tasknamestring = "group.phys-higgs.*TTHbb*%s*%s*"%(tag, version)
    # Get the bigpanda cookie for sensible scraping
    checkAndGetCookie()
    # Define the command to run via ssh
    # TaskID
    # cmd = "ssh tthbbana@lxplus.cern.ch 'curl -b bigpanda.cookie.txt -H '\"'\"'Accept: application/json'\"'\"' -H '\"'\"'Content-Type: application/json'\"'\"' \"https://bigpanda.cern.ch/tasks/?jeditaskid=%s&json\"' "%()
    # Taskname
    # Try getting the streamed info as it seems we are capped at 1000 entries in json
    cmd = "ssh tthbbana@lxplus.cern.ch 'curl -b bigpanda.cookie.txt -H '\"'\"'Accept: application/json'\"'\"' -H '\"'\"'Content-Type: application/json'\"'\"' \"https://bigpanda.cern.ch/tasks/?taskname=%s&json&display_limit=4000&limit=4000&days=60\"' "%(tasknamestring)
    JSONFile = open("json.json","w")
    proc = subprocess.Popen(cmd,stdout=JSONFile,shell=True)
    proc.wait()
    JSONFile.close()
    data = MakeJSONList("json.json")
    return data

def configureColour(status, substatus):
    status = status.replace(" [R]","") # Clean string if it was retried
    status = status.replace(" [V]","") # Clean string if it was resubmitted  
    colour = ""
    if status == "running":
        colour = "%GREEN%"
    elif status == "submitting" and substatus == "scouting":
        colour = "%OLIVE%"
    elif status == "submitting" and substatus == "pending":
        colour = "%GRAY%"
    elif status == "registered":
        colour = "%GRAY%"
    elif status == "broken" or status == "failed" or status == "aborted":
        colour = "%RED%"
    elif status == "done":
        colour = "%BLUE%"
    elif status == "finished":
        colour = "%TEAL%"
    else:
        colour = "%BLACK%"
    return colour

def storeFinishedOutDS(status):
    status = status.replace(" [R]","") # Clean string if it was retried
    status = status.replace(" [V]","") # Clean string if it was resubmitted
    finishedStatus = ["finished","done"]
    return any(status == state for state in finishedStatus)

def doWeRetryJob(status, pct, isData):
    status = status.replace(" [R]","") # Clean string if it was retried
    status = status.replace(" [V]","") # Clean string if it was resubmitted
    # If in finished state and over 95% and not data, do not retry further
    if status == "finished" and pct >= 95 and not isData:
        return False
    retryStatus = ["failed", "finished", "aborted"]
    return any(state in status for state in retryStatus)

def doWeGetCLIParams(status):
    status = status.replace(" [R]","") # Clean string if it was retried
    status = status.replace(" [V]","") # Clean string if it was resubmitted
    retryStatus = ["broken"]
    return any(state in status for state in retryStatus)

def getProgressColour(pct):
    # Depending on percentage complete, choose a html colouring
    pct = int(pct)
    if pct < 50:
        # Light red
        return "#FBB1A0"
    elif pct >= 50 and pct < 90:
        # Light orange
        return "#F5DCBA"
    elif pct >= 90 and pct < 98:
        # Light green
        return "#AAD5A7"
    else:
        # Dark green
        return "#608C5E"

def writeTwikiTable(data, tag, version):
    # Monitor job count
    nJobs = {"done": 0, "finished": 0, "running": 0, "submitting": 0, "broken": 0, "failed": 0, "aborted": 0, "registered": 0, "other": 0}
    # Write twiki table and also filelists
    completedOutDS = ""
    retryCommands = {}
    cliCommands = {}
    outStr = ""
    outStr += " | * %s * |  * %s * | * %s * | * %s * | * %s * | * %s * |  \n"%("user","outDS","status","jobID","updated","progress")
    for entry in data:
        if entry["superstatus"] in nJobs:
            nJobs[ entry["superstatus"] ] += 1
        else:
            nJobs["other"] += 1
        colour = configureColour(entry["superstatus"], entry["status"])
        # Write out the page
        if storeFinishedOutDS(entry["superstatus"]):
            completedOutDS += "%s_out.root \n"%(entry["taskname"][:-1]) # Strip the trailing
         # Make a retry command
        if doWeRetryJob(entry["superstatus"], entry["pctComplete"], "data" in entry["taskname"]):
            user = entry["username"]
            if user not in retryCommands:
                retryCommands[user] = [ "pbook -c \"sync()\""]
            retryCommands[user].append( "pbook -c \"retry(%s)\" # %s : %s "%(entry["reqid"], entry["superstatus"], entry["taskname"]) )
        if doWeGetCLIParams(entry["superstatus"]):
            user = entry["username"]
            if user not in cliCommands:
                cliCommands[user] = [ "# Run this on lxplus for convinience", "cern-get-sso-cookie -u https://bigpanda.cern.ch/ -o bigpanda.cookie.txt" ]
            cliCommands[user].append( "# Taskname : %s"%(entry["taskname"]) )
            cliCommands[user].append( """curl -b bigpanda.cookie.txt -H 'Accept: application/json' -H 'Content-Type: application/json' "https://bigpanda.cern.ch/task/%s/" 2>/dev/null | python -c "import sys, json; print json.load(sys.stdin)['taskparams']['cliParams']" """%(entry["jeditaskid"]) )
        # Edit
        entry["username"] = entry["username"].split()[0].title()
        entry["jeditaskid"] = "[[https://bigpanda.cern.ch/task/%s/][%s]]"%(entry["jeditaskid"], entry["jeditaskid"])
        # Add colour
        for element in getKeys(): #entry:
            entry[element] = "%s %s %s"%(colour, entry[element], "%ENDCOLOR%")
        outStr += " | %s | %s | %s (%s) | %s | %s <td style=\"background-color:%s; text-align:center\"> <progress max=\"100\" value=\"%s\">  </td> | \n"%(entry["username"], entry["taskname"], entry["superstatus"], entry["status"], entry["jeditaskid"], entry["modificationtime"], getProgressColour(entry["pctComplete"]), entry["pctComplete"] )

    # Write out the twiki table which has just been formatted
    outFileName = "%s_%s.twiki.table"%(tag, version)
    outFile = open(outFileName,"w")
    outFile.write(outStr)
    outFile.close()
    # Write out completed jobs
    completedFileName = "outDS.%s_%s.txt"%(tag, version)
    outFile = open(completedFileName,"w")
    outFile.write(completedOutDS)
    outFile.close()
    # Write out file with retry commands
    retryFileName = "retry.%s_%s.txt"%(tag, version)
    retryFile = open(retryFileName,"w")
    retryFile.write( "---+ Retry commands \n" )
    retryFile.write( "---++ Finished Jobs (< 95%) \n" )
    for user in retryCommands:
        filterList = filter( lambda x: "finished" in x, retryCommands[user] )
        if len(filterList) == 0:
            continue
        retryFile.write( ' %TWISTY{mode="div" suffix="" hidelink="Hide..." showlink=" '+user+' " showimgright="" hideimgright=""}% %BR% \n' )
        retryFile.write( "\n%BR%".join( filterList ) )
        retryFile.write( "%ENDTWISTY%\n" )
        retryFile.write( "\n" )
    retryFile.write( "---++ Failed Jobs \n" )
    for user in retryCommands:
        filterList = filter( lambda x: "failed" in x, retryCommands[user] )
        if len(filterList) == 0:
            continue
        retryFile.write( ' %TWISTY{mode="div" suffix="" hidelink="Hide..." showlink=" '+user+' " showimgright="" hideimgright=""}% %BR% \n' )
        retryFile.write( "\n%BR%".join( filterList ) )
        retryFile.write( "%ENDTWISTY%\n" )
        retryFile.write( "\n" )
    retryFile.write( "---++ Aborted Jobs \n" )
    for user in retryCommands:
        filterList = filter( lambda x: "aborted" in x, retryCommands[user] )
        if len(filterList) == 0:
            continue
        retryFile.write( ' %TWISTY{mode="div" suffix="" hidelink="Hide..." showlink=" '+user+' " showimgright="" hideimgright=""}% %BR% \n' )
        retryFile.write( "\n%BR%".join( filterList ) )
        retryFile.write( "%ENDTWISTY%\n" )
        retryFile.write( "\n" )                
    retryFile.close()
    # Write out cli-get commands
    cliFileName = "cliParams.%s_%s.txt"%(tag, version)
    cliFile = open(cliFileName,"w")
    for user in cliCommands:
        cliFile.write( ' %TWISTY{mode="div" suffix="" hidelink="Hide..." showlink=" '+user+' " showimgright="" hideimgright=""}% %BR% \n' )
        cliFile.write( "\n%BR%".join(cliCommands[user]) )
        cliFile.write( "%ENDTWISTY%\n" )
        cliFile.write( "\n" )
    cliFile.close()
    # Write out the summary
    summaryFileName = "summary.%s_%s.txt"%(tag, version)
    summaryFile = open(summaryFileName,"w")
    nTot = sum(nJobs.values())
    summaryFile.write("   * %s %.0f \n"%("Total jobs", float(nTot) ))
    for status in nJobs:
        statusStr = configureColour(status, "")+status+"%ENDCOLOR%"
        summaryFile.write("   * %s (%.1f %%) [%.0f] \n"%(statusStr, 100.0*float(nJobs[status])/float(nTot), float(nJobs[status]) ))
    summaryFile.close()
    return outFileName, completedFileName, retryFileName, summaryFileName, cliFileName

def copyFile(filename):
     print "Copy file (%s) to web location"%(filename)
     cmd = "scp %s tthbbana@lxplus.cern.ch:/eos/user/t/tthbbana/www/production/"%(filename)
     proc = subprocess.Popen(cmd, shell=True)
     proc.wait()
     print "Done"
     return

def makeStreamTables(outFile):
    # Read in the table we just made, and then restructure it to make a table per stream
    tableLineList = []
    streams       = []
    header        = ""
    with open(outFile, "r") as inFile:
        for line in inFile.readlines():
            if header == "":
                header = line.strip()
                continue
            line = line.strip()
            stream = line.split("|")[2].split("-")[-1].split("/")[0]
            if stream not in streams:
                streams.append(stream)
            tableLineList.append( (line, stream) )

    includeFileName = outFile+".include"
    includeFile = open(includeFileName,"w")    

    for s in sorted(streams):
        outFileName = outFile+"."+s
        includeFile.write("%INCLUDE{https://atlas-project-tthbbanalysis.web.cern.ch/atlas-project-tthbbanalysis/production/"+outFileName+"}% \n")
        newFile = open(outFileName,"w")
        newFile.write("---++ Stream: %s \n"%(s))
        newFile.write(header + "\n")
        streamFilter = filter( lambda x: s == x[1], tableLineList )
        print s, len(streamFilter)
        for entry in streamFilter:
            newFile.write( entry[0] + "\n" )
        newFile.close()
        copyFile(outFileName)
    # Also copy an include file which might work for adding this to the twiki
    includeFile.close()
    copyFile(includeFileName)
    


def copyOutputToWebpage(outFile, completedFileName, retryFileName, summaryFile, cliFileName):
    """ Copy via out service user to the eos webpage location """
    copyFile(outFile)
    copyFile(completedFileName)
    copyFile(retryFileName)
    copyFile(summaryFileName)
    copyFile(cliFileName)
    return

if __name__ == "__main__":
    tag     = sys.argv[1] #"TTHbb21216"
    version = sys.argv[2] #""
    data = getProductionJobs(tag=tag, version=version)
    outFileName,completedFileName, retryFileName, summaryFileName, cliFileName = writeTwikiTable(data, tag, version)
    copyOutputToWebpage(outFileName, completedFileName, retryFileName, summaryFileName, cliFileName)
    makeStreamTables(outFileName)
