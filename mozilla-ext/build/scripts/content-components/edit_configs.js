export function getRootDomain(hostname) {
    const rootDomainRE = /([\w\d]*\.\w{2,4}$)/
    let rootDomain = hostname.match(rootDomainRE)
    return rootDomain ? rootDomain[1] : null
}

export async function deleteConfiguration(name, type) {
    let currentData = await getCurrentConfig()
    let reqConfig = {
        domain: getRootDomain(document.location.hostname),
        operation: 'put',
        // object that injects in corresponding pages
        payload: {
            domain: getRootDomain(document.location.hostname),
            table_configurations: [],
            form_configurations: []
        }
    }
    switch (type) {
        case 'table':
            for (let i = 0; i < currentData.table_configurations.length; i++) {
                if (currentData.table_configurations[i].name === name) {
                    currentData.table_configurations.splice(i, 1)
                    break
                }
            }
            break
        case 'form':
            for (let i = 0; i < currentData.form_configurations.length; i++) {
                if (currentData.form_configurations[i].name === name) {
                    currentData.form_configurations.splice(i, 1)
                    break
                }
            }
            break
    }
    reqConfig.payload.table_configurations = JSON.parse(JSON.stringify(currentData.table_configurations))
    reqConfig.payload.form_configurations = JSON.parse(JSON.stringify(currentData.form_configurations))

    if (!currentData.table_configurations.length && !currentData.form_configurations.length) {
        console.log('configuration is empty...')
        reqConfig = {
            error: 'not found'
        }
    }
    await fetch(document.location.protocol + '//' + document.location.hostname + '/3f785034-d25a-11ea-87d0-0242ac130003/store', {
        method: 'post',
        body: JSON.stringify(reqConfig)
    })
    console.log('deleted successfully. result:')
    console.log(reqConfig)
    document.getElementById('sidebarPlugin').contentWindow.postMessage({message: 'deleted_successfully', content: '', domain: getRootDomain(document.location.hostname)}, '*')
    console.log('done')
}

export async function getCurrentConfig() {
    return await new Promise((resolve, reject) => {
        fetch(document.location.protocol + '//' + document.location.hostname + '/3f785034-d25a-11ea-87d0-0242ac130003/store', {
            method: 'post',
            body: JSON.stringify({
                'operation': 'get',
                'domain': getRootDomain(document.location.hostname)
            })
        })
            .then((data) => resolve(data.json()))
    })
}
