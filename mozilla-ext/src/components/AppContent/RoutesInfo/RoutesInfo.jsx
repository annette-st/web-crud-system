import React from 'react'
import s from './RoutesInfo.module.css'
import ConfigDataContainer from "../ConfigDataContainer/ConfigDataContainer"
import {BrowserRouter, Route} from "react-router-dom"
import Redirect from "react-router-dom/es/Redirect"
import ManualSetting from "../CreateTableConfiguration/Step2/ManualSetting/ManualSetting"
import CreateConfigStep1 from "../CreateTableConfiguration/Step1/CreateConfigStep1"
import CreateConfigStep2 from "../CreateTableConfiguration/Step2/CreateConfigStep2"
import CreateConfigStep3 from "../CreateTableConfiguration/Step3/CreateConfigStep3"
import CreateTableConfiguration from "../CreateTableConfiguration/CreateTableConfiguration"
import ConfigurationSettings from "../CreateTableConfiguration/ConfigurationSettings/ConfigurationSettings";
import SequenceRecording from "../CreateTableConfiguration/ConfigurationSettings/SequenceRecording/SequenceRecording";


const RoutesInfo = (props) => {

    return (
        <BrowserRouter>
            <div className={s.contentWrapper}>
                <Redirect exact from="/" to="contentFrame" />

                <Route exact path='/contentFrame' render={ () => <ConfigDataContainer /> }/>
                <Route path='/manualSetting' render={ () => <ManualSetting /> }/>
                <Route path='/startTableConfig' render={ () => <CreateTableConfiguration /> }/>
                <Route path='/toStep1' render={ () => <CreateConfigStep1 /> }/>
                <Route path='/toStep2' render={ () => <CreateConfigStep2 /> }/>
                <Route path='/toStep3' render={ () => <CreateConfigStep3 /> }/>
                <Route path='/tableSettings' render={ () => <ConfigurationSettings /> }/>
                <Route path='/sequenceRecording' render={ () => <SequenceRecording /> }/>

            </div>
        </BrowserRouter>
    )
}

export default RoutesInfo
