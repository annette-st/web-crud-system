return await new Promise((resolve, reject) => {
    fetch("https://zoom.us/profile")
        .then(resp => resp.text())
        .then(data => {
            let doc = new DOMParser().parseFromString(data, 'text/html')
            let result = doc.evaluate('/html/body/div[1]/div[3]/div[3]/div[3]/div[1]/div/div[2]/div[2]/div[5]/div/div[1]/div/div[2]/span/span[1]/span[2]', doc, null, XPathResult.STRING_TYPE, null)
            resolve(result.stringValue)
        })
        .catch(e => {
            console.log(e)
        })
})

