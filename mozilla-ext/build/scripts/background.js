localStorage.setItem('isSidebarInjected', 'false')

browser.tabs.onUpdated.addListener((tabId) => {
    browser.pageAction.show(tabId)
})
browser.pageAction.onClicked.addListener((tab) => {
    console.log(localStorage.getItem('isSidebarInjected'))
    browser.tabs.query({active: true, currentWindow: true}).then((tabs) => {
        if (localStorage.getItem('isSidebarInjected') === 'true') {
            localStorage.setItem('isSidebarInjected', 'false')
            browser.tabs.sendMessage(tab.id, 'close_sidebar')
        } else {
            localStorage.setItem('isSidebarInjected', 'true')
            browser.tabs.sendMessage(tab.id, 'open_sidebar')
        }
    })
})

