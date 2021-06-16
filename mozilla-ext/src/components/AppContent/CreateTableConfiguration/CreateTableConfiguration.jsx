import React, {useState} from 'react'
import { Steps, Step } from 'react-step-builder'
import CreateConfigStep1 from "./Step1/CreateConfigStep1"
import CreateConfigStep3 from "./Step3/CreateConfigStep3"
import CreateConfigStep4 from "./Step4/CreateConfigStep4"
import s from "./CreateTableConfiguration.module.css"
import arrowBack from "../../../assets/icn-back-blue.svg"
import {NavLink} from "react-router-dom"

const CreateTableConfiguration = (props) => {
    const [configName, setConfigName] = useState('')
    const [availableHeaders, setAvailableHeaders] = useState([])
    const updateConfigName = (state) => {
        setConfigName(state)
        sendUpdatedName(state)
    }
    const updateAvailableHeaders = (state) => {
        const modified = JSON.parse(state).map((item, index) => {
            return {
                id: index,
                name: item,
                isClicked: false,
                index: false
            }
        })
        setAvailableHeaders(modified)
    }
    const updateCheckboxStatus = (id) => {
        let modified = [...availableHeaders]
        for (let i = 0; i < modified.length; i++) {
            if (+modified[i].id === +id) {
                modified[i].isClicked = !modified[i].isClicked
            }
        }
        setAvailableHeaders(modified)
    }
    const resetIndexing = () => {
        for (let item of availableHeaders) {
            item.index = false
        }
    }
    const clearHeadersArray = () => {
        window.parent.postMessage({message: 'clear_headers', content: ''}, '*')
        setAvailableHeaders([])
        console.log('headers cleaned:')
        console.log(availableHeaders)
    }

    return (
        <div>
            <div className={s.backToConfigContainer}>
                <NavLink to='/contentFrame' onClick={clearHeadersArray}>
                    <div className={s.backToConfig}>
                        <img src={arrowBack} alt="icn-arrow-back"/>
                        Back to Сonfig list
                    </div>
                </NavLink>
            </div>
            <Steps>
                <Step component={CreateConfigStep1} name={configName} updateConfigName={updateConfigName}
                      headers={availableHeaders} updateAvailableHeaders={updateAvailableHeaders} clearHeadersArray={clearHeadersArray}/>
                {/*<Step component={CreateConfigStep2} name={configName} headers={availableHeaders}/>*/}
                <Step component={CreateConfigStep3} headers={availableHeaders}
                      updateCheckboxStatus={updateCheckboxStatus}/>
                <Step component={CreateConfigStep4} name={configName} headers={availableHeaders}
                      resetIndexing={resetIndexing}/>
            </Steps>
        </div>
    )
}

function sendUpdatedName(name) {
    window.parent.postMessage({message: 'current_config_name', content: name},'*')
    window.parent.postMessage({message: 'config_table_name', content: name},'*')
}

export default CreateTableConfiguration
