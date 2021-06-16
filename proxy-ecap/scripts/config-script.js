function __name_space__(HOSTIP, ORIGIN, SERVER_ADDR, WORKSPACE) {

    let audit_config = {{AUDIT_CONFIG}};
    async function getUserLogin() {
        {{LOGIN_SCRIPT}}
    }
    if (window.top !== window) return ;

    console && console.log('BINADOX')

    function guid() {
        function s4() {
            return Math.floor((1 + Math.random()) * 0x10000).toString(16).substring(1);
        }
        return s4() + s4() + '-' + s4() + '-' + s4() + '-' + s4() + '-' + s4() + s4() + s4();
    }

    const TAB_ID = guid();
    const DEFAULT_TIMEOUT = 15000;
    const PING_TIMEOUT = 1 * 60 * 1000; // 1 Minute
    let isActive = true;

    class PageInfo
    {
        constructor() {
            this.hostname = "";
            this.rootDomain = "";
            this.url = "";
            this.nAgt = navigator.userAgent;
            this.tabId = null;
            this.iconUrl = null;
            this.macId = null;
        }
    }

    class HttpWorker {
        constructor() {
            this.urlObj = {
                hostname: "",
                rootDomain: "",
                url: ""
            };

            this.defaultPort = 40510;
            this.port = this.defaultPort;
            this.macId = null;
            this.icon_map = new Map();
            this.last_active_windowId = 0;
            this.excludedZones = null;
            this.pingTimeout = PING_TIMEOUT;

            this.httpQuery = [];
            this.isQueryInWork = false;
        }

        get userAgent() {
            return navigator.userAgent;
        }

        doHttp(data, addr = SERVER_ADDR, method = "POST", timeout = DEFAULT_TIMEOUT)
        {
            return new Promise((resolve, reject) => {
                let xhr = new XMLHttpRequest();
                xhr.open(method, addr, true);
                if (method == "POST")
                    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                //xhr.setRequestHeader("Access-Control-Allow-Origin", "*");
                if (isNaN(timeout))
                    timeout = DEFAULT_TIMEOUT;
                xhr.timeout = timeout;

                xhr.send(data);

                xhr.addEventListener("timeout", function() {
                    return reject("Timeout error");
                }, false)

                xhr.addEventListener("error", function(e) {
                    return reject("Can't send data to " + addr + " ::: " + e);
                }, false);

                xhr.addEventListener("load", function() {
                    return resolve(this.responseText.toString());
                }, false);
            });
        }

        doSyncHttp(data, addr = SERVER_ADDR, method = "POST")
        {
            let xhr = new XMLHttpRequest();
            xhr.open(method, addr, false)
            if (method == "POST")
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            xhr.send(data);
        }

        addInHttpQuery(data)
        {
            this.httpQuery.push(data);
            if(!this.isQueryInWork)
                this.doQueryHttps();
        }

        doQueryHttps()
        {
            if (!isActive) {
                console.log('Not active');
                return
            }
            this.isQueryInWork = true;
            return this.doHttp(this.httpQuery[0])
            .then(data => {
                this.httpQuery.shift();
                if(this.httpQuery.length > 0)
                    return this.doQueryHttps();
                else
                    this.isQueryInWork = false;
            }, err => {
                console.log("Error: ", err);
                this.isQueryInWork = false;
            })
            .catch(err =>{
                console.log("Error: ", err);
                this.isQueryInWork = false;
            });
        }
    }

    function getRootDomain(hostname)
    {
        const rootDomainRE = /([\w\d]*\.\w{2,4}$)/;
        let rootDomain = hostname.match(rootDomainRE);
        return rootDomain ? rootDomain[1] : null;
    }

    function pingLoop()
    {
        let pingObject = {
            message_type: "ping",
            addr: HOSTIP,
            tabId: TAB_ID,
            GUID: WORKSPACE,
            orig: btoa(ORIGIN)
        };
        //console.log('pingLoop', pingObject)
        pingObject.url = httpWorker.urlObj.url == decodeURI(httpWorker.urlObj.url) ?
                btoa(encodeURI(httpWorker.urlObj.url)) :
                btoa(httpWorker.urlObj.url);
        return httpWorker.addInHttpQuery(JSON.stringify(pingObject));
    }

    function TestLocalWebServer()
    {
	    httpWorker.port = httpWorker.defaultPort;
        return new Promise(resolve => {
            return resolve();
        });
    }

    function setCurrentHostName()
    {
        return new Promise(resolve => {
            const urlObj = window.location
            httpWorker.urlObj.url = urlObj.href
            httpWorker.urlObj.rootDomain = getRootDomain(urlObj.hostname)
            httpWorker.urlObj.hostname = urlObj.hostname.replace('www.', '')
            return resolve(httpWorker.urlObj.hostname)
        });
    }

    let httpWorker = new HttpWorker()

    function addListener(element, eventName, handler)
    {
        if (element.addEventListener)
            element.addEventListener(eventName, handler, false)
        else if (element.attachEvent)
            element.attachEvent('on' + eventName, handler)
        else
            element['on' + eventName] = handler
    }

    function tabClosed() {
        let tabClosed = {
            message_type: "tab_closed",
            addr: HOSTIP,
            tabId: TAB_ID,
            GUID: WORKSPACE,
	        orig: btoa(ORIGIN)
        }
        // Make it synchronously
        httpWorker.doSyncHttp(JSON.stringify(tabClosed), SERVER_ADDR);
    }

    let pageInfo = new PageInfo();

    if (typeof document !== 'undefined' && typeof window !== 'undefined')
    {
        addListener(window, 'beforeunload', tabClosed);
    }
    pageInfo.tabId = TAB_ID;
    pageInfo.macId = HOSTIP;

    function main() {
        if (!document || !document.body) {
            return setTimeout(main, 250);
        }
        pageInfo.url = document.location.href;
        pageInfo.hostname = document.location.hostname.replace('www.', '');
        pageInfo.rootDomain = getRootDomain(pageInfo.hostname);
        setCurrentHostName()
            .then(_ => {
                return TestLocalWebServer()
            }).then(() => {
                setInterval(pingLoop, httpWorker.pingTimeout);
            }).catch(err => {
                console.log(err);
                console.log("Connection failed")
            });
    }

    main();
}
