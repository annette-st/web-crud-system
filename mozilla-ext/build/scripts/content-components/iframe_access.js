export function waitForElement(querySelector, timeout = 0) {
    const startTime = new Date().getTime();
    return new Promise((resolve, reject) => {
        const timer = setInterval(() => {
            const now = new Date().getTime()
            if (document.querySelector(querySelector)) {
                clearInterval(timer)
                resolve()
            } else if (timeout && now - startTime >= timeout) {
                clearInterval(timer)
                reject()
            }
        }, 100)
    })
}

// waitForElement("iframe", 10000).then(function(){
//     alert("iframe is loaded!")
//     console.log(document.querySelector("iframe"))
//     let elem = document.querySelector("iframe")
//
//     let observer = new MutationObserver(function(mutations) {
//         mutations.forEach(function(mutation) {
//             if (mutation.type == "attributes") {
//                 console.log(mutation)
//                 console.log("attributes changed")
//             }
//         })
//     })
//
//     observer.observe(elem, {
//         attributes: true //configure it to listen to attribute changes
//     });
//
//
//     // console.log("src", document.querySelector("iframe").src)
// }).catch(() => {
//     console.log("element did not load in 10 seconds");
// })
