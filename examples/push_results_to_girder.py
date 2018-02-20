## The purpose of this script is to be added to the end of the testing
## done as part of ITKPerformanceBenchmarking
## Hopefully we can set it up so that dashboards that run the
## performance benchmarking will automatically sumbit their json
## files to the girder repository so that analysis can be done
## to understand performance of the toolkit.

import os
import sys
import glob

try:
    import girder_client
except:
    print("girder_client not installed:  pip install girder-client")
    sys.exit(0)

## HACK:  Can't depend on using getpass and requesting password all the time.
##        Hopefully we can find a better way to push data with a "set-once" environmental variable for authentication
##        export GIRDER_APIKEY=XXXq7ZgZe2QMWdrz4vn3Bxr8rxtnUmFnE08lXXXX
##        my_girder_apikey=os.getenv('GIRDER_APIKEY')
##
##        USAGE:  push_data.py <PATH to examples/Testing that contains .json files>
##                push_data.py /Users/johnsonhj/Dashboard/src/ITK-clangtidy/Modules/Remote/PerformanceBenchmarking/examples/Testing/
##
##
import getpass

user = input("Username:")
passwd = getpass.getpass("Password for " + user + ":")

root_path = sys.argv[1]

gc = girder_client.GirderClient(apiUrl='https://data.kitware.com/api/v1')
gc.authenticate(user, passwd)
#gc.inheritAccessControlRecursive('5a8ccf9a8d777f06857838d0')

FOLDER_ID = '5a8ccf9a8d777f06857838d0'  # on data.kitware.com ITKPerformanceTestingData/performance_results/

all_files = glob.glob(root_path+'/*.json')

for this_file in all_files:
    if 0 == 1: ## Check if file already exists in upload
        # file_name=os.path.basename(this_file)
        # current_status = gc.isFileCurrent(FOLDER_ID,file_name, this_file)
        # print(current_status)
        pass

    status = gc.uploadFileToFolder(FOLDER_ID,this_file)
    print("STATUS: {0}".format(status))
    if 0 == 1: # check status and remove local .json file if uploads successfully
        pass
