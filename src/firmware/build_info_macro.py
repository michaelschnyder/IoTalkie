import subprocess
from time import gmtime, strftime
import time
import platform
import getpass


shortRev = (
    subprocess.check_output(["git", "rev-parse", "--short", "HEAD"])
    .strip()
    .decode("utf-8")
)
fullRev = (
    subprocess.check_output(["git", "rev-parse", "HEAD"])
    .strip()
    .decode("utf-8")
)
gmt = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
node = platform.node()
uname = getpass.getuser()

# print("-DBUILDVAR_GIT_SHORTREV='\"%s\"'" % shortRev)
# print("-DBUILDVAR_GIT_FULLREV='\"%s\"'" % fullRev)
# print("-DBUILDVAR_GMT='\"%s\"'" % gmt)
# print("-DBUILDVAR_HOST='\"%s\"'" % node)
# print("-DBUILDVAR_USER='\"%s\"'" % uname)