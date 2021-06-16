import React, {useEffect, useState} from 'react'
import s from '../../../CreateTableConfiguration.module.css'
import {NavLink} from "react-router-dom";
import img from "../../../../../../assets/select-for-2a.gif";

const Step2A = (props) => {

    const [isHeadersReceived, setIsHeadersReceived] = useState(false)

    useEffect(() => {
        window.parent.postMessage({message: 'define_headers_manually', content: ''}, '*')
        if (!props.headers.length) {
            getHeaders()
        }
    }, [])

    let stopHeadersListener = () => {
        window.parent.postMessage({message: 'stop_headers_listener', content: ''}, '*')
    }

    const getHeaders = () => {
        let pr = new Promise((resolve, reject) => {
            window.addEventListener('message', (e) => {
                if ((e.data.message === 'manual_headers') && (e.data.content)) {
                    resolve(e.data.content)
                    console.log(e.data.content)
                    setIsHeadersReceived(true)
                }
            })
        })
        pr.then((res) => {
            props.updateManualHeaders(res)
        })
    }

    return (
        <div className={s.generalStepContainer}>
            <p className={s.stepNumber}>Step 2 of 3</p>
            <div className={s.stepContainer}>
                <p className={s.notesActive}>А. Select a table header</p>
                <p className={s.notes}>B. Select a table cell </p>
            </div>
            <img src={img} className={s.animation} alt="icn-animation"/>
            <div className={s.navButtons}>
                <div className={s.prevButton}><NavLink to='/startTableConfig' onClick={stopHeadersListener}>Previous</NavLink></div>
                <button onClick={ props.next } className={s.nextButton} disabled={!isHeadersReceived}>Next</button>
            </div>
        </div>
    )
}

export default Step2A
