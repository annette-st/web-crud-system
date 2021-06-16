import React, {useEffect, useState} from 'react'
import s from "../CreateTableConfiguration.module.css"
import {Link} from "react-router-dom"
import img from "../../../../assets/step2-animation.png"
import info from "../../../../assets/icn-info.svg"

const CreateConfigStep2 = (props) => {

    useEffect(() => {
        window.parent.postMessage({message: 'highlight_headers', content: ''}, '*')
        return () => {
            window.parent.postMessage({message: 'cancel_highlight_headers', content: ''}, '*')
        }
    }, [])

    const headers = props.headers

    const [isManual, setIsManual] = useState(!headers.length)

    return (
        <div className={s.generalStepContainer}>
            <p className={s.pageName}>Define table</p>
            <p className={s.stepNumber}>Step 2 of 3</p>
            {
                headers.length ? (
                    <div className={s.stepContainer}>
                        <p className={s.notes}>The table is defined automatically.</p>
                        <p className={s.notes}>Set up the table manually in case it is defined
                            incorrectly or proceed to the next step.</p>
                        <div className={s.configureManually}>
                            <Link to={{
                                pathname: '/manualSetting',
                                configName: props.name
                            }}>Configure manually</Link>
                        </div>
                        <img src={img} className={s.animation} alt="icn-animation"/>
                    </div>
                ) : (
                    <div className={s.stepContainer}>
                        <div className={s.manualConfigAlert}>
                            <img src={info} alt="icn-info"/>
                            <p className={s.notes}>Sorry, the table cannot be defined. Please configure manually.</p>
                            <div className={s.configureManually}>
                                <Link to={{
                                    pathname: '/manualSetting',
                                    configName: props.name
                                }}>Configure manually</Link>
                            </div>
                        </div>
                    </div>
                )
            }
            <div className={s.navButtons}>
                <button onClick={props.prev} className={s.prevButton}>Previous</button>
                <button onClick={props.next} className={s.nextButton} disabled={isManual}>Next</button>
            </div>
        </div>
    )
}

export default CreateConfigStep2
