import React from "react"
import s from '../../CreateTableConfiguration.module.css'
import {Link} from "react-router-dom"
import del from '../../../../../assets/icn-delete.svg'

const TabContent = (props) => {
    let deleteSequence = (e) => {
        let btn = e.currentTarget
        window.parent.postMessage({
            message: 'delete_sequence',
            content: {type: props.current.title, name: btn.parentElement.textContent}
        }, '*')
        props.updateConfigData(props.current.title, btn.parentElement.textContent) // (type, name)
    }
    return (
        <div className={s.tabsContent}>
            { props.current.content.length ? (
                props.current.content
                    .sort((a, b) => a.name > b.name ? 1 : -1)
                    .map((seq) => {
                    return <div className={s.seqItem}>
                        <Link to={{
                            pathname: '/sequenceRecording',
                            sequence: {
                                name: seq.name,
                                data: { steps: seq.steps, rollback: seq.rollback },
                                type: props.current.title
                            },
                            configName: props.name,
                            configData: props.items,
                            isRetrieve: props.isRetrieve
                        }}>{seq.name || 'unnamed'}</Link>
                        <button className={s.deleteConfigBtn} onClick={deleteSequence}>
                            <img src={ del } alt="icn-delete"/>
                        </button>
                    </div>
                })
            ) : (
                <p className={s.noSeqAlert}>No sequence added yet </p>
            )}

            <Link to={{
                pathname: '/sequenceRecording',
                sequence: {
                    type: props.current.title,
                    name: '',
                    data: {steps: []}
                },
                configName: props.name,
                configData: props.items,
                isRetrieve: props.isRetrieve
            }}
                  className={s.addSeqButton}
            >Add Sequence</Link>
        </div>
    )
}


export default TabContent
