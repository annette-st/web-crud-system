import React, {useState} from 'react'
import { Steps, Step } from 'react-step-builder'
import s from '../../CreateTableConfiguration.module.css'
import Step2A from "./Steps/Step2A"
import Step2B from "./Steps/Step2B"
import Step2Final from "./Steps/Step2Final"
import img from "../../../../../assets/step2-animation.png";
import arrowBack from "../../../../../assets/icn-back-blue.svg";
import {NavLink} from "react-router-dom";
import Step3 from "./Steps/Step3";
import Step4 from "./Steps/Step4";
import {useLocation} from "react-router";

const ManualSetting = (props) => {

    const location = useLocation()

    const [configName, setConfigName] = useState(location.configName)
    const [headers, setHeaders] = useState([])

    const updateManualHeaders = (state) => {
        const modified = JSON.parse(state).map((item, index) => {
            return {
                id: index,
                name: item,
                isClicked: false,
                index: false
            }
        })
        setHeaders(modified)
    }

    const updateCheckboxStatus = (id) => {
        let modified = [...headers]

        for (let i = 0; i < modified.length; i++) {
            if (+modified[i].id === +id) {
                modified[i].isClicked = !modified[i].isClicked
            }
        }
        setHeaders(modified)
        window.parent.postMessage({message: 'update_headers', content: JSON.stringify(modified)}, '*')
    }

    const resetIndexing = () => {
        for (let item of headers) {
            item.index = false
        }
    }

    let stopHeadersListener = () => {
        window.parent.postMessage({message: 'stop_headers_listener', content: ''}, '*')
    }

    return (
        <div>
            <div className={s.backToConfigContainer}>
                <NavLink to='/contentFrame' onClick={stopHeadersListener}>
                    <div className={s.backToConfig}>
                        <img src={arrowBack} alt="icn-arrow-back"/>
                        Back to Сonfig list
                    </div>
                </NavLink>
            </div>

            <div>
                <p className={s.pageName}>{ props.heading }</p>
                <Steps>
                    <Step component={Step2A} headers={headers} updateManualHeaders={updateManualHeaders}/>
                    <Step component={Step2B}/>
                    <Step component={Step2Final}/>
                    <Step component={Step3} headers={headers} updateCheckboxStatus={updateCheckboxStatus}/>
                    <Step component={Step4} name={configName} headers={headers} resetIndexing={resetIndexing}/>
                </Steps>
            </div>
        </div>
    )
}

export default ManualSetting
