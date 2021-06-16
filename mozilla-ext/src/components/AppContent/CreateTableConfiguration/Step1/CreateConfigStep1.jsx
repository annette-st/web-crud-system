import React, {useEffect, useState} from 'react'
import s from '../CreateTableConfiguration.module.css'
import {Link} from "react-router-dom";

const CreateConfigStep1 = (props) => {

    useEffect(() => {

        props.clearHeadersArray()
        getLocalHeaders()
        window.parent.postMessage({message: 'add_new_tables_listener', content: ''}, '*')
        window.addEventListener('message', newTablesListener)

        return () => {
            window.parent.postMessage({message: 'remove_new_tables_listener', content: ''}, '*')
            window.removeEventListener('message', newTablesListener)
        }
    }, [])

    const [isTableExists, setIsTableExists] = useState(false)


    const activeBtnCss = {
        fontSize: '14px',
        lineHeight: '20px',
        color: '#FFFFFF',
        background: '#3496F7',
        borderRadius: '3px',
        border: 'none',
        padding: '4px 16px',
        textDecoration: 'none'
    }
    const disabledBtnCss = {
        pointerEvents: 'none',
        fontSize: '14px',
        lineHeight: '20px',
        color: '#8E9BAE',
        background: '#DEE1E4',
        borderRadius: '3px',
        border: 'none',
        padding: '4px 16px',
        textDecoration: 'none'
    }

    const newTablesListener = (e) => {
        if (e.data.message === 'url_changed') {
            console.log('message received! url is changed')
            props.clearHeadersArray()
            getLocalHeaders()
        }
    }

    const [isNameFieldEmpty, setIsNameFieldEmpty] = useState(!props.name)

    const getLocalHeaders = () => {
        window.parent.postMessage({message: 'define_table', content: ''}, '*')
        let pr = new Promise((resolve, reject) => {
            window.addEventListener('message', (e) => {
                if (e.data.message === 'headers') {
                    if (e.data.content) {
                        resolve(e.data.content)
                        console.log('headers on this page:')
                        console.log(e.data.content)
                        setIsTableExists(true)
                    } else {
                        setIsTableExists(false)
                    }
                }
            })
        })
        pr.then((res) => {
            props.updateAvailableHeaders(res)
            console.log('updated!')
        })
    }

    let configName = React.createRef()

    let onNameChange = () => {
        let name = configName.current.value
        if (name) {
            setIsNameFieldEmpty(false)
        } else {
            setIsNameFieldEmpty(true)
        }
        props.updateConfigName(name)
    }

    return (
        <div className={s.generalStepContainer}>
            <p className={s.pageName}>Create table configuration</p>
            <p className={s.stepNumber}>Step 1 of 3</p>
            <div className={s.configNameForm}>
                <label>Config name</label>
                <input name="config_name" ref={configName} placeholder="Enter config name" onChange={onNameChange}
                       value={props.name}/>
            </div>

            <div className={s.configNameAction}>
                { isTableExists ? <button onClick={props.next} style={isNameFieldEmpty ? disabledBtnCss : activeBtnCss} disabled={isNameFieldEmpty}>Next
                </button> :
                    <Link to={{
                        pathname: '/manualSetting',
                        configName: props.name
                    }}
                          style={isNameFieldEmpty ? disabledBtnCss : activeBtnCss}
                    >Next</Link>
                }
            </div>
        </div>
    )
}

export default CreateConfigStep1
