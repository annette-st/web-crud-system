return await new Promise((resolve, reject) => {
    fetch("https://www.atlassian.com/gateway/api/me")
        .then(resp => resp.json())
        .then(html => {
            resolve(html['email'])
        })
        .catch(error => {
            console.log(error)
        })
})

