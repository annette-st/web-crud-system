import React, {useEffect, useState} from 'react'
import s from '../CreateTableConfiguration.module.css'
import arrowBack from "../../../../assets/icn-back-blue.svg"
import {NavLink} from "react-router-dom"
import icnEdit from '../../../../assets/icn-edit.svg'
import Tabs from "./ManageTable/Tabs"
import FormControlLabel from "@material-ui/core/FormControlLabel"
import RetrieveSwitch from "./RetrieveSwitch"
import { useLocation } from 'react-router'
import editCancel from '../../../../assets/icn-edit-cancel.svg'
import icnLoading from '../../../../assets/icn-btn-loading.svg'

const ConfigurationSettings = () => {

    const location = useLocation()

    useEffect(() => {
        window.addEventListener('message', getUpdatedConfig, false)
        window.parent.postMessage({message: 'current_config_name', content: location.configName}, '*')

        return () => {
            window.removeEventListener('message', getUpdatedConfig, false)
        }
    }, [])

    let getUpdatedConfig = (e) => {
        if (e.data.message === 'update_page_config') {
            setConfigData(e.data.content)
        }
    }

    const [configData, setConfigData] = useState(location.configData ? location.configData : [])
    const [configName, setConfigName] = useState(location.configName)
    const [isRetrieveOn, setIsRetrieveOn] = useState(location.isRetrieve)
    const [isEditModeOn, setIsEditModeOn] = useState(false)
    const [currentTab, setCurrentTab] = useState(0)
    const [isSavingInProcess, setIsSavingInProcess] = useState(false)
    const [isEditFieldEmpty, setIsEditFieldEmpty] = useState(true)

    let configNameUpdated = React.createRef()

    const handleRetrieveChange = () => {
        setIsRetrieveOn(!isRetrieveOn)
        window.parent.postMessage({message: 'toggle_retrieve', content: isRetrieveOn}, '*')
    }

    const updateCurrentTab = (state) => {
        setCurrentTab(state)
    }

    const clearHeadersArray = () => {
        window.parent.postMessage({message: 'clear_headers', content: ''}, '*')
    }

    const handleEditAction = () => {
        if (isEditModeOn) {
            setIsEditModeOn(false)
            setConfigName(configNameUpdated.current.value)
            sendUpdatedName(configNameUpdated.current.value)
        } else {
            setIsEditModeOn(true)
        }
    }
    let sendUpdatedName = (name) => {
        window.parent.postMessage({message: 'config_table_name', content: name},'*')
    }

    let updateConfigData = (type, name) => {
        console.log(`type: ${type}`)
        console.log(`name: ${name}`)
    }
    let switchEditMode = () => {
        setIsEditModeOn(false)
    }

    let onNameChange = () => {
        let name = configNameUpdated.current.value
        if (name) {
            setIsEditFieldEmpty(false)
        } else {
            setIsEditFieldEmpty(true)
        }
    }

    const saveConfiguration = () => {
        setIsSavingInProcess(true)
        window.parent.postMessage({message: 'save_configuration', content: 'table'}, '*')

        let promise = new Promise((resolve, reject) => {
            window.addEventListener('message', (e) => {
                if (e.data.message === 'saved_successfully') {
                    resolve()
                }
            })
        })

        promise.then(() => {
            let btn = document.getElementById('backToConfig')
            btn.click()
            setIsSavingInProcess(false)
        })
    }

    return (
        <div className={s.settingsContainer}>
            <NavLink to='/contentFrame' onClick={clearHeadersArray}>
                <div className={s.backToConfig} id="backToConfig">
                    <img src={arrowBack} alt="icn-arrow-back"/>
                    Back to Сonfig list
                </div>
            </NavLink>

            <p className={s.pageName}>Configuration Settings</p>
            <p className={s.configName}>"{ configName }"</p>

            {isEditModeOn ? (<div className={s.editingContainer}>
                <button className={s.cancelEditBtn} onClick={switchEditMode}>
                    <img src={editCancel} alt="icn-edit-cancel"/>
                </button>
                <input type="text" ref={configNameUpdated} onChange={onNameChange} placeholder="Enter new config name"/>
                <button className={s.saveConfigName} onClick={handleEditAction} disabled={isEditFieldEmpty}>Save</button>
            </div>) : (<div className={s.editingContainer}>
                <button className={s.editConfigName} onClick={handleEditAction}>
                    <img src={icnEdit} alt="icn-edit"/>
                    Edit name
                </button>
            </div>)}

            <div className={s.manageTableContainer}>
                <p className={s.actionName}>Manage table</p>
                <div className={s.tabsWrapper}>
                    <Tabs items={configData} name={configName} updateCurrentTab={updateCurrentTab} updateConfigData={updateConfigData} isRetrieve={isRetrieveOn} />
                </div>
            </div>
            <div className={s.tableViewsSwitcher}>
                <FormControlLabel
                    control={<RetrieveSwitch checked={isRetrieveOn} onChange={handleRetrieveChange} name="checkedA" />}
                    label="Inform about table views"
                />
            </div>

            <button className={s.saveConfigButton} onClick={saveConfiguration}>
                { isSavingInProcess ? <img className="icnLoading" src={icnLoading} alt="icn-btn-loading"/> : <span>Save Configuration</span> }
            </button>
        </div>
    )
}

export default ConfigurationSettings
