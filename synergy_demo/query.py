import json
import urllib2
import sys
from time import sleep

name2index = {} 
index = 1 
myindex = -1
#size = 1074350
size = 107435
#size = 10743

def query():
	data = 	{
	    "statements" : [ {"statement" : "MATCH (a)-[:AUTHORED]->(paper)-[:AUTHORED_BY]->(b) WHERE a<>b RETURN a.auid,b.auid SKIP 0 LIMIT " + str(size) + ";"} ]
	    }
	req = urllib2.Request('http://ganxis2.nest.rpi.edu:7474/db/data/transaction/commit');
	req.add_header('Content-Type', 'application/json')
	req.add_header('Author', 'application/json')
	req.add_header("Authorization", "Basic bmVvNGo6dTJDcHlrZms=");
	req.add_header('Content-Type', 'application/json')
	
	response = urllib2.urlopen(req, json.dumps(data))
	ret = eval(response.read())
	return ret

def getIndex(thename):
	thename = str(thename)
	global name2index, index
	if thename in name2index:
		return name2index[thename]
	else:
		name2index[thename] = index
	index += 1
	return index - 1

def saveIndex():
	global name2index
	txt = open("./name2index.txt", "w")
	txt.write( str(name2index) + "\r\n" )
	txt.close()

def loadIndex():
	global name2index
	txt = open("./name2index.txt", "r")
	name2index = eval( txt.read() ) 
	txt.close()

def main():
	txt = open("graph.txt", "w")
	ret = query()
    #1074350
	for i in range (size):
		pair = ret["results"][0]["data"][i]["row"]
		txt.write(str(getIndex(pair[0])) + "\t" + str(getIndex(pair[1])) + "\n") 
		sys.stdout.write('\r')
		sys.stdout.write(str( 100.0 * i/size))
		sys.stdout.flush()

	txt.close()

main()
print "     finished"
saveIndex()
