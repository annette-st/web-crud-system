import React, {useEffect, useState} from 'react'
import ConfigData from "./ConfigData/ConfigData"
import * as axios from "axios"
import LoadingUserConfig from "./LoadingUserConfig/LoadingUserConfig"

const ConfigDataContainer = () => {
    const DataLoading = LoadingUserConfig(ConfigData)
    const [appState, setAppState] = useState({
        loading: false,
        configs: null,
        url: null
    })
    const updateAppState = (state) => {
        // set loading...
        console.log("API:")
        console.log(appState.url)
        setAppState(state)
        let pr = new Promise((resolve, reject) => {
            window.addEventListener('message', (e) => {
                if (e.data.message === 'deleted_successfully') {
                    resolve(e.data)
                }
            })
        })
        pr.then(res => {
            console.log(appState.url)
            axios.post(appState.url, {
                operation: 'get',
                domain: res.domain
            }).then(resp => {
                const allConf = resp.data
                window.parent.postMessage({message: 'page_configuration', content: resp.data}, '*')
                console.log('allConf:')
                console.log(allConf)
                setAppState({
                    loading: false,
                    configs: allConf,
                    url: appState.url
                    // form_configs: allConf.form_configurations
                })
                console.log(allConf)
            })
        })
    }

    useEffect(() => {
        // TO RESET PAGE CONFIGURATION
        // window.parent.postMessage({message: 'help', content: ''},'*')
        setAppState({loading: true})

        window.parent.postMessage({message: 'get_url', content: ''}, '*')
        let pr = new Promise((resolve, reject) => {
            window.addEventListener('message', (e) => {
                if (e.data.message === 'parent_url') {
                    resolve(e.data)
                }
            })
        })
        pr.then((res) => {
            console.log(res)
            const apiUrl = res.content
            setAppState({loading: true})
            axios.post(apiUrl, {
                'operation': 'get',
                'domain': res.domain
            }).then((resp) => {
                const allConf = resp.data
                window.parent.postMessage({message: 'page_configuration', content: resp.data}, '*')
                console.log('allConf:')
                console.log(allConf)
                setAppState({
                    loading: false,
                    configs: allConf,
                    url: apiUrl
                    //form_configs: allConf.form_configurations
                })
                console.log(allConf)
            })
            console.log("API")
            console.log(apiUrl)
            console.log(appState.url)
        })
    }, [])
    return (
        <div>
            <DataLoading isLoading={appState.loading} updateAppState={updateAppState} config={appState.configs} />
        </div>
    )
}

export default ConfigDataContainer
