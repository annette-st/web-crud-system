let uid = await new Promise((resolve, reject) => {
    fetch('https://app.box.com/app-api/end-user-web/collections')
        .then((data) => data.json())
        .then((json) => {
            resolve(json.collections[0].user.id)
        })
})
return await new Promise((resolve, reject) => {
    fetch('https://app.box.com/app-api/enduserapp/profile/' + uid)
        .then((data) => data.json())
        .then((json) => {
            resolve(json.email)
        })
        .catch(error => {
            console.log(error)
        })
})

