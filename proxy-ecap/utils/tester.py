import shlex;
import sys;
import argparse;
from subprocess import Popen, PIPE;


if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")


tests = {
    "login.salesforce.com" : "'https://login.salesforce.com/' -H 'Referer: https://um5.salesforce.com/' -H 'DNT: 1' -H 'Cookie: BrowserId=1R1BbWdqEeq2UB0TcnT0hw; webact=%7B%22l_vdays%22%3A143%2C%22l_visit%22%3A1595405665088%2C%22session%22%3A1598256830026%2C%22l_search%22%3A%22%22%2C%22l_dtype%22%3A%22%22%2C%22l_page%22%3A%22SFDC%3Aus%3Alogin%22%2C%22counter%22%3A5%2C%22pv%22%3A6%2C%22f_visit%22%3A1584351914542%2C%22seg%22%3A%22customer%3Aus%22%2C%22customer%22%3A1584352179287%2C%22developer%22%3A1584529909758%2C%22trial%22%3A1585929783768%7D; sfdc_lv2=KxG83HbCPj4cGz2hY8Cc5L7FfMJNkEZ1Zf+l3ivFmazNHmLzs2L/qJ/DrLFyCoTOzNmhi42NrPzpQgCQmrpiIG14DRFp3VRCFAfTmbAvCBQUe05V+6AyI6Fg7b4HfuiwZfEdpKCujRww==; AMCV_8D6C67C25245AF020A490D4C%40AdobeOrg=-1891778711%7CMCIDTS%7C18499%7CMCMID%7C09389251098458716495163408542107579776%7CMCAID%7CNONE%7CMCOPTOUT-1598263212s%7CNONE%7CvVersion%7C2.4.0; s_ecid=MCMID%7C09389251098458716495163408542107579776; rememberUn=false; session=1598256011726; AMCVS_8D6C67C25245AF020A490D4C%40AdobeOrg=1; com.salesforce.LocaleInfo=us; oinfo=c3RhdHVzPUFDVElWRSZ0eXBlPTgmb2lkPTAwRDRKMDAwMDAwcERJOA==; autocomplete=1; 79eb100099b9a8bf=3:false:.salesforce.com; QCQQ=pRuSPtTt6Pi'  -H 'Cache-Control: max-age=0'",
    # ---------------------------------------------------------------------
    "example.com" : "'http://example.com/'",
    # --------------------------------------------------------------------
    "kernel.org" : "'https://www.kernel.org/'",
    # --------------------------------------------------------------------
    "vesty.co.il" : "'https://www.vesty.co.il/main'",
    # --------------------------------------------------------------------
    "mignews.co.il" : "'https://mignews.com/'"
    };

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_test_name(name, max_len):
    print(name, end = "");
    n = max_len - len(name)
    dots = " ";
    for i in range(0, n + 5):
        dots = dots + ".";
    print(dots + " ", end = "");
    
def run(proxy_addr):
    print ("Testing proxy " + proxy_addr);
    failed_tests = 0;
    
    max_test_len = len(max(tests.keys(), key=len))

    COMMAND_PREFIX = "curl --max-time 10 --fail --silent --show-error -k --proxy http://" + proxy_addr ;
    COMMAND_SUFFIX = "-H 'User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:79.0) Gecko/20100101 Firefox/79.0' -H 'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8' -H 'Accept-Language: en-US,en;q=0.5' --compressed -H 'Connection: keep-alive' -H 'Upgrade-Insecure-Requests: 1'";
    print("----------------------------------------------------");
    for test_name in tests.keys():
        shellcmd = COMMAND_PREFIX + " " + tests[test_name] + " " + COMMAND_SUFFIX;
        
        cmd = shlex.split(shellcmd);
        
        print_test_name(test_name, max_test_len);
        
        process = Popen(cmd, stdout=PIPE, stderr=PIPE)
        (output, err) = process.communicate()
        exit_code = process.wait()
        if exit_code != 0:
            failed_tests = failed_tests + 1;
            print (bcolors.FAIL + bcolors.BOLD + "[X]" + bcolors.ENDC);
            print ("\t" + bcolors.WARNING + err.decode('utf-8') + bcolors.ENDC);
        else:
            print(bcolors.OKGREEN + "[V]" + bcolors.ENDC);
    print("----------------------------------------------------");        
    if failed_tests == 0:
        print(bcolors.OKGREEN + bcolors.BOLD + "All passed" + bcolors.ENDC);
    else:
        print(bcolors.FAIL + bcolors.BOLD + "Failed " + str(failed_tests) + " / " + str(len(tests.keys())) + " tests" + bcolors.ENDC);

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--proxy", help="proxy")
    args = parser.parse_args()

    default_proxy = "127.0.0.1:3128"
    if args.proxy:
        default_proxy = args.proxy;
        
    run(default_proxy);
        
if __name__ == "__main__":
    main()


