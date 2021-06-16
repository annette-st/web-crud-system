import React, {useEffect, useState} from 'react'
import s from '../../../CreateTableConfiguration.module.css'
import img from "../../../../../../assets/select-for-2b.gif"
import icnDone from '../../../../../../assets/icn-done.svg'

const Step2B = (props) => {

    const [isRowsDone, setIsRowsDone] = useState(false)

    useEffect(() => {
        window.parent.postMessage({message: 'define_rows_manually', content: ''}, '*')
        waitRowsDone()
    }, [])

    const waitRowsDone = () => {
        let pr = new Promise((resolve, reject) => {
            window.addEventListener('message', (e) => {
                if ((e.data.message === 'rows_done')) {
                    setIsRowsDone(true)
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
                <p className={s.notes}>А. Select a table header <img src={icnDone} alt="icn-done"/></p>
                <p className={s.notesActive}>B. Select a table cell </p>
            </div>
            <img src={img} className={s.animation} alt="icn-animation"/>
            <div className={s.navButtons}>
                <button onClick={ props.prev } className={s.prevButton}>Previous</button>
                <button onClick={props.next} className={s.nextButton} disabled={!isRowsDone}>Next</button>
            </div>
        </div>
    )
}

export default Step2B
