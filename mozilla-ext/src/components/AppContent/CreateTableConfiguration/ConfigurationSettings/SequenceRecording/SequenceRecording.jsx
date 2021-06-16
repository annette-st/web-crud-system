import React, {useEffect, useState} from 'react'
import s from "../../CreateTableConfiguration.module.css";
import arrowBack from "../../../../../assets/icn-back-blue.svg";
import {Link} from "react-router-dom";
import icnEdit from "../../../../../assets/icn-edit.svg";
import arrowDown from '../../../../../assets/icn-arrow-down.svg'
import arrowUp from '../../../../../assets/icn-arrow-up.svg'
import RecordingBlock from './RecordingBlock'
import { useLocation } from 'react-router'
import editCancel from '../../../../../assets/icn-edit-cancel.svg'
import ReactHtmlParser from 'react-html-parser'

const SequenceRecording = () => {

    const location = useLocation()

    useEffect(() => {
        console.log('sequence data:')
        console.log(location.sequence)
        console.log('configData')
        console.log(location.configData)

        // add new sequence...
        if (!location.sequence.name) {
            console.log('sequence is NEW')
            updateSequenceName('New ' + operationType[0].toUpperCase() + operationType.slice(1) + ' sequence')
        } else {
            window.parent.postMessage({message: 'edit_sequence', content: {operation: operationType, name: location.sequence.name}}, '*')
        }
    }, [])

    let sequenceNameUpdated = React.createRef()

    const [configData, setConfigData] = useState(location.configData)
    const [operationType, setOperationType] = useState(location.sequence.type)
    const [sequenceName, setSequenceName] = useState(location.sequence.name || `New ${operationType[0].toUpperCase() + operationType.slice(1)} sequence`)
    const [isPromptOpen, setIsPromptOpen] = useState(true)
    const [isEditModeOn, setIsEditModeOn] = useState(false)
    const [isRerecordOn, setIsRerecordOn] = useState(!!location.sequence.data.steps.length)
    const [promptValue, setPromptValue] = useState('<p>Press Start recording to monitor changes</p>')
    const [isEditFieldEmpty, setIsEditFieldEmpty] = useState(true)


    const updatePromptState = (value) => {
        setPromptValue(value)
    }

    const handlePrompt = () => {
        if (isPromptOpen) {
            setIsPromptOpen(false)
        } else {
            setIsPromptOpen(true)
        }
    }

    const handleEditAction = () => {
        if (isEditModeOn) {
            setIsEditModeOn(false)
            setSequenceName(sequenceNameUpdated.current.value)
            updateSequenceName(sequenceNameUpdated.current.value)
        } else {
            setIsEditModeOn(true)
        }
    }

    const updateSequenceName = (name) => {
        window.parent.postMessage({message: 'update_seq_name', content: name}, '*')
    }

    let switchEditMode = () => {
        setIsEditModeOn(false)
    }

    const saveSequence = () => {
        window.parent.postMessage({message: 'save_sequence', content: operationType}, '*')
    }

    let onNameChange = () => {
        let name = sequenceNameUpdated.current.value
        if (name) {
            setIsEditFieldEmpty(false)
        } else {
            setIsEditFieldEmpty(true)
        }
    }

    return (
        <div className={s.settingsContainer}>
            <Link to={{
                pathname: '/tableSettings',
                configName: location.configName,
                configData: configData,
                isRetrieve: location.isRetrieve
            }} onClick={ saveSequence }>
                <div className={s.backToConfig}>
                    <img src={arrowBack} alt="icn-arrow-back"/>
                    Back to Configuration Settings
                </div>
            </Link>

            <p className={s.pageName}>Sequence "{operationType[0].toUpperCase() + operationType.slice(1)}" Recording</p>

            {isEditModeOn ? (<div className={s.editingContainer}>
                <button className={s.cancelEditBtn} onClick={switchEditMode}>
                    <img src={editCancel} alt="icn-edit-cancel"/>
                </button>
                <input type="text" ref={sequenceNameUpdated} onChange={onNameChange} placeholder="Enter new sequence name"/>
                <button className={s.saveConfigName} onClick={handleEditAction} disabled={isEditFieldEmpty}>Save</button>
            </div>) : (<div className={s.editingContainer}>
                <p>{sequenceName}</p>
                <button className={s.editConfigName} onClick={handleEditAction}>
                    <img src={icnEdit} alt="icn-edit"/>
                    Edit name
                </button>
            </div>)}

            <div className={s.seqPrompt}>
                <button className={s.seqPromptHeader} onClick={handlePrompt}>
                    {isPromptOpen ? (<img src={arrowUp} alt="icn-arrow"/>) :
                        (<img src={arrowDown} alt="icn-arrow"/>)
                    }
                    <p>Prompt</p>
                </button>
                {isPromptOpen ? (
                    <div className={s.promptContent}>
                        <div>{ ReactHtmlParser (promptValue) }</div>
                    </div>
                ) : null }
            </div>


            <div className={s.recordSequenceContainer}>
                <RecordingBlock isRerecord={isRerecordOn} operationType={operationType} seqData={location.sequence} updatePromptState={updatePromptState} />
            </div>
        </div>
    )
}

export default SequenceRecording
