import React, {useEffect, useState} from 'react'
import s from '../../CreateTableConfiguration.module.css'
import Rollback from "./Rollback"

const RecordingBlock = (props) => {

    const [isRecInProcess, setIsRecInProcess] = useState(false)
    const [isActionRecording, setIsActionRecording] = useState(false)
    const [isRollbackRecording, setIsRollbackRecording] = useState(false)
    // check for existing or new
    const [actionArray, setActionArray] = useState(props.isRerecord ? props.seqData.data.steps : [])
    const [startRecValue, setStartRecValue] = useState(props.isRerecord ? "Rerecord" : "Start recording")
    const [isStart, setIsStart] = useState(!props.isRerecord)

    useEffect(() => {
        window.addEventListener('message', rb_request, false)

        let pr = new Promise((resolve, reject) => {
            window.addEventListener('message', (e) => {
                if (e.data.message === 'click_catch') {
                    // cur_seq.steps array
                    resolve(e.data.content)
                }
            })
        })
        pr.then(res => {
            console.log('UPDATED actionArray:')
            console.log(JSON.parse(res))
            setActionArray(JSON.parse(res))
            setIsRollbackRecording(false)
            setIsActionRecording(true)
            props.updatePromptState(`<p>Define steps for ${props.operationType} (it will be highlighted).</p> <p>To add repealing action click <strong>Start Recording Rollback.</strong></p>`)
        })
        return () => {
            window.removeEventListener('message', rb_request, false)
        }
    }, [actionArray, isActionRecording, startRecValue])

    let rb_request = (e) => {
        if (e.data.message === 'rb_request') {
            let actions = document.getElementsByClassName("action-item")
            for (let a of actions) {
                a.classList.add('hover')
                a.addEventListener('click', catchRb, false)
            }
        }
    }

    let catchRb = (e) => {
        let str = e.currentTarget.textContent
        console.log(`you have just clicked on ${str.charAt(str.length - 1)}`)
        window.parent.postMessage({message: 'rb_required_state', content: str.charAt(str.length - 1)}, '*')
        // remove listeners from divs
        let actions = document.getElementsByClassName("action-item")
        for (let a of actions) {
            a.classList.remove('hover')
            a.removeEventListener('click', catchRb, false)
        }
    }

    const handleStartClick = () => {
        props.updatePromptState(`<p>Define steps for ${props.operationType} (it will be highlighted).</p> <p>To add repealing action click.</p> <p>Start Recording Rollback.</p>`)
        setIsRecInProcess(true)
        setIsActionRecording(true)

        // check for rr/start
        if (startRecValue === 'Start recording') {
            window.parent.postMessage({message: 'start_recording', content: 'action'}, '*')
            setIsStart(false)
        } else {
            setActionArray([])
            window.parent.postMessage({message: 'start_rerec', content: 'action'}, '*')
        }
    }

    const handleStopButton = () => {
        setIsRecInProcess(false)
        setIsActionRecording(false)
        setIsRollbackRecording(false)
        window.parent.postMessage({message: 'stop_recording', content: ''}, '*')
        setStartRecValue('Rerecord')
    }

    const sendRequestToContent = (e) => {
        setIsActionRecording(true)
        window.parent.postMessage({message: 'recording', content: e.target.id.toString()}, '*')
    }

    const createRollback = () => {
        window.parent.postMessage({message: 'create_rollback', content: ''}, '*')
        setIsActionRecording(false)
        setIsRollbackRecording(true)
        props.updatePromptState(`<p>Specify to which step you need to rollback your action sequence from the recording list.</p> <p>To do so click on the required Action.</p> `)
    }

    return (
        <div className={s.mainSequenceContainer}>
            <p className={s.actionName}>Action sequence</p>

            {isRecInProcess ? (
                <button className={s.stopRecButton} onClick={handleStopButton}>Stop recording</button>
            ) : (
                <button className={s.startRecButton} onClick={handleStartClick}>{startRecValue}</button>
            )}

            {!isStart && <div className={s.recordingActionBlock}>
                <div className={s.recordingActionBlockItems}>
                    <div className="action-item">
                        <p className={s.actionItem}>Page opened</p>
                    </div>
                    <div className={s.mainActionsContainer}>
                        {actionArray.map((item, i) => {
                            if (actionArray.length === i + 1) {
                                return <div>
                                    <div className="action-item">
                                        <p className={s.activeActionHeader}>&#8226; Action {item.n}</p>
                                    </div>
                                    { item.rb.length ? <Rollback rbs={item.rb} /> : null }
                                </div>
                            } else {
                                return <div>
                                    <div className="action-item">
                                        <p className={s.actionHeader}>Action {item.n}</p>
                                    </div>
                                    {item.rb.length ? <Rollback rbs={item.rb}/> : null}
                                </div>
                            }
                        })}
                    </div>

                    {isActionRecording && <div className={s.actionIsRecording}>
                        <p>Recording Action...</p>
                    </div>
                    }
                    {isRollbackRecording && <div className={s.rollbackIsRecording}>
                        <p>Recording Rollback...</p>
                    </div>
                    }
                </div>

                {isRecInProcess && <div className={s.recordingActions}>
                    <button id='rollback' onClick={createRollback}>Add Rollback</button>
                </div>}
            </div>}

        </div>
    )
}

export default RecordingBlock
